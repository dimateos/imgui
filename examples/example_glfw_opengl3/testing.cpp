// Sandbox tests
#include "imgui.h"

// cpp properties include paths
//"${workspaceFolder}/Open-imgui/",
//"${workspaceFolder}/Open-imgui/backends/",
//"${workspaceFolder}/Open-imgui/examples/libs/glfw/include",


// TMP:: anything
#include <iostream>
void test() {
    std::cout << "> cmd out hooked?" << std::endl;
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

// WIP:: scrolling on top a combo box like QT
void test_combo_scroll()
{
    static bool test_combo_scroll_window = true;
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_2)) test_combo_scroll_window ^= 1;
    if (!test_combo_scroll_window) return;

    ImGui::Begin("test_combo_scroll", &test_combo_scroll_window);

    static int selected_item = 0;
    const char* items[] = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };

    if (ImGui::BeginCombo("Combo", items[selected_item])) {
        ImGuiIO& io = ImGui::GetIO();

        for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
            bool is_selected = (selected_item == i);
            if (ImGui::Selectable(items[i], is_selected))
                selected_item = i;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        // Scroll combo box when hovered and the mouse wheel is moved
        if (ImGui::IsItemHovered()) {
            float scroll_amount = io.MouseWheel;  // Get the mouse wheel delta
            ImGui::SetScrollY(ImGui::GetScrollY() - scroll_amount * 10.0f);  // Adjust the scroll speed as needed
        }

        ImGui::EndCombo();
    }

    ImGui::End();
}