#include "fonts/fonts.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/gui/gui_service.hpp"
#include "services/player_database/player_database_service.hpp"
#include "services/players/player_service.hpp"
#include "views/view.hpp"

#define IMGUI_DEFINE_PLACEMENT_NEW
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace big
{
	bool has_scrollbar = false;
	static void player_button(const player_ptr& plyr)
	{
		if (plyr == nullptr || !plyr->is_valid())
			return;

		bool selected_player = plyr == g_player_service->get_selected();

		// generate icons string
		std::string player_icons;
		if (plyr->is_host())
			player_icons += FONT_ICON_HOST;
		if (plyr->is_friend())
			player_icons += FONT_ICON_FRIEND;
		if (const auto ped = plyr->get_ped(); (ped != nullptr
		        && (ped->m_ped_task_flag & (uint8_t)ePedTask::TASK_DRIVING || PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(plyr->id()))))
			player_icons += FONT_ICON_VEHICLE;

		const auto player_iconsc    = player_icons.c_str();
		const auto player_icons_end = player_iconsc + player_icons.size();

		// calculate icons width
		const auto window = ImGui::GetCurrentWindow();
		ImGui::PushFont(g.window.font_icon);
		const auto icons_size = ImGui::CalcTextSize(player_iconsc, player_icons_end);
		const ImVec2 icons_pos(window->Pos.x + window->Size.x - 16.f - icons_size.x, window->DC.CursorPos.y + 2.0f);
		const ImRect icons_box(icons_pos, icons_pos + icons_size);
		ImGui::PopFont();

		if (plyr->is_admin)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.67f, 0.f, 1.f));
		else if (plyr->is_modder)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.1f, 0.1f, 1.f));
		else if (plyr->is_trusted)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.67f, 0.1f, 1.f));

		if (selected_player)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.29f, 0.45f, 0.69f, 1.f));

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.0, 0.5});
		ImGui::PushID(plyr->id());

		// Fit each row to its containing list box.
		// branchless conditional calculation
		const auto plyr_btn_width = ImGui::GetContentRegionAvail().x;
		if (ImGui::Button(plyr->get_name(), { plyr_btn_width, 0.f}))
		{
			g_player_service->set_selected(plyr);
			g_gui_service->set_selected(tabs::PLAYER);
			g.window.switched_view = true;
		}
		if (auto rockstar_id = plyr->get_rockstar_id(); ImGui::IsItemHovered() && g_player_database_service->get_player_by_rockstar_id(rockstar_id) != nullptr)
		{
			auto sorted_player = g_player_database_service->get_player_by_rockstar_id(rockstar_id);
			if (!sorted_player->infractions.empty())
			{
				ImGui::BeginTooltip();
				for (auto infraction : sorted_player->infractions)
					ImGui::BulletText(sorted_player->get_infraction_description(infraction));
				ImGui::EndTooltip();
			}
		}

		ImGui::PopID();
		ImGui::PopStyleVar();

		if (selected_player)
			ImGui::PopStyleColor();

		if (plyr->is_admin || plyr->is_modder || plyr->is_trusted)
			ImGui::PopStyleColor();

		// render icons on top of the player button
		ImGui::PushFont(g.window.font_icon);
		ImGui::RenderTextWrapped(icons_box.Min, player_iconsc, player_icons_end, icons_size.x);
		ImGui::PopFont();
	}

	void view::players()
	{
		// player count does not include ourself that's why +1
		const auto player_count = g_player_service->players().size() + 1;

		if (!*g_pointers->m_gta.m_is_session_started && player_count < 2)
			return;
        const float window_pos = 10.f + (
#ifdef YIM_DEV
            510.f
#else
            482.f
#endif
        ) * g.window.gui_scale;
        const float available_height = ImGui::GetIO().DisplaySize.y - window_pos - 10.f;
        if (available_height < 60.f)
            return;
        ImGui::SetNextWindowSize(ImVec2(200.f * g.window.gui_scale,
            std::min(180.f * g.window.gui_scale, available_height)), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(10.f, window_pos), ImGuiCond_Always);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {2.0f, 2.0f});

		if (ImGui::Begin("playerlist", nullptr, window_flags))
		{
			compact_ui::decorate_window();
            const float window_height = ImGui::GetContentRegionAvail().y;
            const float row_height = ImGui::GetFrameHeightWithSpacing();
            has_scrollbar = row_height * static_cast<float>(player_count) > window_height;

			ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.f, 0.f, 0.f, 0.f});
			ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, {0.f, 0.f, 0.f, 0.f});

			if (ImGui::BeginListBox("##players", {ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, window_height}))
			{
				ImGui::SetScrollX(0);
				player_button(g_player_service->get_self());

				if (player_count > 1)
					ImGui::Separator();

				for (const auto& [_, player] : g_player_service->players())
					player_button(player);

				ImGui::EndListBox();
			}
			ImGui::PopStyleColor(2);
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}
}
