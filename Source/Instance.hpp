#pragma once
#include <Framework.hpp>
#include "MainView.hpp"
#include "Engine.hpp"
#include <mutex>

namespace UntitledIBusHandwriting
{
    // This is the staging buffer for the thread, copies Instance::characters. Found it better than actually trying to
    // synchronise the threads
    inline std::vector<std::string> stagingBuffer;

    class UIMGUI_PUBLIC_API Instance : public UImGui::Instance
    {
    public:
        Instance();
        virtual void begin() override;
        virtual void tick(float deltaTime) override;
        virtual void end() override;
        virtual ~Instance() override;

        virtual void onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io) override;

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

