#pragma once
#include <ibus.h>

#define IBUS_TYPE_CUSTOM_ENGINE	\
	(engine_get_type())

namespace UntitledIBusHandwriting
{
    struct IBusCustomEngine
    {
        IBusEngine parent;
    };

    struct IBusCustomEngineClass
    {
        IBusEngineClass parent;
        gboolean (*commit_text)(IBusCustomEngine* engine, int index);
    };

    gboolean engine_commit_text(IBusCustomEngine* engine, int index);
    GType engine_get_type();

    class Engine
    {
    public:
        Engine() = default;

        void initIBus(char* executable, bool bIbusAvailable) noexcept;
        static void* shutdown() noexcept;
    private:
        IBusBus* bus = nullptr;
    };


}