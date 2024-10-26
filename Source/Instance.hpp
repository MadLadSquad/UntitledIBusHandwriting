#pragma once
#include <Framework.hpp>
#include <Generated/Config.hpp>
#include "MainView.hpp"
#include "Engine.hpp"
#include <mutex>
#include <thread>

namespace UntitledIBusHandwriting
{
    // This is the staging buffer for the thread, copies Instance::characters. Found it better than actually trying to
    // synchronise the threads
    inline std::vector<std::string> stagingBuffer;

    class UIMGUI_PUBLIC_API Instance final : public UImGui::Instance
    {
    public:
        Instance() noexcept;
        virtual void begin() noexcept override;
        virtual void tick(float deltaTime) noexcept override;
        virtual void end() noexcept override;
        virtual ~Instance() noexcept override = default;

        virtual void onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io) noexcept override;

        // This is only used for rendering
        std::vector<std::string> characters;
    private:
        // Tne ibus engine class. Functions from here are only called in the ibus thread
        Engine engine;

        // The thread where all ibus related functionality is handled
        std::thread ibusThread;
        MainView mainView;
    };
}

