#pragma once
#include <string>
#include "Time.h"

namespace tg
{
    
    class Layer
    {
    public:
        explicit Layer(std::string  name = "Layer")
                : mName(std::move(name)){}
        virtual ~Layer() = default;

        [[nodiscard]] const std::string& GetName() const { return mName; }

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(TimeStep ts) {}
        virtual void OnImGuiRender() {}
        //TODO:virtual bool OnEvent(Event& event) {return false;}

        void SetActive(const bool active) { mIsActive = active; }
        [[nodiscard]] bool IsActive() const { return mIsActive; }

        void SetVisible(const bool visible) { mIsVisible = visible; }
        [[nodiscard]] bool IsVisible() const { return mIsVisible; }

        void SetEventPriority(const int priority) { mEventPriority = priority; }
        [[nodiscard]] int GetEventPriority() const { return mEventPriority; }
        
    protected:
        bool mIsActive = true;
        bool mIsVisible = true;
        int mEventPriority = 0;
    private:
        std::string mName;
    };

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(Layer* layerPtr);
        void PopOverlay(Layer* overlayPtr);


        // Note: These iterators will iterate over all layers, including disabled ones.
        // The App loop will need to check IsActive().
        //TODO: Create custom iterators that skip disabled layers.
        using LayerIterator = std::vector<Layer*>::iterator;
        using ConstLayerIterator = std::vector<Layer*>::const_iterator;

        LayerIterator begin() { return mLayers.begin(); }
        LayerIterator end() { return mLayers.end(); }
        [[nodiscard]] ConstLayerIterator begin() const { return mLayers.begin(); }
        [[nodiscard]] ConstLayerIterator end() const { return mLayers.end(); }

        // Reverse iterators for event handling
        std::vector<Layer*>::reverse_iterator rbegin() { return mLayers.rbegin(); }
        std::vector<Layer*>::reverse_iterator rend() { return mLayers.rend(); }

        [[nodiscard]] size_t Size() const { return mLayers.size(); }
        [[nodiscard]] bool IsEmpty() const { return mLayers.empty(); }
        [[nodiscard]] unsigned int GetLayerInsertIndex() const { return mLayerInsertIndex; }

        void Clear() {
            mLayers.clear();
            mLayerInsertIndex = 0;
        }
    private:
        std::vector<Layer*> mLayers;
        unsigned int mLayerInsertIndex = 0;
    };

}
