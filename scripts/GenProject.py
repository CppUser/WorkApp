import argparse
from pathlib import Path
import logging
import sys
from typing import Optional
import platform
import subprocess
from urllib.parse import urlparse
from urllib.request import urlretrieve
import zipfile
import os

try:
    import requests
    from importlib import metadata
    from colorama import Fore, Style, init as colorama_init
except ImportError as e:
    print(f"Missing required packages. Installing dependencies....")
    subprocess.check_call([sys.executable, '-m', 'pip','install', 'requests', 'colorama'])
    
    import requests
    from colorama import Fore, Style, init as colorama_init
    from importlib import metadata


colorama_init(autoreset=True)

class ProjectBuilder:
    PREMAKE_URLS = {
        "Windows": "https://github.com/premake/premake-core/releases/download/v5.0.0-beta7/premake-5.0.0-beta7-windows.zip",
    }
    
    def __init__(self,project_root: Optional[Path] = None):
        self.script_dir = Path(__file__).resolve().parent
        self.project_root = project_root or self.script_dir.parent
        self.third_party_dir = self.script_dir / "third_party"
        self.premake_dir = self.third_party_dir / "premake"
        
        #TODO: Обозначить правильное имя для других платформ
        self.premake_exe = self.premake_dir / (
            "premake5.exe" if platform.system() == "Windows" else "premake5"
        )
        
        self._setup_logging()
        self._ensure_directories()
        
    def _setup_logging(self) -> None:
        file_handler = logging.FileHandler(
            self.script_dir / "build.log", 
            encoding='utf-8'
        )
        
        console_handler = logging.StreamHandler(sys.stdout)
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        file_handler.setFormatter(formatter)
        console_handler.setFormatter(formatter)
        
        self.logger = logging.getLogger(__name__)
        self.logger.setLevel(logging.INFO)
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)
        
        self.logger.propagate = False
        
    def _ensure_directories(self) -> None:
        for directory in [self.third_party_dir,self.premake_dir]:
            directory.mkdir(parents=True,exist_ok=True)
        
    def _print_status(self, message: str, color: str = Fore.WHITE, style: str = Style.NORMAL) -> None:
        print(f"{style}{color}{message}{Style.RESET_ALL}")
        clean_message = message.replace("✅", "[SUCCESS]").replace("ℹ️", "[INFO]").replace("❌", "[ERROR]")
        self.logger.info(clean_message.strip())
        
    def _print_success(self, message: str) -> None:
        self._print_status(f" ✅ {message}",Fore.GREEN,Style.BRIGHT)
        
    def _print_info(self, message: str) -> None:
        self._print_status(f" ℹ️ {message}",Fore.CYAN)
    
    def _print_error(self, message: str) -> None:
        self._print_status(f" ❌ ERROR: {message}",Fore.RED,Style.BRIGHT)
            
    def validate_package(self,package: str) -> bool:
        try:
            metadata.version(package)
            return True
        except metadata.PackageNotFoundError:
            self._print_info(f"Installing Python package: {package}")
            try:
                subprocess.check_call([
                    sys.executable, '-m', 'pip','install', package
                ], stdout=subprocess.DEVNULL,stderr=subprocess.PIPE)
                return True
            except subprocess.CalledProcessError as e:
                self._print_error(f"Failed to install {package}: {e}")
                return False
            
    def validate_python_dependencies(self) -> bool:
        required_packages = ['requests','colorama']
        all_valid = True
        
        for package in required_packages:
            if not self.validate_package(package):
                all_valid = False
                
        return all_valid
    
    def download_file(self,url: str, destination: Path, description: str = "file") ->bool:
        try:
            self._print_info(f"Downloading {description} from {url}")
            
            response = requests.get(url,stream=True)
            response.raise_for_status()
            
            total_size = int(response.headers.get('content-length', 0))
            downloaded = 0
            
            with open(destination,'wb') as f:
                for chunk in response.iter_content(chunk_size=8192):
                    if chunk:
                        f.write(chunk)
                        downloaded += len(chunk)
                        if total_size > 0:
                            progress = (downloaded / total_size) * 100
                            print(f"\r 📥 Progress: {progress:.1f}%", end='',flush=True)
            print() #New line
            return True
        except Exception as e:
            self._print_error(f"Failed to download {description}: {e}")
            return False
        
    def extract_archive(self,archive_path: Path,destination: Path) -> bool:
        try:
            if archive_path.suffix.lower() == '.zip':
                with zipfile.ZipFile(archive_path,'r') as zip_ref:
                    zip_ref.extractall(destination)
            #else:
                #TODO: Необходжимо прописать для других платформ (tar for Linux ? Mac ?)
                
            return True
        except Exception as e:
            self._print_error(f"Failed to extract {archive_path}: {e}")
            return False   
    
                    
    def download_premake(self) -> bool:
        system = platform.system()
        if system not in self.PREMAKE_URLS:
            self._print_error(f"Unsupported platform: {system}")
            return False
        
        url = self.PREMAKE_URLS[system]
        filename = Path(urlparse(url).path).name
        archive_path = self.third_party_dir / filename
        
        self._print_info("Premake5 not found. Downloading...")
        if not self.download_file(url,archive_path,"Premake5"):
            return False
        
        if not self.extract_archive(archive_path,self.premake_dir):
            return False
        
        archive_path.unlink()
        
        #if system != "Windows":
        #TODO: Linux chmod(0o755) ???    
        
        if self.premake_exe.exists():
            self._print_success("Premake downloaded and Installed successfully")
            return True
        else:
            self._print_error("Premake executable not found after installation")
            return False
    
        
    def check_premake(self) -> bool:
        if self.premake_exe.exists():
            self._print_info(f"Premake found")
            return True
        return self.download_premake()
    
    def generate_project_files(self, action: str = "vs2022") -> bool:
        self._print_info(f"Generating project files with action: {action}")
        
        original_cwd = os.getcwd()
        try:
            os.chdir(self.project_root)
            
            result = subprocess.run(
                [str(self.premake_exe), action],
                capture_output=True,
                text=True,
                timeout=300
            )
            if result.returncode == 0:
                self._print_success("Project files generated successfully!")
                if result.stdout.strip():
                    print(result.stdout)
                    return True
            else:
                self._print_error(f"Premake failed with return code {result.returncode}")
                print("=== Premake STDOUT ===")
                print(result.stdout)
                print("=== Premake STDERR ===")
                print(result.stderr)
                return False
            
        except subprocess.TimeoutExpired:
            self._print_error("Premake execution time out")
            return False
        except Exception as e:
            self._print_error(f"Failed to execute premake: {e}")
            return False
        finally:
            os.chdir(original_cwd)
    
    
    def build_project(self,action: str = "vs2022") -> bool:
        self._print_info("Starting project build process....")
        
        if not self.validate_python_dependencies():
            self._print_error("Failed validate python dependencies")
            
        if not self.check_premake():
            self._print_error("Failed to setup Premake5")
            return False
        
        if not self.generate_project_files(action):
            return False
        
        self._print_success("Build process completed!")
        return True
        

def main():
    parser = argparse.ArgumentParser(
        description= "Build script for Ashborn",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""%(prog)s"""
    )
    
    parser.add_argument(
        '--action','-a',
        default='vs2022',
        help='Premake action to exeec vs2022'
    )
    
    parser.add_argument(
        '--project-root','-p',
        type=Path,
        help='Project root directory (default: parent of script directory)'
    )
    
    parser.add_argument(
        '--verbose','-v',
        action='store_true',
        help='Enable verbose logging'
    )
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
        
        
    try:
        builder = ProjectBuilder(args.project_root)
        
        success = builder.build_project(
            action=args.action,
        )
        
        sys.exit(0 if success else 1)
    except KeyboardInterrupt:
        print(f"\n{Style.BRIGHT}{Fore.YELLOW}Build canceled{Style.RESET_ALL}")
        sys.exit(1)
    except Exception as e:
        print(f"\n{Style.BRIGHT}{Fore.RED}Unexpected error: {e}{Style.RESET_ALL}")
        sys.exit(1)


if __name__ == '__main__':
    main()