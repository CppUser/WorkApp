#pragma once
#include <string>
#include <memory>
#include <imgui.h>

namespace tg
{
    enum class PanelFlags
    {
        None = 0,
        CanClose = 1 << 0,
        CanDetach = 1 << 1,
        NoScrollbar = 1 << 2,
        MenuBar = 1 << 3,
    };

    inline PanelFlags operator|(PanelFlags a, PanelFlags b)
    {
        return static_cast<PanelFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline bool operator&(PanelFlags a, PanelFlags b)
    {
        return (static_cast<int>(a) & static_cast<int>(b)) != 0;
    }

    
    class Panel
    {
    public:
        Panel(const std::string& name, const std::string& icon = "")
        : mName(name), mIcon(icon), mID(sNextID++)
        {
            mUniqueID = mName + "##Panel" + std::to_string(mID);
        }
        virtual ~Panel() = default;

        virtual void OnRender() = 0;
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        
        virtual void OnFocus() {}
        virtual void OnLostFocus() {}

        void SetFlags(PanelFlags flags) { mFlags = flags; }
        PanelFlags GetFlags() const { return mFlags; }
        
        const std::string& GetName() const { return mName; }
        const std::string& GetIcon() const { return mIcon; }
        const std::string& GetUniqueID() const { return mUniqueID; }
        
        bool IsOpen() const { return mIsOpen; }
        void SetOpen(bool open) { mIsOpen = open; }
        
        bool IsFocused() const { return mIsFocused; }
        void SetFocused(bool focused) { mIsFocused = focused; }
        
        bool IsDetached() const { return mIsDetached; }
        void SetDetached(bool detached) { mIsDetached = detached; }
        
        void SetDetachedWindowSize(const ImVec2& size) { mDetachedWindowSize = size; }
        const ImVec2& GetDetachedWindowSize() const { return mDetachedWindowSize; }
        
    protected:
        std::string mName;
        std::string mIcon;
        std::string mUniqueID;
        uint32_t mID;
        
        bool mIsOpen = true;
        bool mIsFocused = false;
        bool mIsDetached = false;
        
        PanelFlags mFlags = PanelFlags::CanClose | PanelFlags::CanDetach;
        ImVec2 mDetachedWindowSize = ImVec2(400, 600);
        
    private:
        static uint32_t sNextID;
        
    };
}
