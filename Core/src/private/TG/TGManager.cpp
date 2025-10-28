#include "TG/TGManager.h"

#include <algorithm>
#include <imgui.h>
#include <sstream>

namespace tg
{
    ////////////////////////////////////////////////////////
    ///              TelegramAccount
    ////////////////////////////////////////////////////////
    TelegramAccount::TelegramAccount(const std::string& phoneNumber)
    : mPhoneNumber(phoneNumber)
    {
        mDisplayName = "Account " + phoneNumber;
        AddMockChats(); // For testing
        mIsAuthorized = true; // Mock authorization
    }

    TelegramAccount::~TelegramAccount()
    {
    }

    void TelegramAccount::AddMockChats()
    {
        // Create mock data for testing
        std::vector<std::pair<std::string, std::string>> mockChats = {
            {"John Doe", "Hey, how are you?"},
            {"Alice Smith", "Can we meet tomorrow?"},
            {"Work Group", "Meeting at 3 PM"},
            {"Family", "Don't forget dinner tonight!"},
            {"Bob Johnson", "Thanks for your help!"},
            {"Project Team", "New updates available"},
            {"Sarah Connor", "I'll be back"},
            {"Gaming Squad", "Ready for tonight's raid?"},
            {"Book Club", "Next book: 1984"},
            {"Fitness Group", "Morning workout at 6 AM"}
        };
        
        for (size_t i = 0; i < mockChats.size(); ++i)
        {
            ChatInfo chat;
            chat.chatId = static_cast<int64_t>(i + 1);
            chat.title = mockChats[i].first;
            chat.lastMessage = mockChats[i].second;
            chat.lastMessageTime = (i < 3) ? "12:0" + std::to_string(i) : "Yesterday";
            chat.unreadCount = (i % 3 == 0) ? (i + 1) : 0;
            chat.isPinned = (i < 2);
            chat.isOnline = (i % 2 == 0);
            
            // Generate avatar text (first letters)
            std::string avatarText;
            std::istringstream iss(chat.title);
            std::string word;
            while (iss >> word && avatarText.length() < 2)
            {
                if (!word.empty())
                    avatarText += word[0];
            }
            chat.avatarText = avatarText;
            
            // Generate random-ish color based on name
            float hue = (float)(std::hash<std::string>{}(chat.title) % 360) / 360.0f;
            float r = std::abs(std::sin(hue * 2.0f * 3.14159f));
            float g = std::abs(std::sin((hue + 0.33f) * 2.0f * 3.14159f));
            float b = std::abs(std::sin((hue + 0.67f) * 2.0f * 3.14159f));
            chat.avatarColor = ImVec4(r * 0.7f + 0.3f, g * 0.7f + 0.3f, b * 0.7f + 0.3f, 1.0f);
            
            mChats.push_back(chat);
        }
    }

    ////////////////////////////////////////////////////////
    ///               ChatWindow
    ////////////////////////////////////////////////////////
    ChatWindow::ChatWindow(const ChatInfo& chatInfo, const std::string& accountPhone)
    : mChatInfo(chatInfo), mAccountPhone(accountPhone)
    {
        mInputBuffer.resize(256);
        LoadMockMessages();
    }

    ChatWindow::~ChatWindow()
    {
    }

