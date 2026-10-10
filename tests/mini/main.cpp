#include "Mock.hpp"
#include "../../src/gui/mini/ClickTheme.cpp"
#include "../../src/gui/mini/MiniUI.cpp"
#include "../../src/gui/mini/MiniTravel.cpp"
#include "../../src/views/self/view_custom_teleport.cpp"
#include "../../src/renderer/GifAnimation.hpp"
int main(int argc,char** argv){using namespace big;assert(argc==2);g_file_manager.path=argv[1];
 auto add=[](std::map<tabs,navigation_struct>& map,tabs id,const char* label){auto& n=map[id];std::strncpy(n.name,label,79);n.hash=rage::joaat(label);n.func=[] {++legacyCalls;};};
 for(auto entry:{std::pair{tabs::SELF,"Self"},{tabs::VEHICLE,"Vehicle"},{tabs::WORLD,"World"},{tabs::NETWORK,"Network"},{tabs::SETTINGS,"Settings"},{tabs::PLAYER,"Players"}})add(service.nav,entry.first,entry.second);
 add(service.nav.at(tabs::SELF).sub_nav,tabs::TELEPORT,"Teleport");add(service.nav.at(tabs::SELF).sub_nav.at(tabs::TELEPORT).sub_nav,tabs::CUSTOM_TELEPORT,"Saved");add(service.nav.at(tabs::SETTINGS).sub_nav,tabs::LUA_SCRIPTS,"Lua Scripts");
 auto clean=[] {assert(ImGui::windows==0&&ImGui::children==0&&ImGui::ids==0&&ImGui::fonts==0&&ImGui::styleColors==0&&ImGui::tables==0);};
 const float rounding=ImGui::GetStyle().WindowRounding;mini::Draw();clean();assert(ImGui::GetStyle().WindowRounding==rounding);assert(ClickUI::ActiveThemeName()=="Rockstar Mini");assert(ClickUI::CurrentTheme().Width==640);assert(ClickUI::ThemeNames().size()==5);
 ImGui::click="\xef\x86\xb9";mini::Draw();clean();assert(service.selected==std::vector<tabs>{tabs::VEHICLE});ImGui::availableWidth=330;mini::Draw();clean();ImGui::availableWidth=740;
 ImGui::click="Legacy Controls";mini::Draw();clean();assert(mini::vehicleAdvanced);ImGui::click="Main";mini::Draw();clean();assert(!mini::vehicleAdvanced);
 ImGui::click="\xef\x82\xac";mini::Draw();clean();assert(service.selected==(std::vector<tabs>{tabs::SELF,tabs::TELEPORT}));ImGui::click="Directional";mini::Draw();clean();assert(mini::travelTab==1);ImGui::click="Saved";mini::Draw();clean();assert(mini::travelTab==2);
 ImGui::click="+ Save section";mini::Draw();clean();assert(mini::favorites.size()==1);
 ImGui::begin=false;mini::Draw();clean();ImGui::begin=true;ImGui::child=false;mini::Draw();clean();ImGui::child=true;
 g_custom_teleport_service.data["Default"]={{"Beach",1,2,3}};ImGui::click="Beach";view::custom_teleport();assert(jobs.empty());ImGui::click="Teleport";view::custom_teleport();assert(jobs.size()==1);g_custom_teleport_service.data["Default"][0].x=99;auto run=[] {auto queued=std::move(jobs);jobs.clear();for(auto& f:queued)f();};run();assert(lastCoords.x==1);
 ImGui::pop=true;inputValues["Category"]="  Holiday  ";inputValues["Name"]="  Pier  ";ImGui::click="Save";view::custom_teleport();assert(jobs.size()==1);inputValues["Name"]="Changed after click";run();assert(g_custom_teleport_service.data["Holiday"].front().name=="Pier");assert(g_custom_teleport_service.data["Holiday"].front().x==4);inputValues.clear();ImGui::pop=false;
 ImGui::click="Pier";view::custom_teleport();ImGui::click="Delete...";view::custom_teleport();assert(jobs.empty());ImGui::pop=true;ImGui::click="Delete location";view::custom_teleport();assert(jobs.size()==1);run();assert(g_custom_teleport_service.data["Holiday"].empty());clean();
 assert(LocationBrowser::Trim(" Home ")=="Home");assert(!LocationBrowser::ValidName("Bad##id"));assert(LocationBrowser::Filter(std::vector<telelocation>{{"Beach",0,0,0}},"BEA").size()==1);
 Animation::Canvas canvas(1,1);auto frame=canvas.Push(0,0,1,1,{255,0,0,255},0,20);assert(frame.Pixels[0]==255);Animation::Clip clip{1,1,{frame,frame}};assert(Animation::FrameAt(clip,21)==1&&Animation::FrameAt(clip,41)==0);
 std::cout<<"PASS: real Legacy Mini UI/Travel/Saved source against mocks; routes, callbacks, clipped windows, stacks/style restoration, favorites, themes, category filtering and copied save/teleport/delete fibers; portable GIF timing/compositing. Native GTA/D3D11/WIC not exercised.\n";
}
