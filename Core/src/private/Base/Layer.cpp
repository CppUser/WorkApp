#include "Base/Layer.h"
#include <ranges>
#include <utility>

namespace tg
{
    LayerStack::~LayerStack() {
        for (const auto & mLayer : std::ranges::reverse_view(mLayers))
        {
            if (mLayer)
            {
                mLayer->OnDetach();
            }
        }
        mLayers.clear();
    }

    void LayerStack::PushLayer(Layer *layer) {
        mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
        mLayerInsertIndex++;
        layer->OnAttach();
    }

    void LayerStack::PushOverlay(Layer *overlay) {
        mLayers.emplace_back(overlay);
        overlay->OnAttach();
    }

    void LayerStack::PopLayer(Layer* layer) {
        const auto it = std::find(mLayers.begin(), mLayers.begin() + mLayerInsertIndex, layer);

        if (it != mLayers.begin() + mLayerInsertIndex)
        {
            layer->OnDetach();
            mLayers.erase(it);
            mLayerInsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay) {
        const auto it = std::find(mLayers.begin() + mLayerInsertIndex, mLayers.end(), overlay);

        if (it != mLayers.end())
        {
            overlay->OnDetach();
            mLayers.erase(it);
        }
    }
}
