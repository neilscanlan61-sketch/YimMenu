#include "MiniUI.hpp"
#include "ClickTheme.hpp"
#include "LocationBrowserPolicy.hpp"
#include "util/teleport.hpp"
#include "views/view.hpp"
#include <cmath>
namespace big::mini {
void Travel(int tab){
 if(tab==0){bool table=ImGui::GetContentRegionAvail().x>=500 && ImGui::BeginTable("travel-cards",2,ImGuiTableFlags_SizingStretchSame);if(table)ImGui::TableNextColumn();
 if(ImGui::BeginChild("Destinations",{0,200},true)){ImGui::TextColored(ClickUI::CurrentTheme().Accent,"Destinations");ImGui::Separator();components::command_button<"waypointtp">({},"Teleport to Waypoint");components::command_button<"objectivetp">({},"Teleport to Objective");components::command_button<"highlighttp">({},"Teleport to Selected Blip");}ImGui::EndChild();if(table)ImGui::TableNextColumn();
 if(ImGui::BeginChild("Waypoint",{0,130},true)){ImGui::TextColored(ClickUI::CurrentTheme().Accent,"Waypoint");ImGui::Separator();components::command_checkbox<"autotptowp">("Auto Teleport to Waypoint");}ImGui::EndChild();if(table)ImGui::EndTable();
 }else {static float distance=1.f;ImGui::TextUnformatted("Distance");ImGui::SetNextItemWidth(-1);ImGui::InputFloat("##directional-distance",&distance);auto move=[&](const char* text,float x,float y,float z){if(ImGui::Button(text,{-1,28})){const float amount=distance;if(!std::isfinite(amount)||amount<=0||amount>10000){g_notification_service.push_warning("Teleport","Enter a distance above zero and at most 10000.");return;}g_fiber_pool->queue_job([amount,x,y,z]{if(!self::ped)return;teleport::to_coords(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(self::ped,x*amount,y*amount,z*amount));});}};
 ImGui::SeparatorText("Forward / Backward");move("Forward",0,1,0);move("Backward",0,-1,0);ImGui::SeparatorText("Left / Right");move("Left",-1,0,0);move("Right",1,0,0);ImGui::SeparatorText("Up / Down");move("Up",0,0,1);move("Down",0,0,-1);}
}
}
