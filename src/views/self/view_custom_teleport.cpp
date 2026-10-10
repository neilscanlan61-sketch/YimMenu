#include "services/custom_teleport/custom_teleport_service.hpp"
#include "gui/mini/LocationBrowserPolicy.hpp"
#include "util/teleport.hpp"
#include "util/blip.hpp"
#include "views/view.hpp"
#include <optional>
namespace big {
namespace {
void QueuePlace(telelocation value){g_fiber_pool->queue_job([value]{if(!self::ped || !LocationBrowser::Finite(value))return;teleport::to_coords({value.x,value.y,value.z},{value.yaw,value.pitch,value.roll});});}
}
void view::custom_teleport(){
 static std::string category="Default",name,filter;
 static std::optional<telelocation> selected,deletion;
 static std::string deletionCategory;
 const auto saved=g_custom_teleport_service.snapshot();
 if(!saved.empty() && saved.find(category)==saved.end()){category=saved.begin()->first;selected.reset();}
 ImGui::TextWrapped("Select a location, then Teleport. Double-click also teleports.");
 ImGui::TextUnformatted("Category");ImGui::SetNextItemWidth(-1);
 if(ImGui::BeginCombo("##saved-category",category.c_str())){for(const auto& entry:saved)if(ImGui::Selectable(entry.first.c_str(),entry.first==category)){category=entry.first;selected.reset();}ImGui::EndCombo();}
 ImGui::SetNextItemWidth(-1);components::input_text_with_hint("##saved-search","Search this category",filter);
 if(ImGui::Button("Save current location...",{-1,28}))ImGui::OpenPopup("Save current location");
 if(ImGui::BeginListBox("##saved-list",{-1,150})){auto it=saved.find(category);if(it!=saved.end())for(const auto& place:LocationBrowser::Filter(it->second,filter)){ImGui::PushID(place.name.c_str());if(ImGui::Selectable(place.name.c_str(),selected && selected->name==place.name,ImGuiSelectableFlags_AllowDoubleClick)){selected=place;if(ImGui::IsMouseDoubleClicked(0))QueuePlace(place);}ImGui::PopID();}ImGui::EndListBox();}
 ImGui::BeginDisabled(!selected);if(ImGui::Button("Teleport",{-1,28})&&selected)QueuePlace(*selected);if(ImGui::Button("Delete...",{-1,28})&&selected){deletion=selected;deletionCategory=category;ImGui::OpenPopup("Delete saved location");}ImGui::EndDisabled();
 if(ImGui::BeginPopupModal("Delete saved location",nullptr,ImGuiWindowFlags_AlwaysAutoResize)){if(deletion){ImGui::TextWrapped("Delete %s from %s?",deletion->name.c_str(),deletionCategory.c_str());if(ImGui::Button("Delete location")){const auto place=*deletion;const auto capturedCategory=deletionCategory;g_fiber_pool->queue_job([capturedCategory,place]{if(!g_custom_teleport_service.delete_saved_location(capturedCategory,place.name))g_notification_service.push_warning("Teleport","Location deletion failed.");});selected.reset();deletion.reset();ImGui::CloseCurrentPopup();}}ImGui::SameLine();if(ImGui::Button("Cancel")){deletion.reset();ImGui::CloseCurrentPopup();}ImGui::EndPopup();}
 if(ImGui::BeginPopupModal("Save current location",nullptr,ImGuiWindowFlags_AlwaysAutoResize)){
 components::input_text_with_hint("Category","Default",category);components::input_text_with_hint("Name","Location name",name);
 auto save=[&](bool fromBlip){const auto capturedName=LocationBrowser::Trim(name),capturedCategory=LocationBrowser::Trim(category);
 if(!LocationBrowser::ValidName(capturedName)||!LocationBrowser::ValidName(capturedCategory)){g_notification_service.push_warning("Teleport","Use a name/category of 1 to 64 bytes, without control characters or #.");return;}
 category=capturedCategory;name=capturedName;selected.reset();
 g_fiber_pool->queue_job([capturedName,capturedCategory,fromBlip]{telelocation place{};place.name=capturedName;
 if(fromBlip){auto* b=blip::get_selected_blip();if(!b){g_notification_service.push_warning("Teleport","No selected blip.");return;}place.x=b->m_position.x;place.y=b->m_position.y;place.z=b->m_position.z;place.yaw=b->m_rotation;}
 else {const auto entity=self::veh?self::veh:self::ped;if(!entity)return;const auto pos=ENTITY::GET_ENTITY_COORDS(entity,TRUE);place.x=pos.x;place.y=pos.y;place.z=pos.z;place.yaw=ENTITY::GET_ENTITY_HEADING(entity);place.pitch=CAM::GET_GAMEPLAY_CAM_RELATIVE_PITCH();place.roll=CAM::GET_GAMEPLAY_CAM_RELATIVE_HEADING();}
 if(!LocationBrowser::Finite(place)||!g_custom_teleport_service.try_save_unique(capturedCategory,place))g_notification_service.push_warning("Teleport","Save failed, or that name already exists.");});ImGui::CloseCurrentPopup();};
 if(ImGui::Button("Save"))save(false);ImGui::SameLine();if(ImGui::Button("Save selected blip"))save(true);if(ImGui::Button("Cancel"))ImGui::CloseCurrentPopup();ImGui::EndPopup();
 }
}
}
