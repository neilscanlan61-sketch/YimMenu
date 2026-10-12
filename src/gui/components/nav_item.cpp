#include "components.hpp"
#include "services/gui/gui_service.hpp"
#include "services/translation_service/translation_service.hpp"

namespace big
{
    void components::nav_item(std::pair<tabs, navigation_struct>& navItem, int nested)
    {
        const auto& path = g_gui_service->get_selected_tab();
        const bool current_tab = nested >= 0 && path.size() > static_cast<size_t>(nested)
            && navItem.first == path.at(nested);
        ImGui::PushID(static_cast<int>(navItem.first));
        if (current_tab)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(.35f, .14f, .55f, 1));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .94f, 1, 1));
        }
        else
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        const char* key = g_translation_service.get_translation(navItem.second.name).data();
        if (!key) key = navItem.second.name;
        if (components::nav_button(key))
            g_gui_service->set_selected(navItem.first);
        ImGui::PopStyleColor(current_tab ? 2 : 1);
        // Layout-local indentation replaces hardcoded screen-space rectangles.
        if (current_tab && !navItem.second.sub_nav.empty())
        {
            ImGui::Indent();
            for (std::pair<tabs, navigation_struct> item : navItem.second.sub_nav)
                nav_item(item, nested + 1);
            ImGui::Unindent();
        }
        ImGui::PopID();
        g_gui_service->increment_nav_size();
    }
}
