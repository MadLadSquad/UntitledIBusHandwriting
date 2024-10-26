#pragma once
#include <Framework.hpp>

namespace UntitledIBusHandwriting
{
    class Instance;
    class UIMGUI_PUBLIC_API MainView final : public UImGui::InlineComponent
    {
    public:
        MainView() noexcept = default;
        virtual void begin() noexcept override;
        virtual void tick(float deltaTime) noexcept override;
        virtual void end() noexcept override;
        virtual ~MainView() noexcept override = default;
    private:
        Instance* data = nullptr;

        void* lib;
        std::function<const char*(size_t, const char*)> loadData;
        std::function<void(const char*)> deallocateLoadedData;
    };
}

