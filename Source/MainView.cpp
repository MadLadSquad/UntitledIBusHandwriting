#include "MainView.hpp"
#include <utfcpp/source/utf8.h>
#include <Engine.hpp>
#include <Instance.hpp>

#ifndef UIBUS_HANDWRITE_HANZI_LOOKUP_DIR
    #define UIBUS_HANDWRITE_HANZI_LOOKUP_DIR "../hanzi_lookup/.target/release/libhanzi_lookup.so"
#endif

UntitledIBusHandwriting::MainView::MainView()
{

}

void UntitledIBusHandwriting::MainView::begin()
{
    beginAutohandle();
    lib = URLL::dlopen(UIBUS_HANDWRITE_HANZI_LOOKUP_DIR"libhanzi_lookup.so");
    if (lib == nullptr)
    {
        Logger::log("Died at loading the library from Rust. Error: ", UVKLog::UVK_LOG_TYPE_ERROR, URLL::dlerror());
        UImGui::Instance::shutdown();
    }

    if (URLL::dlsym(lib, "c_lib_main", loadData) != lib)
    {
        Logger::log("Died at loading c_lib_main from Rust. Error: ", UVKLog::UVK_LOG_TYPE_ERROR, URLL::dlerror());
        UImGui::Instance::shutdown();
        return;
    }
    if (URLL::dlsym(lib, "c_lib_main_cleanup", deallocateLoadedData) != lib)
    {
        Logger::log("Died at loading c_lib_main_cleanup from Rust. Error: ", UVKLog::UVK_LOG_TYPE_ERROR, URLL::dlerror());
        UImGui::Instance::shutdown();
        return;
    }
    data = (Instance*)UImGui::Instance::getGlobal();
}