    void ChatWindow::Render()
    {
        std::string windowTitle = mChatInfo.title + "###ChatWindow" + std::to_string(mChatInfo.chatId);
        
        ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
        
        if (!ImGui::Begin(windowTitle.c_str(), &mIsOpen))
        {
            ImGui::End();
            return;
        }
        
        // Chat header
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
        ImGui::BeginChild("##header", ImVec2(0, 50), true);
        {
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, 50);
            
            // Avatar
            ImGui::SetCursorPos(ImVec2(8, 8));
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetCursorScreenPos();
            drawList->AddCircleFilled(
                ImVec2(pos.x + 17, pos.y + 17), 17,
                ImColor(mChatInfo.avatarColor.x, mChatInfo.avatarColor.y, mChatInfo.avatarColor.z));
            
            ImVec2 textSize = ImGui::CalcTextSize(mChatInfo.avatarText.c_str());
            drawList->AddText(
                ImVec2(pos.x + 17 - textSize.x/2, pos.y + 17 - textSize.y/2),
                ImColor(1.0f, 1.0f, 1.0f),
                mChatInfo.avatarText.c_str());
            
            ImGui::NextColumn();
            
            // Name and status
            ImGui::SetCursorPosY(8);
            ImGui::Text("%s", mChatInfo.title.c_str());
            
            if (mChatInfo.isOnline)
            {
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "Online");
            }
            else
            {
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Last seen recently");
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        // Messages area
        float inputHeight = 50;
        ImGui::BeginChild("##messages", ImVec2(0, -inputHeight), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        {
            for (const auto& msg : mMessages)
            {
                DrawMessage(msg);
            }
            
            if (mScrollToBottom)
            {
                ImGui::SetScrollHereY(1.0f);
                mScrollToBottom = false;
            }
        }
        ImGui::EndChild();
        
        // Input area
        ImGui::Separator();
        ImGui::PushItemWidth(-60);
        
        if (ImGui::InputText("##input", mInputBuffer.data(), mInputBuffer.size(), 
                            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (strlen(mInputBuffer.data()) > 0)
            {
                // Add new message
                Message newMsg;
                newMsg.sender = "Me";
                newMsg.text = mInputBuffer.data();
                newMsg.time = "Now";
                newMsg.isOutgoing = true;
                mMessages.push_back(newMsg);
                
                // Clear input
                memset(mInputBuffer.data(), 0, mInputBuffer.size());
                mScrollToBottom = true;
            }
        }
        
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        if (ImGui::Button("Send", ImVec2(50, 0)))
        {
            if (strlen(mInputBuffer.data()) > 0)
            {
                Message newMsg;
                newMsg.sender = "Me";
                newMsg.text = mInputBuffer.data();
                newMsg.time = "Now";
                newMsg.isOutgoing = true;
                mMessages.push_back(newMsg);
                
                memset(mInputBuffer.data(), 0, mInputBuffer.size());
                mScrollToBottom = true;
            }
        }
        
        ImGui::End();
    }

    void ChatWindow::LoadMockMessages()
    {
        mMessages = {
            {"Me", "Hi there!", "10:30", true},
            {mChatInfo.title, "Hello! How are you?", "10:31", false},
            {"Me", "I'm doing great, thanks! How about you?", "10:32", true},
            {mChatInfo.title, "Pretty good! Working on some new projects.", "10:35", false},
            {"Me", "That sounds interesting!", "10:36", true},
            {mChatInfo.title, mChatInfo.lastMessage, "10:40", false}
        };
    }

    void ChatWindow::DrawMessage(const Message& msg)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
        
        float maxWidth = ImGui::GetContentRegionAvail().x * 0.75f;
        
        if (msg.isOutgoing)
        {
            // Right-aligned for outgoing messages
            float textWidth = ImGui::CalcTextSize(msg.text.c_str()).x + 16;
            float actualWidth = std::min(textWidth, maxWidth);
            ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - actualWidth);
            
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.3f, 0.5f, 0.3f, 0.3f));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 0.3f));
        }
        
        ImGui::BeginChild(("##msg" + std::to_string((intptr_t)&msg)).c_str(), 
                         ImVec2(maxWidth, 0), false, 
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
        
        if (!msg.isOutgoing)
        {
            ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "%s", msg.sender.c_str());
        }
        
        ImGui::TextWrapped("%s", msg.text.c_str());
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("%s", msg.time.c_str());
        ImGui::PopStyleColor();
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        
        ImGui::Spacing();
    }

    ////////////////////////////////////////////////////////
    ///               TGManager
    ////////////////////////////////////////////////////////

    TGManager& TGManager::Get()
    {
        static TGManager manager;
        return manager;
    }
    
    void tg::TGManager::Init()
    {
        ApplyTelegramStyle();
        //TODO: Init tdlib here
    }

    void tg::TGManager::Shutdown()
    {
        mChatWindows.clear();
        mAccounts.clear();
    }

    void tg::TGManager::Render()
    {
        RenderMainPanel();
        
        // Render chat windows
        for (auto it = mChatWindows.begin(); it != mChatWindows.end();)
        {
            if (!(*it)->IsOpen())
            {
                it = mChatWindows.erase(it);
            }
            else
            {
                (*it)->Render();
                ++it;
            }
        }
        
        RenderAddAccountPopup();
    }

    void TGManager::AddAccount(const std::string& phoneNumber)
    {
        // Check if account already exists
        for (const auto& account : mAccounts)
        {
            if (account->GetPhoneNumber() == phoneNumber)
                return;
        }
        
        mAccounts.push_back(std::make_unique<TelegramAccount>(phoneNumber));
        
        // Select the newly added account
        mSelectedAccountIndex = static_cast<int>(mAccounts.size()) - 1;
    
    }

    void TGManager::RemoveAccount(const std::string& phoneNumber)
    {
        auto it = std::remove_if(mAccounts.begin(), mAccounts.end(),
            [&phoneNumber](const std::unique_ptr<TelegramAccount>& account) {
                return account->GetPhoneNumber() == phoneNumber;
            });
        
        if (it != mAccounts.end())
        {
            mAccounts.erase(it, mAccounts.end());
            
            // Adjust selected index
            if (mSelectedAccountIndex >= static_cast<int>(mAccounts.size()))
            {
                mSelectedAccountIndex = static_cast<int>(mAccounts.size()) - 1;
            }
        }
    }

    void TGManager::OpenChatWindow(const std::string& accountPhone, const ChatInfo& chat)
    {
        // Check if window already exists
        for (const auto& window : mChatWindows)
        {
            if (window->GetChatId() == chat.chatId)
            {
                return; // Window already open
            }
        }
        
        mChatWindows.push_back(std::make_unique<ChatWindow>(chat, accountPhone));
   
    }

    void TGManager::RenderMainPanel()
    {
        // Main non-dockable window
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | 
                                       ImGuiWindowFlags_NoMove | 
                                       ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus;
        
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + 20, viewport->Pos.y + 50));
        ImGui::SetNextWindowSize(ImVec2(350, viewport->Size.y - 100));
        
        ImGui::Begin("Telegram", nullptr, windowFlags);
        
        if (mAccounts.empty())
        {
            RenderEmptyState();
        }
        else
        {
            RenderAccountSelector();
            ImGui::Separator();
            RenderChatList();
        }
        
        ImGui::End();
    }

    void TGManager::RenderAccountSelector()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        
        if (mSelectedAccountIndex >= 0 && mSelectedAccountIndex < mAccounts.size())
        {
            const auto& account = mAccounts[mSelectedAccountIndex];
            
            ImGui::Text("Account: %s", account->GetDisplayName().c_str());
            
            if (ImGui::BeginCombo("##accountSelector", account->GetPhoneNumber().c_str()))
            {
                for (size_t i = 0; i < mAccounts.size(); ++i)
                {
                    bool isSelected = (mSelectedAccountIndex == i);
                    if (ImGui::Selectable(mAccounts[i]->GetPhoneNumber().c_str(), isSelected))
                    {
                        mSelectedAccountIndex = static_cast<int>(i);
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            
            ImGui::SameLine();
            RenderAddAccountButton();
        }
        
        ImGui::PopStyleVar();
    }

    void TGManager::RenderChatList()
    {
        if (mSelectedAccountIndex < 0 || mSelectedAccountIndex >= mAccounts.size())
            return;
        
        const auto& account = mAccounts[mSelectedAccountIndex];
        
        // Search bar
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
        ImGui::InputText("##search", mSearchBuffer.data(), 256, ImGuiInputTextFlags_None);
        ImGui::PopStyleVar();
        
        if (ImGui::IsItemHovered() && strlen(mSearchBuffer.data()) == 0)
        {
            ImGui::SetTooltip("Search chats...");
        }
        
        ImGui::Separator();
        
        // Chat list
        ImGui::BeginChild("##chatList", ImVec2(0, 0), false);
        
        const auto& chats = account->GetChats();
        
        // Sort chats: pinned first, then by time
        std::vector<ChatInfo> sortedChats = chats;
        std::sort(sortedChats.begin(), sortedChats.end(),
            [](const ChatInfo& a, const ChatInfo& b) {
                if (a.isPinned != b.isPinned) return a.isPinned > b.isPinned;
                return a.chatId < b.chatId; // Normally sort by last message time
            });
        
        for (const auto& chat : sortedChats)
        {
            // Filter by search
            if (strlen(mSearchBuffer.data()) > 0)
            {
                std::string searchLower = mSearchBuffer.data();
                std::ranges::transform(searchLower, searchLower.begin(), ::tolower);
                
                std::string titleLower = chat.title;
                std::ranges::transform(titleLower, titleLower.begin(), ::tolower);
                
                if (titleLower.find(searchLower) == std::string::npos)
                    continue;
            }
            
            DrawChatItem(chat, false);
        }
        
        ImGui::EndChild();
    }

    void TGManager::RenderAddAccountButton()
    {
        if (ImGui::Button("+"))
        {
            mShowAddAccountPopup = true;
        }
        
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Add new account");
        }
    }

    void TGManager::RenderAddAccountPopup()
    {
        if (mShowAddAccountPopup)
        {
            ImGui::OpenPopup("Add Telegram Account");
            mShowAddAccountPopup = false;
        }
        
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(300, 150));
        
        if (ImGui::BeginPopupModal("Add Telegram Account", nullptr, ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Enter phone number:");
            ImGui::InputText("##phone", mPhoneNumberBuffer, 64);
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            if (ImGui::Button("Add Account", ImVec2(120, 0)))
            {
                if (strlen(mPhoneNumberBuffer) > 0)
                {
                    AddAccount(mPhoneNumberBuffer);
                    memset(mPhoneNumberBuffer, 0, sizeof(mPhoneNumberBuffer));
                    ImGui::CloseCurrentPopup();
                }
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
    
    }

    void TGManager::RenderEmptyState()
    {
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 center(windowSize.x * 0.5f, windowSize.y * 0.5f);
        
        // Center the content
        ImGui::SetCursorPos(ImVec2(center.x - 50, center.y - 100));
        
        // Draw large + button
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 buttonCenter = ImGui::GetCursorScreenPos();
        buttonCenter.x += 50;
        buttonCenter.y += 50;
        
        // Draw circle button
        if (ImGui::InvisibleButton("##addFirstAccount", ImVec2(100, 100)))
        {
            mShowAddAccountPopup = true;
        }
        
        bool isHovered = ImGui::IsItemHovered();
        
        drawList->AddCircleFilled(buttonCenter, 50, 
            isHovered ? ImColor(0.3f, 0.6f, 0.9f, 1.0f) : ImColor(0.26f, 0.59f, 0.98f, 1.0f));
        
        // Draw + sign
        float crossSize = 25;
        drawList->AddLine(
            ImVec2(buttonCenter.x - crossSize, buttonCenter.y),
            ImVec2(buttonCenter.x + crossSize, buttonCenter.y),
            ImColor(1.0f, 1.0f, 1.0f), 3.0f);
        drawList->AddLine(
            ImVec2(buttonCenter.x, buttonCenter.y - crossSize),
            ImVec2(buttonCenter.x, buttonCenter.y + crossSize),
            ImColor(1.0f, 1.0f, 1.0f), 3.0f);
        
        // Add text below
        ImGui::SetCursorPos(ImVec2(center.x - 60, center.y + 60));
        ImGui::Text("Add Telegram Account");
    }

    void TGManager::DrawChatItem(const ChatInfo& chat, bool isSelected)
    {
        ImGui::PushID(chat.chatId);
        
        ImVec2 cursorPos = ImGui::GetCursorPos();
        bool clicked = ImGui::Selectable("##chat", isSelected, 
                                        ImGuiSelectableFlags_AllowDoubleClick, 
                                        ImVec2(0, 60));
        
        if (clicked && ImGui::IsMouseDoubleClicked(0))
        {
            // Open chat window
            if (mSelectedAccountIndex >= 0 && mSelectedAccountIndex < mAccounts.size())
            {
                OpenChatWindow(mAccounts[mSelectedAccountIndex]->GetPhoneNumber(), chat);
            }
        }
        
        ImGui::SetCursorPos(cursorPos);
        
        // Draw avatar
        DrawAvatar(chat, 40);
        
        ImGui::SameLine();
        ImGui::BeginGroup();
        
        // First line: name and time
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x - 50);
        
        if (chat.isPinned)
        {
            ImGui::Text("📌 %s", chat.title.c_str());
        }
        else
        {
            ImGui::Text("%s", chat.title.c_str());
        }
        
        ImGui::NextColumn();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", chat.lastMessageTime.c_str());
        ImGui::Columns(1);
        
        // Second line: message and unread count
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x - 30);
        
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", chat.lastMessage.c_str());
        
        ImGui::NextColumn();
        
        if (chat.unreadCount > 0)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetCursorScreenPos();
            float radius = 10;
            
            drawList->AddCircleFilled(
                ImVec2(pos.x + radius, pos.y + radius),
                radius,
                ImColor(0.26f, 0.59f, 0.98f));
            
            std::string countStr = std::to_string(chat.unreadCount);
            ImVec2 textSize = ImGui::CalcTextSize(countStr.c_str());
            drawList->AddText(
                ImVec2(pos.x + radius - textSize.x/2, pos.y + radius - textSize.y/2),
                ImColor(1.0f, 1.0f, 1.0f),
                countStr.c_str());
        }
        
        ImGui::Columns(1);
        ImGui::EndGroup();
        
        ImGui::Separator();
        
        ImGui::PopID();
    }

    void TGManager::DrawAvatar(const ChatInfo& chat, float size)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float radius = size / 2;
        
        // Draw circle with chat color
        drawList->AddCircleFilled(
            ImVec2(pos.x + radius, pos.y + radius),
            radius,
            ImColor(chat.avatarColor.x, chat.avatarColor.y, chat.avatarColor.z));
        
        // Draw initials
        ImVec2 textSize = ImGui::CalcTextSize(chat.avatarText.c_str());
        drawList->AddText(
            ImVec2(pos.x + radius - textSize.x/2, pos.y + radius - textSize.y/2),
            ImColor(1.0f, 1.0f, 1.0f),
            chat.avatarText.c_str());
        
        // Online indicator
        if (chat.isOnline)
        {
            float indicatorRadius = 6;
            ImVec2 indicatorPos = ImVec2(pos.x + size - indicatorRadius, pos.y + size - indicatorRadius);
            drawList->AddCircleFilled(indicatorPos, indicatorRadius, ImColor(0.0f, 0.0f, 0.0f));
            drawList->AddCircleFilled(indicatorPos, indicatorRadius - 2, ImColor(0.4f, 0.8f, 0.4f));
        }
        
        ImGui::Dummy(ImVec2(size, size));
    }

    void TGManager::ApplyTelegramStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        
        // Telegram-like colors
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.082f, 0.086f, 0.090f, 1.0f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.106f, 0.110f, 0.114f, 1.0f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 0.5f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.082f, 0.086f, 0.090f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.082f, 0.086f, 0.090f, 1.0f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.082f, 0.086f, 0.090f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.4f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.6f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.8f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 0.5f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.3f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.5f);
        
        style.WindowRounding = 8.0f;
        style.FrameRounding = 4.0f;
        style.ScrollbarRounding = 12.0f;
        style.GrabRounding = 4.0f;
        style.WindowBorderSize = 0.0f;
        style.FrameBorderSize = 0.0f;
    }
}



