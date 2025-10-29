#include "UI/TabManager.h"

#include "Base/Log.h"

namespace tg
{
    void TabManager::Init()
    {
        TG(CoreLog, Info, "TabManager initialized")
    }

    void TabManager::Shutdown()
    {
        for (auto& panel : mPanels)
        {
            if (panel)
            {
                panel->OnDetach();
            }
        }
        mPanels.clear();
        mActivePanel = nullptr;
    }

    void TabManager::Render()
    {
        RenderDetachedWindows();
        
        bool hasAttachedPanels = std::ranges::any_of(mPanels,
                                                     [](const auto& panel) { return panel && !panel->IsDetached(); });
        
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | 
                                      ImGuiWindowFlags_NoCollapse | 
                                      ImGuiWindowFlags_NoScrollbar;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        
        if (ImGui::Begin("##MainTabArea", nullptr, windowFlags))
        {
            RenderTabBar();
            
            if (hasAttachedPanels)
            {
                // Content area
                ImGui::BeginChild("##TabContent", ImVec2(0, 0), false);
                {
                    RenderTabContent();
                }
                ImGui::EndChild();
            }
        }
        ImGui::End();
        
        ImGui::PopStyleVar();
    }

    void TabManager::AddPanel(std::shared_ptr<Panel> panel)
    {
        if (!panel) return;
        
        auto it = std::ranges::find_if(mPanels,
                                       [&panel](const auto& p) { return p && p->GetName() == panel->GetName(); });
        
        if (it == mPanels.end())
        {
            panel->OnAttach();
            mPanels.push_back(panel);
            
            if (!mActivePanel)
            {
                mActivePanel = panel.get();
                panel->OnFocus();
            }
            
            TG(CoreLog, Info, "Added panel: {}", panel->GetName())
        }
    }

    void TabManager::RemovePanel(const std::string& panelName)
    {
        auto it = std::ranges::find_if(mPanels,
                                       [&panelName](const auto& p) { return p && p->GetName() == panelName; });
        
        if (it != mPanels.end())
        {
            if (mActivePanel == it->get())
            {
                mActivePanel = nullptr;
                (*it)->OnLostFocus();
                
                auto nextIt = std::ranges::find_if(mPanels,
                                                   [&it](const auto& p) { return p && p != *it && !p->IsDetached(); });
                
                if (nextIt != mPanels.end())
                {
                    mActivePanel = nextIt->get();
                    (*nextIt)->OnFocus();
                }
            }
            
            (*it)->OnDetach();
            mPanels.erase(it);
        }
    }

    void TabManager::RemovePanel(Panel* panel)
    {
        if (!panel) return;
        RemovePanel(panel->GetName());
    }

    std::shared_ptr<Panel> TabManager::GetPanel(const std::string& panelName)
    {
        auto it = std::ranges::find_if(mPanels,
                                       [&panelName](const auto& p) { return p && p->GetName() == panelName; });
        
        return (it != mPanels.end()) ? *it : nullptr;
   
    }

    void TabManager::SetActivePanel(const std::string& panelName)
    {
        auto panel = GetPanel(panelName);
        if (panel)
        {
            SetActivePanel(panel.get());
        }
    }

    void TabManager::SetActivePanel(Panel* panel)
    {
        if (!panel || panel->IsDetached()) return;
        
        if (mActivePanel && mActivePanel != panel)
        {
            mActivePanel->OnLostFocus();
            mActivePanel->SetFocused(false);
        }
        
        mActivePanel = panel;
        panel->SetFocused(true);
        panel->OnFocus();
    }

    void TabManager::DetachPanel(Panel* panel)
    {
        if (!panel || panel->IsDetached()) return;
        
        panel->SetDetached(true);
        
        if (mActivePanel == panel)
        {
            mActivePanel = nullptr;
            panel->OnLostFocus();
            
            auto it = std::ranges::find_if(mPanels,
                                           [](const auto& p) { return p && !p->IsDetached(); });
            
            if (it != mPanels.end())
            {
                SetActivePanel(it->get());
            }
        }
    }

