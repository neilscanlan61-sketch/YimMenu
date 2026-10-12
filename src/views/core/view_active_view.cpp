#include "views/view.hpp"

#include "lua/lua_manager.hpp"
#include "pointers.hpp"
#include "services/gui/gui_service.hpp"
#include "services/translation_service/translation_service.hpp"

namespace big
{
	void view::active_view()
	{
		const auto selected = g_gui_service->get_selected();

		if (selected->func == nullptr &&
			(g_lua_manager && !g_lua_manager->has_gui_to_draw(selected->hash)))
		{
			return;
		}

		constexpr float alpha = 1.f;

        const float scale = g.window.gui_scale;
        const auto display = ImGui::GetIO().DisplaySize;
        const float x = 20.f + 200.f * scale;
        const float y = 10.f + (
#ifdef YIM_DEV
            102.f
#else
            74.f
#endif
        ) * scale;
        ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(std::max(80.f, std::min(640.f * scale, display.x - x - 10.f)),
            std::max(80.f, std::min(400.f * scale, display.y - y - 10.f))), ImGuiCond_Always);


		if (ImGui::Begin("main", nullptr, window_flags | ImGuiWindowFlags_HorizontalScrollbar))
		{
			compact_ui::decorate_window();
			const char* key = nullptr;
			if (key = g_translation_service.get_translation(selected->hash).data(); !key)
				key = selected->name;

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			components::title(key);
			ImGui::Separator();

			if (selected->func)
			{
				selected->func();

				const auto has_lua_gui_to_draw = g_lua_manager && g_lua_manager->has_gui_to_draw(selected->hash);
				if (has_lua_gui_to_draw)
					ImGui::Separator();
			}

			if (g_lua_manager)
				g_lua_manager->draw_gui(selected->hash);

			ImGui::PopStyleVar();
		}
		ImGui::End();
	}
}
