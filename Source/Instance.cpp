#include "Instance.hpp"
#include "Engine.hpp"

UntitledIBusHandwriting::Instance::Instance()
{
    // TODO: Remove this in production
    chdir("/home/i-use-gentoo-btw/Documents/UntitledImGuiFramework/Projects/UntitledIBusHandwriting/cmake-build-relwithdebinfo/");
    initInfo.inlineComponents = { &mainView };
    initInfo.globalData = this;
    initInfo.bGlobalAllocatedOnHeap = false;
}

void UntitledIBusHandwriting::Instance::begin()
{
    beginAutohandle();
    UImGui::Window::Platform::setWindowAlwaysOnTop();
    UImGui::Window::Platform::setWindowType(X11_WINDOW_TYPE_SPLASH);

    gboolean bIBusAvailable = false;

    if (arguments.size() > 1 && arguments[1] == "--ibus")
        bIBusAvailable = true;

    ibusThread = std::thread([&](){
        engine.initIBus(arguments[0].data(), bIBusAvailable);
    });
}

void UntitledIBusHandwriting::Instance::tick(float deltaTime)
{
    tickAutohandle(deltaTime);
}

void UntitledIBusHandwriting::Instance::end()
{
    endAutohandle();
    ibus_quit();
    ibusThread.join();
}

UntitledIBusHandwriting::Instance::~Instance()
{

}

void UntitledIBusHandwriting::Instance::onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io)
{
    io.Fonts->AddFontFromFileTTF("../Content/NotoSansCJK-Regular.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
}