void UntitledIBusHandwriting::MainView::tick(float deltaTime)
{
    tickAutohandle(deltaTime);

    static std::vector<std::vector<UImGui::FVector2>> pp;
    static std::string saveString;
    static std::string resultString;

    static std::vector<UImGui::FVector2> undoTmpStroke;

    static ImVec2 scrolling(0.0f, 0.0f);
    static bool adding_line = false;

    static ImVec4 colours = { 1.0f, 1.0f, 0.0f, 1.0f };

    ImGui::Text("Colour:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##Color", (float*)&colours, ImGuiColorEditFlags_NoInputs);

    ImGui::SameLine();

    if (ImGui::Button("Undo"))
    {
        undoTmpStroke = pp.back();
        pp.pop_back();
    }
    ImGui::SameLine();
    if (ImGui::Button("Redo"))
    {
        pp.push_back(undoTmpStroke);
        undoTmpStroke.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear") || UImGui::Input::getKey(Keys_Escape) == Keys_KeyPressed)
    {
        stagingBuffer.clear();
        data->characters.clear();
        pp.clear();
    }

    if (!data->characters.empty() && ImGui::BeginTable("Results table", 8))
    {
        ImGui::TableNextColumn();
        for (uint8_t i = 0; static_cast<size_t>(i) < data->characters.size(); i++)
        {
            // Add '0' so that we can convert our normal number to ASCII number character
            uint8_t tmp = i + '0';

            // If the number is larger than ASCII '9', then switch to Latin letters.
            // 'A' - '9' is the difference between the 'A' and '9' letters. We remove 1 because we're above 9, we could
            // use '-' but that will be ambiguous and wouldn't make sense
            if (tmp > '9')
                tmp += ('A' - '9' - 1);

            ImGui::Text("%c.%s", tmp, data->characters[i].c_str());
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    else
    {
        ImGui::NewLine();
        ImGui::NewLine();
    }

    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    // Create an invisible button to serve as our canvas click field
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
    const UImGui::FVector2 mouse_pos_in_canvas = { io.MousePos.x - origin.x, io.MousePos.y - origin.y };

    // Add first and second point
    if (ImGui::IsItemHovered() && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        pp.emplace_back();
        pp.back().push_back(mouse_pos_in_canvas);
        //pp.back().push_back(mouse_pos_in_canvas);
        adding_line = true;
    }

    if (adding_line)
    {
        pp.back().push_back(mouse_pos_in_canvas);
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            // Reset all variables to their original state
            adding_line = false;
            saveString.clear();

            // Start L1 of the SVG path
            saveString = "[";
            for (auto& a : pp)
            {
                // Sometimes we get this weird behaviour, where a user accidentally clicks and this creates a stroke
                // only with beginning coordinates. This check solves this partially
                if (a.size() < 2)
                    continue;

                // Start inner array
                saveString += "[";
                UImGui::FVector2 old = { 0, 0 };

                // These 2 variables are here because while the last check removed the occurrence of [[[x,y]]], because
                // we sanitize strings by reducing duplicate coordinates in neighboring positions, [[[x,y],[x,y]]] is
                // still a type of behaviour we might observe. Because of this, we create this temporary FVector2
                // string that we can use to check if we only have 1 point
                std::string tmpArrayAccum;
                size_t insertionCount = 0;
                for (auto& f : a)
                {
                    // Check if we need to sanitize duplicate neighboring points
                    if (old.x != f.x && old.y != f.y)
                    {
                        // Unique point, increase insertion count
                        insertionCount++;
                        // Limit points to 254 since the undelying implementation uses Rust's u8 type = uint8_t
                        if (abs(f.x) >= 254.0f)
                            f.x = 254.0f;
                        if (abs(f.y) >= 254.0f)
                            f.y = 254.0f;

                        // Append the temporary string
                        tmpArrayAccum += "[" + std::to_string(static_cast<int64_t>(abs(f.x))) + "," + std::to_string(static_cast<int64_t>(abs(f.y))) + "],";
                    }

                    // Update old point
                    old = f;
                }

                // If we have more than 2 points, then we push
                if (insertionCount >= 2)
                {
                    saveString += tmpArrayAccum;
                    saveString.pop_back();
                    saveString += "],";
                }
                else // Otherwise we remove the [ character inserted at the beginning of the iteration
                    saveString.pop_back();
            }
            // Remove last character since the loop will always insert a trailing ',' character
            saveString.pop_back();
            // If the string is empty it means that we had a single point line, so we return out
            if (saveString.empty())
                return;
            saveString += "]";

            // Clear data for the characters
            data->characters.clear();

            // Send the string to Rust and request 16 characters as a result
            auto str = loadData(16, saveString.c_str());
            resultString = std::string(str);

            // Create a temporary to store each unicode character
            std::u32string tmp;
            tmp.resize(1);

            // Iterators for iterating the unicode string returned by Rust
            utf8::iterator begin(resultString.begin(), resultString.begin(), resultString.end());
            utf8::iterator end(resultString.end(), resultString.begin(), resultString.end());
            for (auto i = begin; i != end; ++i)
            {
                // Create characters array
                tmp[0] = *i;
                data->characters.push_back(utf8::utf32to8(tmp));
            }

            // Copy the staging buffer so that the ibus thread can take advantage of the newly found data
            stagingBuffer = data->characters;
            // Send string back to Rust for the memory to be freed
            deallocateLoadedData(str);
        }
    }
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    for (auto& a : pp)
    {
        for (int n = 1; n < a.size(); ++n)
            draw_list->AddLine(ImVec2(origin.x + a[n].x, origin.y + a[n].y), ImVec2(origin.x + a[n - 1].x, origin.y + a[n - 1].y), ImGui::ColorConvertFloat4ToU32(colours), 5.0f);
    }

    draw_list->PopClipRect();
}

void UntitledIBusHandwriting::MainView::end()
{
    endAutohandle();
    URLL::dlclose(lib);
}

UntitledIBusHandwriting::MainView::~MainView()
{

}

