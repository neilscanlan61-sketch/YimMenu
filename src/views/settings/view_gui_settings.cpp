#include "renderer/renderer.hpp"
#include "gui.hpp"
#include "views/view.hpp"

namespace big
{
	void view::gui_settings()
	{
        static ImVec4 col_gui = ImGui::ColorConvertU32ToFloat4(g.window.background_color);
        static ImVec4 col_text = ImGui::ColorConvertU32ToFloat4(g.window.text_color);
        static ImVec4 col_button = ImGui::ColorConvertU32ToFloat4(g.window.button_color);
        static ImVec4 col_frame = ImGui::ColorConvertU32ToFloat4(g.window.frame_color);
        static auto gui_scale = g.window.gui_scale;
        if (ImGui::Button("Apply compact violet preset"))
        {
            g.window.background_color = IM_COL32(12, 6, 19, 247);
            g.window.text_color = IM_COL32(240, 235, 255, 255);
            g.window.button_color = IM_COL32(48, 19, 82, 255);
            g.window.frame_color = IM_COL32(36, 17, 59, 255);
            g.window.gui_scale = gui_scale = .90f;
            col_gui = ImGui::ColorConvertU32ToFloat4(g.window.background_color);
            col_text = ImGui::ColorConvertU32ToFloat4(g.window.text_color);
            col_button = ImGui::ColorConvertU32ToFloat4(g.window.button_color);
            col_frame = ImGui::ColorConvertU32ToFloat4(g.window.frame_color);
            g_gui->dx_init();
            g_renderer.rescale(g.window.gui_scale);
        }
        ImGui::Separator();
		components::sub_title("SETTINGS_UI_SCALE"_T);
		ImGui::SliderFloat("##gui-scale", &gui_scale, 0.75f, 1.5f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			g.window.gui_scale = gui_scale;
			g_renderer.rescale(g.window.gui_scale);
		}

		components::sub_title("SETTINGS_UI_COLOR"_T);
		if (ImGui::ColorEdit4("SETTINGS_UI_COLOR_PICKER"_T.data(), (float*)&col_gui, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
		{
			g.window.background_color = ImGui::ColorConvertFloat4ToU32(col_gui);
		}

		if (ImGui::ColorEdit4("VIEW_GUI_SETTINGS_TEXT_COLOR"_T.data(), (float*)&col_text, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
		{
			g.window.text_color = ImGui::ColorConvertFloat4ToU32(col_text);
		}

		if (ImGui::ColorEdit4("VIEW_GUI_SETTINGS_BUTTON_COLOR"_T.data(), (float*)&col_button, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
		{
			g.window.button_color = ImGui::ColorConvertFloat4ToU32(col_button);
		}

		if (ImGui::ColorEdit4("VIEW_GUI_SETTINGS_FRAME_COLOR"_T.data(), (float*)&col_frame, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
		{
			g.window.frame_color = ImGui::ColorConvertFloat4ToU32(col_frame);
		}

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