    void TabManager::AttachPanel(Panel* panel)
    {
        if (!panel || !panel->IsDetached()) return;
        
        panel->SetDetached(false);
        
        if (!mActivePanel)
        {
            SetActivePanel(panel);
        }
    }

    void TabManager::RenderTabBar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, mInactiveTabColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, mHoveredTabColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, mActiveTabColor);
        
        ImVec2 availSize = ImGui::GetContentRegionAvail();
        float totalTabWidth = availSize.x;
        if (mShowAddButton) totalTabWidth -= 30; 
        
        int attachedCount = 0;
        for (const auto& panel : mPanels)
        {
            if (panel && !panel->IsDetached())
                attachedCount++;
        }
        
        if (attachedCount > 0)
        {
            float tabWidth = std::min(200.0f, totalTabWidth / attachedCount);
            
            float currentX = 0;
            for (auto& panel : mPanels)
            {
                if (!panel || panel->IsDetached()) continue;
                
                ImGui::PushID(panel->GetUniqueID().c_str());
                
                bool isActive = (mActivePanel == panel.get());
                
                ImGui::SetCursorPosX(currentX);
                
                if (isActive)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, mActiveTabColor);
                }
                
                // Calculate actual tab width (with space for close button if needed)
                float actualTabWidth = tabWidth;
                bool hasCloseButton = (panel->GetFlags() & PanelFlags::CanClose);
                float closeButtonWidth = hasCloseButton ? 20.0f : 0.0f;
                
                // Tab button
                std::string tabLabel = panel->GetIcon().empty() ? 
                    panel->GetName() : 
                    panel->GetIcon() + " " + panel->GetName();
                    
                bool clicked = ImGui::Button(tabLabel.c_str(), 
                    ImVec2(actualTabWidth - closeButtonWidth - 2, mTabBarHeight));
                
                if (clicked)
                {
                    SetActivePanel(panel.get());
                }
                
                // Drag source for tab reordering
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    Panel* draggedPanel = panel.get();
                    ImGui::SetDragDropPayload("TAB_PANEL", &draggedPanel, sizeof(Panel*));
                    ImGui::Text("%s", panel->GetName().c_str());
                    ImGui::EndDragDropSource();
                }
                
                // Drop target for tab reordering
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TAB_PANEL"))
                    {
                        Panel* droppedPanel = *(Panel**)payload->Data;
                        if (droppedPanel && droppedPanel != panel.get())
                        {
                            // Find and swap panels
                            auto dragIt = std::find_if(mPanels.begin(), mPanels.end(),
                                [droppedPanel](const auto& p) { return p.get() == droppedPanel; });
                            auto dropIt = std::find_if(mPanels.begin(), mPanels.end(),
                                [&panel](const auto& p) { return p == panel; });
                            
                            if (dragIt != mPanels.end() && dropIt != mPanels.end())
                            {
                                std::swap(*dragIt, *dropIt);
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                
                // Close button
                if (hasCloseButton)
                {
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(currentX + actualTabWidth - closeButtonWidth);
                    
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.2f, 0.2f, 0.5f));
                    
                    if (ImGui::Button(("X##close" + panel->GetUniqueID()).c_str(), 
                        ImVec2(closeButtonWidth - 2, mTabBarHeight)))
                    {
                        panel->SetOpen(false);
                        RemovePanel(panel.get());
                    }
                    
                    ImGui::PopStyleColor(2);
                    ImGui::PopStyleVar();
                }
                
                // Right-click context menu
                if (ImGui::BeginPopupContextItem())
                {
                    if (panel->GetFlags() & PanelFlags::CanDetach)
                    {
                        if (ImGui::MenuItem("Detach"))
                        {
                            DetachPanel(panel.get());
                        }
                    }
                    
                    if (panel->GetFlags() & PanelFlags::CanClose)
                    {
                        if (ImGui::MenuItem("Close"))
                        {
                            panel->SetOpen(false);
                            RemovePanel(panel.get());
                        }
                    }
                    
                    if (ImGui::MenuItem("Close Others"))
                    {
                        // Close all other panels
                        for (auto it = mPanels.begin(); it != mPanels.end();)
                        {
                            if ((*it).get() != panel.get() && ((*it)->GetFlags() & PanelFlags::CanClose))
                            {
                                (*it)->SetOpen(false);
                                (*it)->OnDetach();
                                it = mPanels.erase(it);
                            }
                            else
                            {
                                ++it;
                            }
                        }
                    }
                    
                    ImGui::EndPopup();
                }
                
                if (isActive)
                {
                    ImGui::PopStyleColor();
                }
                
                currentX += actualTabWidth + 1; // +1 for spacing
                
                ImGui::PopID();
            }
            
            // Add button
            if (mShowAddButton)
            {
                ImGui::SetCursorPos(ImVec2(totalTabWidth, 0));
                
                if (ImGui::Button("+", ImVec2(25, mTabBarHeight)))
                {
                    ImGui::OpenPopup("AddPanelMenu");
                }
                
                if (ImGui::BeginPopup("AddPanelMenu"))
                {
                    // This should be customized by the application
                    ImGui::Text("Add Panel Menu");
                    ImGui::Separator();
                    ImGui::Text("Override this in your application");
                    ImGui::EndPopup();
                }
            }
        }
        else
        {
            // No panels message
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No panels open. Use View menu to add panels.");
        }
        
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
        
        ImGui::Separator();
    }

    void TabManager::RenderDetachedWindows()
    {
        std::vector<Panel*> panelsToRemove;
        
        for (auto& panel : mPanels)
        {
            if (!panel || !panel->IsDetached() || !panel->IsOpen()) continue;
            
            ImGuiWindowFlags windowFlags = 0;
            if (panel->GetFlags() & PanelFlags::MenuBar)
            {
                windowFlags |= ImGuiWindowFlags_MenuBar;
            }
            if (panel->GetFlags() & PanelFlags::NoScrollbar)
            {
                windowFlags |= ImGuiWindowFlags_NoScrollbar;
            }
            
            bool open = panel->IsOpen();
            std::string windowTitle = panel->GetName();
            if (!panel->GetIcon().empty())
            {
                windowTitle = panel->GetIcon() + " " + windowTitle;
            }
            windowTitle += "###" + panel->GetUniqueID();
            
            ImGui::SetNextWindowSize(panel->GetDetachedWindowSize(), ImGuiCond_FirstUseEver);
            
            if (ImGui::Begin(windowTitle.c_str(), &open, windowFlags))
            {
                // Add a small toolbar for window actions
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
                if (ImGui::Button("Attach"))
                {
                    AttachPanel(panel.get());
                }
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Right-click for options");
                ImGui::PopStyleColor();
                ImGui::Separator();
                
                // Context menu
                if (ImGui::BeginPopupContextWindow())
                {
                    if (ImGui::MenuItem("Attach to Main Window"))
                    {
                        AttachPanel(panel.get());
                    }
                    ImGui::Separator();
                    if (panel->GetFlags() & PanelFlags::CanClose)
                    {
                        if (ImGui::MenuItem("Close"))
                        {
                            open = false;
                        }
                    }
                    ImGui::EndPopup();
                }
                
                // Render panel content
                panel->OnRender();
            }
            ImGui::End();
            
            panel->SetOpen(open);
            if (!open)
            {
                panelsToRemove.push_back(panel.get());
            }
        }
        
        // Remove closed panels
        for (auto* panel : panelsToRemove)
        {
            RemovePanel(panel);
        }
    
    }

    void TabManager::RenderTabContent()
    {
        if (mActivePanel && !mActivePanel->IsDetached())
        {
            mActivePanel->OnRender();
        }
        else
        {
            // Empty state
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 textSize = ImGui::CalcTextSize("No active panel");
            ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.5f));
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No active panel");
        }
    }

    void TabManager::HandleTabDragDrop()
    {
    }
}
