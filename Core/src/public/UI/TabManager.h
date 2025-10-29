#pragma once
#include "Panel.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace tg
{
    class TabManager
    {
    public:
        static TabManager& Get()
        {
            static TabManager instance;
            return instance;
        }
        
        void Init();
        void Shutdown();
        void Render();
        
        void AddPanel(std::shared_ptr<Panel> panel);
        void RemovePanel(const std::string& panelName);
        void RemovePanel(Panel* panel);
        
        std::shared_ptr<Panel> GetPanel(const std::string& panelName);
        void SetActivePanel(const std::string& panelName);
        void SetActivePanel(Panel* panel);
        
        void DetachPanel(Panel* panel);
        void AttachPanel(Panel* panel);
        
    private:
        TabManager() = default;
        ~TabManager() = default;
        
        void RenderTabBar();
        void RenderDetachedWindows();
        void RenderTabContent();
        void HandleTabDragDrop();

    private:
        std::vector<std::shared_ptr<Panel>> mPanels;
        Panel* mActivePanel = nullptr;
        Panel* mDraggingTab = nullptr;
        
        float mTabBarHeight = 25.0f;
        ImVec4 mActiveTabColor = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
        ImVec4 mInactiveTabColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        ImVec4 mHoveredTabColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        
        bool mShowAddButton = true;
        bool mTabsReorderable = true;
    };
}
