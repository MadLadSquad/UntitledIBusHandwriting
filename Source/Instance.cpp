#include "Instance.hpp"

UntitledIBusHandwriting::Instance::Instance()
{
    initInfo.inlineComponents = { &mainView };
}

void UntitledIBusHandwriting::Instance::begin()
{
    beginAutohandle();

}

void UntitledIBusHandwriting::Instance::tick(float deltaTime)
{
    tickAutohandle(deltaTime);

}

void UntitledIBusHandwriting::Instance::end()
{
    endAutohandle();

}

UntitledIBusHandwriting::Instance::~Instance()
{

}

void UntitledIBusHandwriting::Instance::onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io)
{

}

