#include "Engine.hpp"
#include "Framework.hpp"
#include "Instance.hpp"

#ifndef UIBUS_HANDWRITE_DATA_DIR
    #define UIBUS_HANDWRITE_DATA_DIR "/usr/share/"
#endif

#define IBUS_TYPE_CUSTOM_ENGINE	\
	(engine_get_type())

GType engine_get_type();

#define IBUS_CUSTOM_ENGINE(obj) G_TYPE_CHECK_INSTANCE_CAST(obj,IBUS_TYPE_CUSTOM_ENGINE,IBusCustomEngine)

namespace UntitledIBusHandwriting
{
    typedef void (*ibus_engine_callback)(IBusEngine* engine);

    [[maybe_unused]] static void engine_class_init(IBusCustomEngineClass* klass);
    [[maybe_unused]] static void engine_init(IBusCustomEngine* self);
    static void engine_destroy(IBusCustomEngine* engine);
    static gboolean engine_process_key_event(IBusEngine* engine, guint keyval, guint keycode, guint modifiers);
    static void engine_focus_in(IBusCustomEngine* engine);
    static void engine_focus_out(IBusCustomEngine* engine);
    static void engine_reset(IBusCustomEngine* engine);
    static void engine_enable(IBusCustomEngine* engine);
    static void engine_disable(IBusCustomEngine* engine);

    static void property_activate(IBusEngine* engine, const gchar* prop_name, guint prop_state);

    G_DEFINE_TYPE(IBusCustomEngine,engine,IBUS_TYPE_ENGINE)

    [[maybe_unused]] static void engine_class_init(IBusCustomEngineClass* klass)
    {
        const auto ibus_object_class = IBUS_OBJECT_CLASS (klass);
        const auto engine_class = IBUS_ENGINE_CLASS (klass);

        ibus_object_class->destroy = reinterpret_cast<IBusObjectDestroyFunc>(engine_destroy);

        engine_class->process_key_event = engine_process_key_event;
        engine_class->disable = reinterpret_cast<ibus_engine_callback>(engine_disable);
        engine_class->enable = reinterpret_cast<ibus_engine_callback>(engine_enable);
        engine_class->focus_in = reinterpret_cast<ibus_engine_callback>(engine_focus_in);
        engine_class->focus_out = reinterpret_cast<ibus_engine_callback>(engine_focus_out);
        engine_class->reset = reinterpret_cast<ibus_engine_callback>(engine_reset);
        engine_class->property_activate = property_activate;

        klass->commit_text = engine_commit_text;
    }

    [[maybe_unused]] static void engine_init(IBusCustomEngine* self)
    {
        const auto instance = static_cast<UntitledIBusHandwriting::Instance*>(UImGui::Instance::getGlobal());
        instance->initInfo.globalData = self;
    }

    static void engine_destroy(IBusCustomEngine* engine)
    {
        engine_disable(engine);
        IBUS_OBJECT_CLASS(engine_parent_class)->destroy(reinterpret_cast<IBusObject*>(engine));
        UImGui::Instance::shutdown();
    }

    static void engine_enable(IBusCustomEngine* engine)
    {
        engine_focus_in(engine);
    }

    static void engine_disable(IBusCustomEngine*)
    {

    }

    static void engine_focus_in(IBusCustomEngine* engine)
    {
        const auto* en = IBUS_ENGINE(engine);
        if (en->cursor_area.x == 0 && en->cursor_area.y == 0)
            return;

        const auto y = static_cast<float>(en->cursor_area.y + en->cursor_area.height);
        UImGui::Window::setCurrentWindowPosition({ static_cast<float>(en->cursor_area.x), y });
    }

    static void engine_focus_out(IBusCustomEngine*)
    {
    }

    static void engine_reset(IBusCustomEngine*)
    {
    }

    void property_activate(IBusEngine*, const gchar*, guint)
    {
    }

    gboolean engine_commit_text(IBusCustomEngine* engine, const int index)
    {
        ibus_engine_commit_text(IBUS_ENGINE(engine), ibus_text_new_from_string(stagingBuffer[index].c_str()));
        return true;
    }

    static gboolean engine_process_key_event(IBusEngine* engine, const guint keyval, guint, const guint modifiers)
    {
        auto* handwrite = reinterpret_cast<IBusCustomEngine*>(engine);

        if (modifiers & IBUS_RELEASE_MASK)
            return true;

        if (keyval >= IBUS_KEY_0 && keyval <= IBUS_KEY_9)
            return engine_commit_text(handwrite, static_cast<int>(keyval - IBUS_KEY_0));
        if (keyval >= IBUS_KEY_a && keyval <= IBUS_KEY_f)
            return engine_commit_text(handwrite, static_cast<int>((keyval - IBUS_KEY_a) + (IBUS_KEY_9 - IBUS_KEY_0) + 1));
        return false;
    }
}

void UntitledIBusHandwriting::Engine::initIBus(const char* executable, const bool bIbusAvailable) noexcept
{
   IBusComponent* component;
   static IBusFactory* factory = nullptr;
   static constexpr char icondir[4096]= UIBUS_HANDWRITE_DATA_DIR"icons/untitled-ibus-handwriting.png";

   ibus_init();
   bus = ibus_bus_new();

   g_signal_connect(bus, "disconnected", G_CALLBACK(shutdown), nullptr);

   factory = ibus_factory_new(ibus_bus_get_connection(bus));
   ibus_bus_request_name(bus, "org.freedesktop.IBus.UntitledIBusHandwriting", 0);

   if (!bIbusAvailable)
   {
       char* binaryPath = realpath(executable, nullptr);

       component = ibus_component_new("org.freedesktop.IBus.UntitledIBusHandwriting",
                                      "The UntitledIBusHandwriting engine", "1.0.0.0", "MIT", "contact@madladsquad.com", "https://github.com/MadLadSquad/UntitledIBusHandwriting/",
                                      binaryPath, "UntitledIBusHandwriting");

       IBusEngineDesc* desc = ibus_engine_desc_new("UntitledIBusHandwriting", "handwrite",
                                   "The UntitledIBusHandwriting engine", "zh_CN", "MIT",
                                   "contact@madladsquad.com", icondir, "default");

       ibus_component_add_engine(component, desc);
       free(binaryPath);
   }
   else
   {
       component = ibus_component_new("org.freedesktop.IBus.UntitledIBusHandwriting",
                                      "The UntitledIBusHandwriting engine", "1.0.0.0", "MIT", "contact@madladsquad.com", "https://github.com/MadLadSquad/UntitledIBusHandwriting/",
                                      UIBUS_HANDWRITE_DATA_DIR, "UntitledIBusHandwriting");
   }
   ibus_bus_register_component(bus, component);
   ibus_factory_add_engine(factory, "UntitledIBusHandwriting", IBUS_TYPE_CUSTOM_ENGINE);
   g_object_unref(component);
   ibus_main();
}

void* UntitledIBusHandwriting::Engine::shutdown() noexcept
{
    UImGui::Instance::shutdown();
    return nullptr;
}
