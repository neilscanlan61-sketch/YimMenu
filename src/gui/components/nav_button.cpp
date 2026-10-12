#include "gui/components/components.hpp"

namespace big
{
    bool components::nav_button(const std::string_view text)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3));
        // Available width, not a percentage of game resolution.
        const bool result = ImGui::Button(text.data(), ImVec2(ImGui::GetContentRegionAvail().x, 0));
        ImGui::PopStyleVar(3);
        return result;
    }
}
