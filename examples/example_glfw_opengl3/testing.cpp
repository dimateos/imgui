// Sandbox tests
#include "imgui.h"

// cpp properties include paths
//"${workspaceFolder}/Open-imgui/",
//"${workspaceFolder}/Open-imgui/backends/",
//"${workspaceFolder}/Open-imgui/examples/libs/glfw/include",

// clear id is internal, also breakpoint
#include "imgui_internal.h"


// TMP:: anything
#include <iostream>
#include <string>
void test() {
    //std::cout << "> std out" << std::endl;

    if (ImGui::IsKeyChordPressed(ImGuiKey_B)) {
        std::cout << "breakpoint (crashes with when debugger)" << std::endl;
        IM_DEBUG_BREAK();
    }
}


// https://github.com/sethk/imgui/tree/raii/misc/cpp + modified for tests
#include "imgui_scoped.h"

// does the compiler properly use the move constructor instead of deleted noncopyable?
void test_scoped(int k = 0, bool start_open = true)
{
    static bool test_scoped_window = true;
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | k)) test_scoped_window ^= 1;
    if (!test_scoped_window) return;

    ImGui::SetNextWindowCollapsed(!start_open, ImGuiCond_Once);
    std::string name = std::to_string(k-ImGuiKey_0) + ". test_scoped_window";
    ImGui::SetNextWindowCollapsed(!start_open, ImGuiCond_Once);
    if (auto win = ImScoped::Window(name.c_str(), &test_scoped_window)) {
        ImGui::Text("Hello");
        ImGui::Text("Bye...");
    }
}

// Scrolling on top a combo box like QT
void test_combo_scroll(int k = 0, bool start_open = true)
{
    static bool test_combo_scroll_window = true;
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | k)) test_combo_scroll_window ^= 1;
    if (!test_combo_scroll_window) return;

    ImGui::SetNextWindowCollapsed(!start_open, ImGuiCond_Once);
    std::string name = std::to_string(k-ImGuiKey_0) + ". test_combo_scroll_window";
    ImGui::Begin(name.c_str(), &test_combo_scroll_window);

    static int selected_item = 0;
    const char* items[] = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
    int items_size = IM_ARRAYSIZE(items);

    // callback when changed should be called when scroll changes it too
    if (ImGui::Combo("Combo", &selected_item, items, items_size)) {
    }

    // own the scroll over the window
    ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
    ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

    // show hover
    auto combo_hover = ImGui::IsItemHovered();
    if (combo_hover) {
        ImGui::BulletText("Combo is hovered now...");
    }

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

    // simlate scrollable
    for (size_t i = 0; i < 10; i++) ImGui::TextDisabled("...");
    ImGui::End();
}

// there could be one for floating, int, bool?
bool ModifyItemValueByScroll(float *v, float scale = 1.0f, ImGuiHoveredFlags hoverFlags = 0) {
    // issues:
    // - ignores limits: only adjusted when the behavior is clicked not on hover
    // - eats the is item hovered query? nop issue arises when a new item is added before the next check

    // own the scroll over the window -> only for items, not texts!
    //ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
    //ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

    bool hovered = ImGui::IsItemHovered(hoverFlags);
    if (!hovered) return false;
    //ImGui::BulletText("hovered... in");

    // own the scroll even for texts
    //ImGuiContext& g = *GImGui;
    //ImGuiID id = g.LastItemData.ID;
    //ImGui::SetHoveredID(id);
    //ImGui::SetKeyOwner(ImGuiKey_MouseWheelX, id);
    //ImGui::SetKeyOwner(ImGuiKey_MouseWheelY, id);
    // ID are ONLY commited by actual items, texts appear as g.LastItemData but with no ID -> IsItemHovered checks if its rect was touched
    ImGui::SetKeyOwner(ImGuiKey_MouseWheelX, ImGuiKeyOwner_Any, ImGuiInputFlags_LockThisFrame);
    ImGui::SetKeyOwner(ImGuiKey_MouseWheelY, ImGuiKeyOwner_Any, ImGuiInputFlags_LockThisFrame);

    float scroll_delta = ImGui::GetIO().MouseWheel; // both axis
    if (scroll_delta == 0) return false;

    // stop interaction
    if (ImGui::IsItemActive()) ImGui::ClearActiveID();

    // edit the value
    *v -= scroll_delta * scale;
    return true;
}

