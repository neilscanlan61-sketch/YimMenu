#include "renderer/renderer.hpp"
#include "views/view.hpp"
#include "gui/mini/MiniUI.hpp"

namespace big
{
	void view::gui_settings()
	{
        mini::Themes();

		components::sub_title("VIEW_GUI_SETTINGS_INGAME_OVERLAY"_T);
		ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_OVERLAY"_T.data(), &g.window.ingame_overlay.opened);
		ImGui::SameLine();
		ImGui::Checkbox("VIEW_GUI_SETTINGS_OVERLAY_SHOW_WHEN_MENU_OPENED"_T.data(), &g.window.ingame_overlay.show_with_menu_opened);

		ImGui::BeginGroup();

		ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_FPS"_T.data(), &g.window.ingame_overlay.show_fps);
		ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_PLAYERS"_T.data(), &g.window.ingame_overlay.show_players);
		ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_TIME"_T.data(), &g.window.ingame_overlay.show_time);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("VIEW_GUI_SETTINGS_SHOW_TIME_TOOLTIP"_T.data());
		ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_INDICATORS"_T.data(), &g.window.ingame_overlay.show_indicators);

		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();

		ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_REPLAY_INTERFACE"_T.data(), &g.window.ingame_overlay.show_replay_interface);
		ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_POSITION"_T.data(), &g.window.ingame_overlay.show_position);
		ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_GAME_VERSION"_T.data(), &g.window.ingame_overlay.show_game_versions);

		ImGui::EndGroup();

		ImGui::Checkbox("VIEW_GUI_FORMAT_MONEY"_T.data(), &g.window.gui.format_money);
		ImGui::SameLine();
		ImGui::Checkbox("METRIC_WARNING"_T.data(), &g.notifications.warn_metric);

		if (g.window.ingame_overlay.show_indicators)
		{
			if (ImGui::TreeNode("VIEW_GUI_SETTINGS_OVERLAY_INDICATORS"_T.data()))
			{
				ImGui::BeginGroup();
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_PLAYER_GODMODE"_T.data(), &g.window.ingame_overlay_indicators.show_player_godmode);
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_OFF_THE_RADAR"_T.data(), &g.window.ingame_overlay_indicators.show_off_radar);
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_VEHICLE_GODMODE"_T.data(), &g.window.ingame_overlay_indicators.show_vehicle_godmode);
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_NEVER_WANTED"_T.data(), &g.window.ingame_overlay_indicators.show_never_wanted);
				ImGui::EndGroup();

				ImGui::SameLine();

				ImGui::BeginGroup();
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_INFINITE_AMMO"_T.data(), &g.window.ingame_overlay_indicators.show_infinite_ammo);
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_ALWAYS_FULL_AMMO"_T.data(), &g.window.ingame_overlay_indicators.show_always_full_ammo);
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_INFINITE_MAGAZINE"_T.data(), &g.window.ingame_overlay_indicators.show_infinite_mag);
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_AIMBOT"_T.data(), &g.window.ingame_overlay_indicators.show_aimbot);
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_TRIGGERBOT"_T.data(), &g.window.ingame_overlay_indicators.show_triggerbot);
				ImGui::Checkbox("VIEW_GUI_SETTINGS_SHOW_INVISIBILITY"_T.data(), &g.window.ingame_overlay_indicators.show_invisibility);
				ImGui::EndGroup();

				ImGui::TreePop();
			}
		}
	}

}
