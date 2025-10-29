#pragma once
#include "UI/Panel.h"
#include <string>
#include <vector>
#include <memory>
#include <imgui.h>

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
        
        ImVec4 avatarColor = ImVec4(0.5f, 0.5f, 0.8f, 1.0f);
        std::string avatarText;
    };

    struct Message
    {
        std::string sender;
        std::string text;
        std::string time;
        bool isOutgoing;
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
        void AddMockChats();
        
    private:
        std::string mPhoneNumber;
        std::string mDisplayName;
        std::vector<ChatInfo> mChats;
        bool mIsAuthorized = false;
        void* mTdlibClient = nullptr;
    };

    class ChatWindow
    {
    public:
        ChatWindow(const ChatInfo& chatInfo, const std::string& accountPhone);
        ~ChatWindow();

        void Render();
        bool IsOpen() const { return mIsOpen; }
        int64_t GetChatId() const { return mChatInfo.chatId; }

    private:
        ChatInfo mChatInfo;
        std::string mAccountPhone;
        bool mIsOpen = true;
        std::vector<char> mInputBuffer;
        std::vector<Message> mMessages;
        bool mScrollToBottom = false;
        
        void LoadMockMessages();
        void DrawMessage(const Message& msg);
    };
    
    class TGPanel : public Panel
    {
    public:
        TGPanel();
        ~TGPanel();
        
        void OnRender() override;
        void OnAttach() override;
        void OnDetach() override;
        void OnFocus() override;
        
        // Account management
        void AddAccount(const std::string& phoneNumber);
        void RemoveAccount(const std::string& phoneNumber);
        
    private:
        // UI State
        bool mShowAddAccountPopup = false;
        char mPhoneNumberBuffer[64] = {};
        char mSearchBuffer[256] = {};
        int mSelectedAccountIndex = -1;
        
        // Data
        std::vector<std::unique_ptr<TelegramAccount>> mAccounts;
        std::vector<std::unique_ptr<ChatWindow>> mChatWindows;
        
        // Internal methods
        void OpenChatWindow(const std::string& accountPhone, const ChatInfo& chat);
        void RenderAccountSection();
        void RenderAccountSelector();
        void RenderChatList();
        void RenderAddAccountButton();
        void RenderAddAccountPopup();
        void RenderEmptyState();
        void DrawChatItem(const ChatInfo& chat, bool isSelected);
        void DrawAvatar(const ChatInfo& chat, float size);
   
    };
}
