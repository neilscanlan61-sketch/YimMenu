#pragma once
#include <imgui.h>
#include <cstring>

// Visual-only helpers. No game, networking, or hook dependencies.
namespace big::compact_ui
{
    inline void apply_style()
    {
        ImGui::StyleColorsDark();
        auto& s = ImGui::GetStyle();
        s.WindowPadding = ImVec2(10, 10);
        s.WindowRounding = 8;
        s.WindowBorderSize = 1;
        s.ChildRounding = 6;
        s.ChildBorderSize = 1;
        s.FramePadding = ImVec2(6, 3);
        s.FrameRounding = 4;
        s.FrameBorderSize = 0;
        s.ItemSpacing = ImVec2(8, 5);
        s.ItemInnerSpacing = ImVec2(5, 4);
        s.IndentSpacing = 12;
        s.ScrollbarSize = 8;
        s.ScrollbarRounding = 4;
        s.GrabMinSize = 8;
        s.GrabRounding = 4;
        s.PopupRounding = 6;
        s.TabRounding = 4;
        auto* c = s.Colors;
        c[ImGuiCol_Text] = ImVec4(.94f, .92f, 1.f, 1.f);
        c[ImGuiCol_TextDisabled] = ImVec4(.65f, .61f, .74f, 1.f);
        c[ImGuiCol_WindowBg] = ImVec4(.045f, .025f, .075f, .97f);
        c[ImGuiCol_ChildBg] = ImVec4(.055f, .03f, .085f, .96f);
        c[ImGuiCol_PopupBg] = ImVec4(.065f, .035f, .10f, .99f);
        c[ImGuiCol_Border] = ImVec4(.42f, .20f, .65f, .85f);
        c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_FrameBg] = ImVec4(.14f, .065f, .23f, 1);
        c[ImGuiCol_FrameBgHovered] = ImVec4(.25f, .12f, .40f, 1);
        c[ImGuiCol_FrameBgActive] = ImVec4(.36f, .18f, .56f, 1);
        c[ImGuiCol_TitleBg] = c[ImGuiCol_WindowBg];
        c[ImGuiCol_TitleBgActive] = ImVec4(.13f, .055f, .22f, 1);
        c[ImGuiCol_TitleBgCollapsed] = c[ImGuiCol_WindowBg];
        c[ImGuiCol_MenuBarBg] = c[ImGuiCol_WindowBg];
        c[ImGuiCol_Button] = ImVec4(.19f, .075f, .32f, 1);
        c[ImGuiCol_ButtonHovered] = ImVec4(.32f, .14f, .51f, 1);
        c[ImGuiCol_ButtonActive] = ImVec4(.43f, .20f, .65f, 1);
        c[ImGuiCol_Header] = ImVec4(.24f, .10f, .39f, 1);
        c[ImGuiCol_HeaderHovered] = ImVec4(.34f, .15f, .53f, 1);
        c[ImGuiCol_HeaderActive] = ImVec4(.44f, .20f, .66f, 1);
        c[ImGuiCol_CheckMark] = ImVec4(.78f, .50f, 1, 1);
        c[ImGuiCol_SliderGrab] = ImVec4(.57f, .29f, .86f, 1);
        c[ImGuiCol_SliderGrabActive] = ImVec4(.79f, .54f, 1, 1);
        c[ImGuiCol_ScrollbarBg] = ImVec4(.04f, .025f, .06f, .5f);
        c[ImGuiCol_ScrollbarGrab] = ImVec4(.31f, .16f, .47f, 1);
        c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(.46f, .23f, .69f, 1);
        c[ImGuiCol_ScrollbarGrabActive] = ImVec4(.61f, .33f, .88f, 1);
        c[ImGuiCol_Separator] = c[ImGuiCol_Border];
        c[ImGuiCol_SeparatorHovered] = c[ImGuiCol_SliderGrab];
        c[ImGuiCol_SeparatorActive] = c[ImGuiCol_SliderGrabActive];
        c[ImGuiCol_Tab] = c[ImGuiCol_Button];
        c[ImGuiCol_TabHovered] = c[ImGuiCol_ButtonHovered];
        c[ImGuiCol_TabActive] = c[ImGuiCol_ButtonActive];
        c[ImGuiCol_TextSelectedBg] = ImVec4(.46f, .21f, .72f, .5f);
        c[ImGuiCol_NavHighlight] = c[ImGuiCol_CheckMark];
        c[ImGuiCol_ResizeGrip] = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_ResizeGripHovered] = c[ImGuiCol_SliderGrab];
        c[ImGuiCol_ResizeGripActive] = c[ImGuiCol_SliderGrabActive];
    }

    // Draw inside the window's clip rectangle; no foreground overlap with controls.
    inline void decorate_window()
    {
        const auto p = ImGui::GetWindowPos();
        const auto size = ImGui::GetWindowSize();
        auto* d = ImGui::GetWindowDrawList();
        const float y = p.y + 2;
        for (int i = 5; i >= 1; --i)
            d->AddLine(ImVec2(p.x + 8, y), ImVec2(p.x + size.x - 8, y),
                IM_COL32(164, 77, 255, 14), static_cast<float>(i * 2));
        d->AddLine(ImVec2(p.x + 8, y), ImVec2(p.x + size.x - 8, y),
            IM_COL32(209, 154, 255, 235), 1);
    }

    inline bool toggle(const char* label, bool* value)
    {
        const float h = ImGui::GetFrameHeight();
        const float w = h * 1.65f;
        const float radius = h * .5f;
        const auto p = ImGui::GetCursorScreenPos();
        const auto label_size = ImGui::CalcTextSize(label, nullptr, true);
        const float gap = label_size.x > 0 ? ImGui::GetStyle().ItemInnerSpacing.x : 0;
        const bool pressed = ImGui::InvisibleButton(label, ImVec2(w + gap + label_size.x, h));
        if (pressed) *value = !*value;
        auto* d = ImGui::GetWindowDrawList();
        const auto bg = ImGui::GetColorU32(*value ? ImGuiCol_SliderGrab :
            (ImGui::IsItemHovered() ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg));
        d->AddRectFilled(p, ImVec2(p.x + w, p.y + h), bg, radius);
        d->AddRect(p, ImVec2(p.x + w, p.y + h), ImGui::GetColorU32(ImGuiCol_Border), radius);
        d->AddCircleFilled(ImVec2(p.x + (*value ? w - radius : radius), p.y + radius),
            radius - 3.f, ImGui::GetColorU32(ImGuiCol_Text));
        if (label_size.x > 0)
        {
            const char* end = std::strstr(label, "##");
            d->AddText(ImVec2(p.x + w + gap, p.y + (h - label_size.y) * .5f),
                ImGui::GetColorU32(ImGuiCol_Text), label, end);
        }
        return pressed;
    }
}
