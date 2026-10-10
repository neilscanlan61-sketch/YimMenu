#include "MiniUI.hpp"
#include "ClickTheme.hpp"
#include "MiniFonts.hpp"
#include "LocationBrowserPolicy.hpp"
#include "renderer/MiniBackground.hpp"
#include "services/gui/gui_service.hpp"
#include "services/translation_service/translation_service.hpp"
#include "services/players/player_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "lua/lua_manager.hpp"
#include "file_manager.hpp"
#include "util/mobile.hpp"
#include "util/vehicle.hpp"
#include "views/view.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
namespace big::mini {
namespace {
using Route=std::vector<tabs>;
struct Entry {Route route;std::string label;};
std::vector<Route> favorites;
bool initialized=false,vehicleAdvanced=false;
char search[96]{},playerSearch[64]{};
int travelTab=0;
std::string Name(const navigation_struct& node){auto text=g_translation_service.get_translation(node.hash);return text.empty()?std::string(node.name):std::string(text);}
void Visit(const std::map<tabs,navigation_struct>& nav,Route prefix,std::string parent,std::vector<Entry>& entries){
 for(const auto& item:nav){auto path=prefix;path.push_back(item.first);auto label=Name(item.second);if(label.empty())continue;
 auto full=parent.empty()?label:parent+" / "+label;entries.push_back({path,full});Visit(item.second.sub_nav,path,full,entries);}}
navigation_struct* Resolve(const Route& path){auto* nav=&g_gui_service->get_navigation();navigation_struct* result=nullptr;for(auto tab:path){auto it=nav->find(tab);if(it==nav->end())return nullptr;result=&it->second;nav=&result->sub_nav;}return result;}
void Select(const Route& path){if(!Resolve(path))return;if(path==Route{tabs::SELF,tabs::TELEPORT,tabs::CUSTOM_TELEPORT})travelTab=2;g_gui_service->get_selected_tab()=path;g.window.switched_view=true;}
void SaveFavorites(){std::ofstream file(ClickUI::ThemeFolder()/"legacy-bookmarks.txt");for(const auto& route:favorites){for(auto tab:route)file<<static_cast<int>(tab)<<' ';file<<'\n';}}
void Initialize(){if(initialized)return;initialized=true;ClickUI::InitializeThemes(g_file_manager.get_project_folder("themes/click-ui").get_path());std::ifstream file(ClickUI::ThemeFolder()/"legacy-bookmarks.txt");std::string line;while(favorites.size()<64 && std::getline(file,line)){std::istringstream in(line);int n;Route route;while(route.size()<16 && in>>n)route.push_back(static_cast<tabs>(n));if(!route.empty() && Resolve(route))favorites.push_back(route);}if(!Resolve(g_gui_service->get_selected_tab()))Select({tabs::SELF});}
bool Icon(const char* id,const char* glyph,const char* label,bool active=false){ImGui::PushID(id);if(active)ImGui::PushStyleColor(ImGuiCol_Button,ClickUI::CurrentTheme().Selection);if(icons)ImGui::PushFont(icons);bool pressed=ImGui::Button(glyph,{38,36});if(icons)ImGui::PopFont();if(active)ImGui::PopStyleColor();if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label);ImGui::PopID();return pressed;}
void Card(const char* label,float height,const std::function<void()>& body){if(ImGui::BeginChild(label,{0,height},true)){ImGui::TextColored(ClickUI::CurrentTheme().Accent,"%s",label);ImGui::Separator();body();}ImGui::EndChild();}
void Action(const char* label,const std::function<void()>& task){if(ImGui::Button(label,{-1,28}))g_fiber_pool->queue_job(task);}
void Cmd(const char* id,const char* label){auto* cmd=command::get(rage::joaat(id));if(!cmd){ImGui::TextWrapped("Unavailable in this Legacy source: %s",label);return;}if(ImGui::Button(label,{-1,28})){command_arguments args(std::vector<uint64_t>{});cmd->call(args);}}
struct Style {ImGuiStyle previous=ImGui::GetStyle();Style(){auto& s=ImGui::GetStyle();const auto& t=ClickUI::CurrentTheme();s.WindowPadding={8,8};s.ItemSpacing={5,4};s.ItemInnerSpacing={5,3};s.FramePadding={5,3};s.WindowRounding=t.Rounding;s.ChildRounding=t.Rounding;s.FrameRounding=std::clamp(t.Rounding,6.f,12.f);s.GrabRounding=s.FrameRounding;s.TabRounding=s.FrameRounding;s.FrameBorderSize=1;s.WindowBorderSize=1;s.ScrollbarSize=10;s.Colors[ImGuiCol_WindowBg]=t.Background;s.Colors[ImGuiCol_ChildBg]=t.Surface;s.Colors[ImGuiCol_PopupBg]={t.Background.x,t.Background.y,t.Background.z,1};s.Colors[ImGuiCol_Text]=t.Text;s.Colors[ImGuiCol_TextDisabled]=t.Muted;s.Colors[ImGuiCol_Border]=t.Border;s.Colors[ImGuiCol_Button]=t.Surface;s.Colors[ImGuiCol_ButtonHovered]=t.ButtonHover;s.Colors[ImGuiCol_ButtonActive]=t.Selection;s.Colors[ImGuiCol_FrameBg]=t.Surface;s.Colors[ImGuiCol_FrameBgHovered]=t.ButtonHover;s.Colors[ImGuiCol_FrameBgActive]=t.Selection;s.Colors[ImGuiCol_CheckMark]=t.Accent;s.Colors[ImGuiCol_SliderGrab]=t.Accent;s.Colors[ImGuiCol_Header]=t.Selection;s.Colors[ImGuiCol_HeaderHovered]=t.ButtonHover;s.Colors[ImGuiCol_HeaderActive]=t.Selection;s.Colors[ImGuiCol_TitleBg]=t.Header;s.Colors[ImGuiCol_TitleBgActive]=t.Header;}~Style(){ImGui::GetStyle()=previous;}};
void Paint(){const auto& t=ClickUI::CurrentTheme();auto* d=ImGui::GetWindowDrawList();const auto pos=ImGui::GetWindowPos(),size=ImGui::GetWindowSize();d->AddRectFilled(pos,{pos.x+size.x,pos.y+size.y},ImGui::GetColorU32(t.Background),t.Rounding);DrawBackground(d,pos,size);d->PushClipRect({pos.x+8,pos.y+8},{pos.x+size.x-8,pos.y+size.y-8},true);const ImVec2 centers[]={{.18f,.8f},{.82f,.26f},{.5f,.9f}};for(int i=0;i<3;++i)for(int ring=12;ring>0;--ring){auto color=i==1?t.Accent:t.BackgroundLow;color.w=t.LightIntensity*static_cast<float>(13-ring)/156.f;d->AddCircleFilled({pos.x+size.x*centers[i].x,pos.y+size.y*centers[i].y},std::min(size.x,size.y)*.27f*static_cast<float>(ring)/12.f,ImGui::GetColorU32(color),32);}d->PopClipRect();}
void Players(){components::input_text_with_hint("##players","Search players",playerSearch,sizeof(playerSearch));auto playerButton=[](const player_ptr& player){if(!player || !player->is_valid() || !LocationBrowser::Matches(player->get_name(),playerSearch))return;ImGui::PushID(player->id());if(ImGui::Selectable(player->get_name(),player==g_player_service->get_selected())){g_player_service->set_selected(player);Select({tabs::PLAYER});}ImGui::PopID();};playerButton(g_player_service->get_self());for(const auto& entry:g_player_service->players())playerButton(entry.second);if(g_player_service->get_selected()){ImGui::Separator();view::view_player();}}
}
void Themes(){Initialize();ImGui::TextUnformatted("Theme");ImGui::SetNextItemWidth(-1);if(ImGui::BeginCombo("##mini-theme",ClickUI::ActiveThemeName().c_str())){for(const auto& name:ClickUI::ThemeNames())if(ImGui::Selectable(name.c_str(),name==ClickUI::ActiveThemeName()) && ClickUI::LoadTheme(name))ReloadBackground();ImGui::EndCombo();}static char custom[49]="Rojo Mini";ImGui::TextUnformatted("Custom theme name");ImGui::SetNextItemWidth(-1);ImGui::InputText("##custom-mini-theme",custom,sizeof(custom));if(ImGui::Button("Load custom theme",{-1,28}) && ClickUI::LoadTheme(custom))ReloadBackground();if(ImGui::Button("Refresh themes / GIF",{-1,28})){ClickUI::ThemeNames();ReloadBackground();}auto& t=ClickUI::CurrentTheme();bool enabled=t.BackgroundEnabled!=0;if(ImGui::Checkbox("Animated GIF background",&enabled)){t.BackgroundEnabled=enabled?1.f:0.f;ClickUI::SaveTheme(ClickUI::ActiveThemeName());ReloadBackground();}ImGui::TextWrapped("%s",ClickUI::ThemeStatus().c_str());ImGui::TextWrapped("%s",BackgroundStatus().c_str());ImGui::TextWrapped("Theme folder: %s",ClickUI::ThemeFolder().string().c_str());ImGui::TextWrapped("Use matching My Theme.theme and My Theme.gif files. Advanced colors and dimensions are edited in the theme file.");}
void VehicleMain(){
 const bool columns=ImGui::GetContentRegionAvail().x>=500;bool table=columns && ImGui::BeginTable("mini-vehicle-cards",2,ImGuiTableFlags_SizingStretchSame);
 if(table)ImGui::TableNextColumn();
 Card("Tools",250,[]{Cmd("lastvehtp","Enter Last Vehicle");Action("Repair Vehicle",[]{vehicle::repair(self::veh);});if(ImGui::Button("Mechanic / Vehicle Requests",{-1,28}))Select({tabs::SELF,tabs::MOBILE});Cmd("bringpv","Bring Personal Vehicle");if(ImGui::Button("Save Current Vehicle (Local)",{-1,28}))ImGui::OpenPopup("Save vehicle locally");
 if(ImGui::BeginPopupModal("Save vehicle locally",nullptr,ImGuiWindowFlags_AlwaysAutoResize)){static char name[49]="My Vehicle";ImGui::InputText("Name",name,sizeof(name));if(ImGui::Button("Save")){std::string captured=name;bool valid=!captured.empty() && captured.size()<=48;for(unsigned char c:captured)valid=valid && ((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c==' ');std::string upper=captured;std::transform(upper.begin(),upper.end(),upper.begin(),[](unsigned char c){return static_cast<char>(std::toupper(c));});valid=valid && !captured.empty() && captured.front()!=' ' && captured.back()!=' ' && upper!="CON" && upper!="PRN" && upper!="AUX" && upper!="NUL";if(upper.size()==4 && (upper.substr(0,3)=="COM" || upper.substr(0,3)=="LPT") && upper[3]>='1' && upper[3]<='9')valid=false;if(valid){g_fiber_pool->queue_job([captured]{persist_car_service::save_vehicle(self::veh,captured,"");});ImGui::CloseCurrentPopup();}else g_notification_service.push_warning("Vehicle","Use letters, numbers, spaces, dash or underscore.");}ImGui::SameLine();if(ImGui::Button("Cancel"))ImGui::CloseCurrentPopup();ImGui::TextWrapped("Local vehicle JSON only; this is not conversion to an online PV.");ImGui::EndPopup();}
 });
 if(table)ImGui::TableNextColumn();
 Card("Globals",145,[]{components::command_checkbox<"vehgodmode">("Godmode");components::command_checkbox<"keepfixed">("Keep Fixed");components::command_checkbox<"hornboost">("Horn Boost");});
 Card("Misc",140,[]{components::command_checkbox<"speedometer">("Speedometer");components::command_checkbox<"seatbelt">("Seatbelt");components::command_checkbox<"keepvehicleclean">("Keep Vehicle Clean");});
 if(table)ImGui::EndTable();
}
void Draw(){
 Initialize();Style scope;bool openFavorites=false;const auto& t=ClickUI::CurrentTheme();const auto display=ImGui::GetIO().DisplaySize;
 ImGui::SetNextWindowSize({std::min(t.Width,std::max(220.f,display.x-24)),std::min(t.Height,std::max(220.f,display.y-24))},ImGuiCond_Always);
 ImGui::SetNextWindowPos({24,24},ImGuiCond_FirstUseEver);
 if(options)ImGui::PushFont(options);
 const bool shown=ImGui::Begin("##RockstarToolLegacyMini",nullptr,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
 if(shown){
 Paint();if(title)ImGui::PushFont(title);ImGui::TextUnformatted("ROCKSTAR TOOL");if(title)ImGui::PopFont();
 float remaining=ImGui::GetContentRegionAvail().x; // on next line this is the window width; header controls use explicit right positions.
 if(remaining>390){ImGui::SameLine(std::max(180.f,remaining-216.f));ImGui::SetNextItemWidth(130);ImGui::InputTextWithHint("##section-search","Search sections",search,sizeof(search));ImGui::SameLine();}
 else {ImGui::SetNextItemWidth(std::max(60.f,remaining-90));ImGui::InputTextWithHint("##section-search","Search",search,sizeof(search));ImGui::SameLine();}
 if(icons)ImGui::PushFont(icons);if(ImGui::Button("\xef\x87\xbc##themes",{28,28}))ImGui::OpenPopup("Themes");ImGui::SameLine();bool favButton=ImGui::Button("\xef\x80\x85##favorites",{28,28});if(icons)ImGui::PopFont();if(favButton)ImGui::OpenPopup("Favorites");
 ImGui::Separator();
 Route path=g_gui_service->get_selected_tab();
 if(ImGui::BeginChild("##rail",{44,0},false)){
 if(Icon("self","\xef\x80\x87","Self",path==Route{tabs::SELF}))Select({tabs::SELF});
 if(Icon("players","\xef\x83\x80","Players",path==Route{tabs::PLAYER}))Select({tabs::PLAYER});
 if(Icon("network","\xef\x83\xa8","Network",!path.empty()&&path.front()==tabs::NETWORK))Select({tabs::NETWORK});
 if(Icon("teleport","\xef\x82\xac","Teleport",std::find(path.begin(),path.end(),tabs::TELEPORT)!=path.end()))Select({tabs::SELF,tabs::TELEPORT});
 if(Icon("vehicle","\xef\x86\xb9","Vehicle",!path.empty()&&path.front()==tabs::VEHICLE))Select({tabs::VEHICLE});
 if(Icon("world","\xef\x82\xac","World",!path.empty()&&path.front()==tabs::WORLD))Select({tabs::WORLD});
 if(Icon("settings","\xef\x80\x93","Settings",!path.empty()&&path.front()==tabs::SETTINGS))Select({tabs::SETTINGS});
 if(Icon("scripts","\xef\x84\xa0","Lua Scripts",std::find(path.begin(),path.end(),tabs::LUA_SCRIPTS)!=path.end()))Select({tabs::SETTINGS,tabs::LUA_SCRIPTS});
 }ImGui::EndChild();ImGui::SameLine();
 if(ImGui::BeginChild("##content",{0,0},false)){
 path=g_gui_service->get_selected_tab();auto* node=Resolve(path);
 if(search[0]){std::vector<Entry> all;Visit(g_gui_service->get_navigation(),{},"",all);for(const auto& entry:all)if(LocationBrowser::Matches(entry.label,search) && ImGui::Selectable(entry.label.c_str())){Select(entry.route);search[0]=0;}}
 else if(node){
 const bool travel=path==Route{tabs::SELF,tabs::TELEPORT}||path==Route{tabs::SELF,tabs::TELEPORT,tabs::CUSTOM_TELEPORT};
 if(ImGui::BeginChild("##tabs",{0,36},false,ImGuiWindowFlags_HorizontalScrollbar)){
 if(travel){const char* labels[]={"Quick Travel","Directional","Saved","Legacy Extras"};for(int i=0;i<4;++i){if(i)ImGui::SameLine();const bool active=i==travelTab;if(active)ImGui::PushStyleColor(ImGuiCol_Button,t.Selection);if(ImGui::Button(labels[i],{0,28}))travelTab=i;if(active)ImGui::PopStyleColor();}}
 else {if(ImGui::Button("Main",{0,28})){if(path.size()>1)path.pop_back();Select(path);vehicleAdvanced=false;}if(path==Route{tabs::VEHICLE}){ImGui::SameLine();if(ImGui::Button("Legacy Controls",{0,28}))vehicleAdvanced=true;}auto children=node->sub_nav;for(const auto& item:children){ImGui::SameLine();ImGui::PushID(static_cast<int>(item.first));if(ImGui::Button(Name(item.second).c_str(),{0,28})){auto next=path;next.push_back(item.first);Select(next);}ImGui::PopID();}}
 }ImGui::EndChild();
 if(ImGui::SmallButton("+ Save section")){auto route=g_gui_service->get_selected_tab();if(favorites.size()<64 && std::find(favorites.begin(),favorites.end(),route)==favorites.end()){favorites.push_back(route);SaveFavorites();}}ImGui::SameLine();if(ImGui::SmallButton("Favorites"))openFavorites=true;ImGui::Separator();
 if(ImGui::BeginChild("##body",{0,0},false,ImGuiWindowFlags_HorizontalScrollbar)){
 path=g_gui_service->get_selected_tab();node=Resolve(path);
 if(travel){if(travelTab==2)view::custom_teleport();else if(travelTab==3)view::teleport();else {extern void Travel(int);Travel(travelTab);}}
 else if(path==Route{tabs::VEHICLE}&&!vehicleAdvanced)VehicleMain();
 else if(path==Route{tabs::PLAYER})Players();
 else if(node){if(node->func)node->func();if(g_lua_manager)g_lua_manager->draw_gui(node->hash);}
 }ImGui::EndChild();
 }
 }ImGui::EndChild();
 ImGui::SetNextWindowSize({std::min(440.f,std::max(220.f,display.x-24)),std::min(390.f,std::max(220.f,display.y-24))},ImGuiCond_Appearing);
 if(ImGui::BeginPopupModal("Themes",nullptr,ImGuiWindowFlags_NoResize)){Themes();if(ImGui::Button("Close"))ImGui::CloseCurrentPopup();ImGui::EndPopup();}
 if(openFavorites)ImGui::OpenPopup("Favorites");
 if(ImGui::BeginPopup("Favorites")){for(size_t i=0;i<favorites.size();){auto* item=Resolve(favorites[i]);if(!item){favorites.erase(favorites.begin()+static_cast<std::ptrdiff_t>(i));SaveFavorites();continue;}ImGui::PushID(static_cast<int>(i));if(ImGui::Selectable(Name(*item).c_str(),false,ImGuiSelectableFlags_DontClosePopups))Select(favorites[i]);ImGui::SameLine();if(ImGui::SmallButton("Remove")){favorites.erase(favorites.begin()+static_cast<std::ptrdiff_t>(i));SaveFavorites();ImGui::PopID();continue;}ImGui::PopID();++i;}ImGui::EndPopup();}
 }
 ImGui::End();if(options)ImGui::PopFont();
}
}

namespace big::ClickUI { void DrawThemeEditor(){ mini::Themes(); } }
