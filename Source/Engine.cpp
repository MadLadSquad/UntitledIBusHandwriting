#include "Engine.hpp"
#include "Framework.hpp"
#include "Instance.hpp"

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
        IBusObjectClass* ibus_object_class = IBUS_OBJECT_CLASS (klass);
        IBusEngineClass* engine_class = IBUS_ENGINE_CLASS (klass);

        ibus_object_class->destroy = (IBusObjectDestroyFunc)engine_destroy;

        engine_class->process_key_event = engine_process_key_event;
        engine_class->disable = (ibus_engine_callback)engine_disable;
        engine_class->enable = (ibus_engine_callback)engine_enable;
        engine_class->focus_in = (ibus_engine_callback)engine_focus_in;
        engine_class->focus_out = (ibus_engine_callback)engine_focus_out;
        engine_class->reset = (ibus_engine_callback)engine_reset;
        engine_class->property_activate = property_activate;

        klass->commit_text = engine_commit_text;
    }

    [[maybe_unused]] static void engine_init(IBusCustomEngine* self)
    {
        auto instance = (UntitledIBusHandwriting::Instance*)UImGui::Instance::getGlobal();
        instance->initInfo.globalData = self;
    }

    static void engine_destroy(IBusCustomEngine* engine)
    {
        engine_disable(engine);
        IBUS_OBJECT_CLASS(engine_parent_class)->destroy((IBusObject*)engine);
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
        auto* en = IBUS_ENGINE(engine);
        if (en->cursor_area.x == 0 && en->cursor_area.y == 0)
            return;

        auto y = static_cast<float>(en->cursor_area.y + en->cursor_area.height);

        // This is to ensure that on input bars at the bottom of the screen, we don't
        if ((y + UImGui::Window::windowSize().y) > 0)
        {

        }
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

    gboolean engine_commit_text(IBusCustomEngine* engine, int index)
    {
        ibus_engine_commit_text(IBUS_ENGINE(engine), ibus_text_new_from_string(stagingBuffer[index].c_str()));
        return true;
    }

    static gboolean engine_process_key_event(IBusEngine* engine, guint keyval, guint, guint modifiers)
    {
        auto* handwrite = (IBusCustomEngine*)engine;

        if (modifiers & IBUS_RELEASE_MASK)
            return true;

        if (keyval >= IBUS_KEY_0 && keyval <= IBUS_KEY_9)
            return engine_commit_text(handwrite, static_cast<int>(keyval - IBUS_KEY_0));
        else if (keyval >= IBUS_KEY_a && keyval <= IBUS_KEY_f)
            return engine_commit_text(handwrite, static_cast<int>((keyval - IBUS_KEY_a) + (IBUS_KEY_9 - IBUS_KEY_0) + 1));
        return false;
    }
}

void UntitledIBusHandwriting::Engine::initIBus(char* executable, bool bIbusAvailable) noexcept
{
   IBusComponent* component;
   IBusEngineDesc* desc;
   static IBusFactory* factory = nullptr;

   // TODO: Make this non-hardcoded and use the relevant variables
   static constexpr char icondir[4096]= "/usr/share/icons/untitled-ibus-handwriting.png";

   ibus_init();
   bus = ibus_bus_new();

   g_signal_connect(bus, "disconnected", G_CALLBACK(shutdown), nullptr);

   factory = ibus_factory_new(ibus_bus_get_connection(bus));
   ibus_bus_request_name(bus, "org.freedesktop.IBus.UntitledIBusHandwriting", 0);

   if (!bIbusAvailable)
   {
       char* binaryPath;
       binaryPath = realpath(executable, nullptr);

       component = ibus_component_new("org.freedesktop.IBus.UntitledIBusHandwriting",
                                      "The UntitledIBusHandwriting engine", "1.0.0.0", "MIT", "contact@madladsquad.com", "https://github.com/MadLadSquad/UntitledIBusHandwriting/",
                                      binaryPath, "UntitledIBusHandwriting");

       desc = ibus_engine_desc_new("UntitledIBusHandwriting", "handwrite",
                                   "The UntitledIBusHandwriting engine", "bg", "MIT",
                                   "contact@madladsquad.com", icondir, "default");

       ibus_component_add_engine(component, desc);
       free(binaryPath);
   }
   else
   {
       component = ibus_component_new("org.freedesktop.IBus.UntitledIBusHandwriting",
                                      "The UntitledIBusHandwriting engine", "1.0.0.0", "MIT", "contact@madladsquad.com", "https://github.com/MadLadSquad/UntitledIBusHandwriting/",
                                      "/usr/share/", "UntitledIBusHandwriting");
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
