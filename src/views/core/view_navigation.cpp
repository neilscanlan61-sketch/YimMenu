#include "services/gui/gui_service.hpp"
#include "views/view.hpp"

namespace big
{
	void view::navigation()
	{
		ImGui::SetNextWindowPos({10.f, 10.f + (
#ifdef YIM_DEV
            102.f
#else
            74.f
#endif
        ) * g.window.gui_scale}, ImGuiCond_Always);
		ImGui::SetNextWindowSize({200.f * g.window.gui_scale, 400.f * g.window.gui_scale}, ImGuiCond_Always);

		if (ImGui::Begin("navigation", 0, window_flags))
		{
			compact_ui::decorate_window();
			g_gui_service->reset_nav_size();
			for (std::pair<tabs, navigation_struct> nav_item : g_gui_service->get_navigation())
			{
				switch (nav_item.first)
				{
				case tabs::PLAYER:
				case tabs::DEBUG: continue;
				default: components::nav_item(nav_item, 0);
				}
			}
		}
		ImGui::End();
	}
}
