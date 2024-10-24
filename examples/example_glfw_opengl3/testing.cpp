// Sandbox tests
#include "imgui.h"

// cpp properties include paths
//"${workspaceFolder}/Open-imgui/",
//"${workspaceFolder}/Open-imgui/backends/",
//"${workspaceFolder}/Open-imgui/examples/libs/glfw/include",


// TMP:: anything
#include <iostream>
void test() {
    //std::cout << "> cmd out hooked?" << std::endl;
}


// https://github.com/sethk/imgui/tree/raii/misc/cpp + modified for tests
#include "imgui_scoped.h"

// does the compiler properly use the move constructor instead of deleted noncopyable?
void test_scoped()
{
    static bool test_scoped_window = true;
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_1)) test_scoped_window ^= 1;
    if (!test_scoped_window) return;

    if (auto win = ImScoped::Window("test_scoped", &test_scoped_window)) {
        ImGui::Text("Hello");
        ImGui::Text("Bye...");
    }
}

// Scrolling on top a combo box like QT -> could be also done for sliders, makes sense for fixed windows with no scroll
void test_combo_scroll()
{
    static bool test_combo_scroll_window = true;
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_2)) test_combo_scroll_window ^= 1;
    if (!test_combo_scroll_window) return;

    ImGui::Begin("test_combo_scroll", &test_combo_scroll_window);

    static int selected_item = 0;
    const char* items[] = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
    int items_size = IM_ARRAYSIZE(items);

    if (ImGui::Combo("Combo", &selected_item, items, items_size)) {
        // callback when changed should be called when scroll changes it too
    }

    // show hover
    auto combo_hover = ImGui::IsItemHovered();
    if (combo_hover) ImGui::BulletText("Combo is hovered now...");

    // show scroll
    float scroll_delta = ImGui::GetIO().MouseWheel;
    ImGui::BeginDisabled();
    ImGui::SliderFloat("wheel", &scroll_delta, 0, 1);
    ImGui::EndDisabled();

    // show min max
    static float min_scroll_delta = FLT_MAX;
    static float max_scroll_delta = -FLT_MAX;
    static float last_non_zero_scroll_delta = 0.0f;
    if (scroll_delta < min_scroll_delta) min_scroll_delta = scroll_delta;
    if (scroll_delta > max_scroll_delta) max_scroll_delta = scroll_delta;
    if (scroll_delta != 0.0f) last_non_zero_scroll_delta = scroll_delta;
    ImGui::BulletText("Min scroll delta: %.3f", min_scroll_delta);
    ImGui::BulletText("Max scroll delta: %.3f", max_scroll_delta);
    ImGui::BulletText("Last non-zero scroll delta: %.3f", last_non_zero_scroll_delta);

    // scroll on hover (flip index)
    if (combo_hover && scroll_delta != 0.0f) {
        int dir = scroll_delta > 0.0f ? 1 : -1;
        selected_item -= dir;
        if (selected_item < 0) selected_item = 0;
        else if (selected_item > items_size-1) selected_item = items_size-1;
    }

    ImGui::End();
}