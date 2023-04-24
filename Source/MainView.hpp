#pragma once
#include <Framework.hpp>

namespace UntitledIBusHandwriting
{
    class UIMGUI_PUBLIC_API MainView : public UImGui::InlineComponent
    {
    public:
        MainView();
        virtual void begin() override;
        virtual void tick(float deltaTime) override;
        virtual void end() override;
        virtual ~MainView() override;
    private:
        void* lib;
        std::function<const char*(size_t, const char*)> loadData;
        std::function<void(const char*)> deallocateLoadedData;
    };
}

