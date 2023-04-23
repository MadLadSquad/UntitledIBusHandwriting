#include "MainView.hpp"

UntitledIBusHandwriting::MainView::MainView()
{

}

void UntitledIBusHandwriting::MainView::begin()
{
    beginAutohandle();

}

void UntitledIBusHandwriting::MainView::tick(float deltaTime)
{
    tickAutohandle(deltaTime);

    static std::vector<std::vector<UImGui::FVector2>> pp;
    static std::string saveString;

    static ImVec2 scrolling(0.0f, 0.0f);
    static bool adding_line = false;

    static ImVec4 colours = { 1.0f, 1.0f, 0.0f, 1.0f };

    ImGui::Text("Colour:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##Color", (float*)&colours, ImGuiColorEditFlags_NoInputs);

    ImGui::SameLine();

    if (ImGui::Button("Save"))
    {
        saveString.clear();
        saveString = "[";
        for (auto& a : pp)
        {
            saveString += "[";
            UImGui::FVector2 old = { 0, 0 };
            for (auto& f : a)
            {
                if (old.x != f.x && old.y != f.y)
                    saveString += "[" + std::to_string(static_cast<int64_t>(f.x)) + "," + std::to_string(static_cast<int64_t>(f.y)) + "],";
                old = f;
            }
            saveString.pop_back();
            saveString += "],";
        }
        saveString.pop_back();
        saveString += "]";

        std::cout << saveString << std::endl;
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
    {
        pp.clear();
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

    // This will catch our interactions
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
    const UImGui::FVector2 mouse_pos_in_canvas = { io.MousePos.x - origin.x, io.MousePos.y - origin.y };

    // Add first and second point
    if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        pp.emplace_back();
        pp.back().push_back(mouse_pos_in_canvas);
        pp.back().push_back(mouse_pos_in_canvas);
        adding_line = true;
    }
    if (adding_line)
    {
        pp.back().push_back(mouse_pos_in_canvas);
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            adding_line = false;
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

}

UntitledIBusHandwriting::MainView::~MainView()
{

}

