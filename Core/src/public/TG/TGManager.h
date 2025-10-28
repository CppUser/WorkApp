#pragma once
#include <string>
#include <imgui.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace tg
{
    struct ChatInfo
    {
        int64_t chatId;
        std::string title;
        std::string lastMessage;
        std::string lastMessageTime;
        int unreadCount = 0;
        bool isPinned = false;
        bool isOnline = false;
        
        // Avatar data (placeholder for now, can be loaded from file/memory)
        ImVec4 avatarColor = ImVec4(0.5f, 0.5f, 0.8f, 1.0f); // Default blue color
        std::string avatarText; // First letters of name
    };

    class TelegramAccount
    {
    public:
        TelegramAccount(const std::string& phoneNumber);
        ~TelegramAccount();

        const std::string& GetPhoneNumber() const { return mPhoneNumber; }
        const std::string& GetDisplayName() const { return mDisplayName; }
        const std::vector<ChatInfo>& GetChats() const { return mChats; }
        bool IsAuthorized() const { return mIsAuthorized; }
        
        void SetDisplayName(const std::string& name) { mDisplayName = name; }
        void AddMockChats(); // For testing without real TDLib connection
        
    private:
        std::string mPhoneNumber;
        std::string mDisplayName;
        std::vector<ChatInfo> mChats;
        bool mIsAuthorized = false;
        void* mTdlibClient = nullptr; // Will hold TDLib client pointer
    };

    class ChatWindow
    {
    public:
        ChatWindow(const ChatInfo& chatInfo, const std::string& accountPhone);
        ~ChatWindow();

        void Render();
        bool IsOpen() const { return mIsOpen; }
        int64_t GetChatId() const { return mChatInfo.chatId; }
        
        struct Message
        {
            std::string sender;
            std::string text;
            std::string time;
            bool isOutgoing;
        };

    private:
        ChatInfo mChatInfo;
        std::string mAccountPhone;
        bool mIsOpen = true;
        std::string mInputBuffer;
        std::vector<Message> mMessages;
        bool mScrollToBottom = false;
        
        void LoadMockMessages(); // For testing
        void DrawMessage(const Message& msg);
    };

    

    
    class TGManager
    {
    public:
        static TGManager& Get();
        
        void Init();
        void Shutdown();
        void Render();
        
        void AddAccount(const std::string& phoneNumber);
        void RemoveAccount(const std::string& phoneNumber);
        void OpenChatWindow(const std::string& accountPhone, const ChatInfo& chat);
        
    private:
        TGManager() = default;
        ~TGManager() = default;
        
        // UI State
        bool mShowAddAccountPopup = false;
        char mPhoneNumberBuffer[64] = {};
        int mSelectedAccountIndex = -1;
        std::string mSearchBuffer;
        
        // Data
        std::vector<std::unique_ptr<TelegramAccount>> mAccounts;
        std::vector<std::unique_ptr<ChatWindow>> mChatWindows;
        
        // UI Methods
        void RenderMainPanel();
        void RenderAccountSelector();
        void RenderChatList();
        void RenderAddAccountButton();
        void RenderAddAccountPopup();
        void RenderEmptyState();
        void DrawChatItem(const ChatInfo& chat, bool isSelected);
        void DrawAvatar(const ChatInfo& chat, float size);
        
        // Styling
        void ApplyTelegramStyle();
    };

    
}
