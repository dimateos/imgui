// Sandbox tests
#include "imgui.h"

// cpp properties include paths
//"${workspaceFolder}/Open-imgui/",
//"${workspaceFolder}/Open-imgui/backends/",
//"${workspaceFolder}/Open-imgui/examples/libs/glfw/include",


// https://github.com/sethk/imgui/tree/raii/misc/cpp + modified for tests
#include "imgui_scoped.h"

void test_scoped()
{
    static bool test_scoped_window = true;
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_1)) test_scoped_window ^= 1;
    if (!test_scoped_window) return;

    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    if (auto win = ImScoped::Window("Test Window", &test_scoped_window)) {
        ImGui::Text("Hello");
        ImGui::Text("Bye...");
    }
}