void test_value_scroll(int k = 0, bool start_open = true)
{
    static bool test_value_scroll_window = true;
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | k)) test_value_scroll_window ^= 1;
    if (!test_value_scroll_window) return;

    ImGui::SetNextWindowCollapsed(!start_open, ImGuiCond_Once);
    std::string name = std::to_string(k-ImGuiKey_0) + ". test_value_scroll_window";
    ImGui::Begin(name.c_str(), &test_value_scroll_window);

    static float f = 0.0f;
    static bool disabled = false, query2 = false;

    ImGui::Checkbox("disabled", &disabled);
    ImGui::Checkbox("query2", &query2);

    // widget
    if (disabled) ImGui::BeginDisabled();
    ImGui::SliderFloat("slider", &f, -100, 100);
    //ImGui::SliderFloat("slider2", &f, -100, 100);
    //ImGui::DragFloat("drag", &f, 1.0f, -100, 100);
    //ImGui::InputFloat("input", &f, 1.0f);
    ImGui::Text("f: %f", f);
    if (disabled) ImGui::EndDisabled();

    ImGuiContext& g = *GImGui;
    int last = g.LastItemData.ID, hover = g.HoveredId, active = g.ActiveId;

    ModifyItemValueByScroll(&f);

    // the spawned text affects it?
    if (query2 && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::BulletText("hovered... 2");
    }
    //ImGui::SliderFloat("slider3", &f, -100, 100);

    // draw ids
    ImGui::SeparatorText("Ids");
    ImGui::BulletText("val Id %d", last);
    ImGui::BulletText("val HOVER Id %d", hover);
    ImGui::BulletText("val ACTIV Id %d", active);

    // ALL
    ImGui::SeparatorText("ALL");
    ImGui::SliderFloat("slider", &f, -100, 100);
    ModifyItemValueByScroll(&f);
    ImGui::SliderFloat("slider2", &f, -100, 100);
    ModifyItemValueByScroll(&f);
    ImGui::DragFloat("drag", &f, 1.0f, -100, 100);
    ModifyItemValueByScroll(&f);
    ImGui::InputFloat("input", &f, 1.0f);
    ModifyItemValueByScroll(&f);
    ImGui::Text("f: %f", f);
    ModifyItemValueByScroll(&f);
    ModifyItemValueByScroll(&f);

    // simlate scrollable
    ImGui::SeparatorText("fill");
    for (size_t i = 0; i < 10; i++) ImGui::TextDisabled("...");
    ImGui::End();
}

void test_disabled_hover(int k = 0, bool start_open = true)
{
    static bool test_disabled_hover_window = true;
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | k)) test_disabled_hover_window ^= 1;
    if (!test_disabled_hover_window) return;

    ImGui::SetNextWindowCollapsed(!start_open, ImGuiCond_Once);
    std::string name = std::to_string(k-ImGuiKey_0) + ". test_disabled_hover_window";
    ImGui::Begin(name.c_str(), &test_disabled_hover_window);

    static float f = 0.0f, fd = 0.0f;

    // disabled?
    ImGui::BeginDisabled();
    ImGui::SliderFloat("disabled", &fd, -1, 1);
    //if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::BulletText("hovered... 1");
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::BulletText("hovered... 2"); // both ok but require the flag

    // regular?
    ImGui::SliderFloat("regular", &f, -1, 1);
    if (ImGui::IsItemHovered()) ImGui::BulletText("hovered... 3");

    ImGui::End();
}