#include "views/view.hpp"
#include "lua/lua_manager.hpp"

namespace big
{
	void view::heading()
	{
		ImGui::SetNextWindowSize({200.f * g.window.gui_scale,
#ifdef YIM_DEV
            94.f * g.window.gui_scale
#else
            66.f * g.window.gui_scale
#endif
        });
		ImGui::SetNextWindowPos({10.f, 10.f});
		if (ImGui::Begin("menu_heading", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar))
		{
			compact_ui::decorate_window();
			ImGui::BeginGroup();
			ImGui::Text("HEADING_WELCOME"_T.data());
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.78f, .50f, 1.f, 1.f));
			ImGui::Text(g_local_player == nullptr || g_local_player->m_player_info == nullptr ?
			        "UNKNOWN_USERNAME"_T.data() :
			        g_local_player->m_player_info->m_net_player_data.m_name);
			ImGui::PopStyleColor();
			ImGui::EndGroup();
#ifdef YIM_DEV
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.69f, 0.29f, 0.29f, 1.00f));
			if (components::nav_button("UNLOAD"_T))
			{
				g_lua_manager->trigger_event<menu_event::MenuUnloaded>();
				g_running = false;
			}
			ImGui::PopStyleColor();
#endif
		}
		ImGui::End();
	}
}
