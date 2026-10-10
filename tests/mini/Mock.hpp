#pragma once
#include "CoreMock.hpp"
#include <map>
#include <optional>
#include <filesystem>
#include <cstring>
#include <sstream>
#include <cmath>
namespace ImGui {
inline int tables=0;inline bool doubleClick=false;
inline bool BeginTable(const char*,int,int){++tables;return true;}inline void EndTable(){--tables;}inline void TableNextColumn(){}
inline void BeginDisabled(bool){}inline void EndDisabled(){}inline void SeparatorText(const char*){}
inline bool InputFloat(const char*,float*){return false;}inline bool BeginListBox(const char*,ImVec2){return true;}inline void EndListBox(){}
inline bool IsMouseDoubleClicked(int){bool value=doubleClick;doubleClick=false;return value;}
}
enum{ImGuiTableFlags_SizingStretchSame=1,ImGuiWindowFlags_AlwaysAutoResize=32,ImGuiSelectableFlags_AllowDoubleClick=1,ImGuiSelectableFlags_DontClosePopups=2};
#define TRUE 1
namespace rage{using joaat_t=unsigned;inline unsigned joaat(const char* text){unsigned n=0;while(*text)n=n*31+static_cast<unsigned char>(*text++);return n;}}
struct Vector3{float x=0,y=0,z=0;};
namespace big {
enum class tabs
	{
		NONE,

		SELF,
		WEAPONS,
		WEAPONS_AMMUNATION,
		TELEPORT,
		CUSTOM_TELEPORT,
		MOBILE,
		OUTFIT_EDITOR,
		OUTFIT_SLOTS,
		ANIMATIONS,

		VEHICLE,
		HANDLING,
		HANDLING_SEARCH,
		HANDLING_SAVED_PROFILE,
		HANDLING_MY_PROFILES,
		HANDLING_CURRENT_PROFILE,
		LSC,
		SPAWN_VEHICLE,
		FUN_VEHICLE,

		WORLD,
		SPAWN_PED,
		SQUAD_SPAWNER,
		CREATOR,
		TRAIN,
		BLACKHOLE,
		MODEL_SWAPPER,
		VFX,
		XML_MAPS,

		NETWORK,
		CHAT,
		NETWORK_CONTROLS,
		MISSIONS,
		SPOOFING,
		PLAYER_DATABASE,
		SESSION_BROWSER,
		STAT_EDITOR,

		SETTINGS,
		LUA_SCRIPTS,
		CONTEXT_MENU_SETTINGS,
		ESP_SETTINGS,
		GTA_CACHE_SETTINGS,
		GUI_SETTINGS,
		HOTKEY_SETTINGS,
		REACTION_SETTINGS,
		PROTECTION_SETTINGS,
		TRANSLATION_SETTINGS,
		PROXY_SETTINGS,
		DEBUG,

		PLAYER,

		// Added at runtime by things like lua scripts.
		RUNTIME_CUSTOM
	};

struct navigation_struct{char name[80]{};std::function<void()> func;std::map<tabs,navigation_struct> sub_nav;rage::joaat_t hash=0;};
struct GuiService{std::map<tabs,navigation_struct> nav;std::vector<tabs> selected;auto& get_navigation(){return nav;}auto& get_selected_tab(){return selected;}};inline GuiService service;inline auto* g_gui_service=&service;
inline int legacyCalls=0,teleports=0;inline Vector3 lastCoords;inline std::map<std::string,std::string> inputValues;inline std::vector<std::function<void()>> jobs;
struct FiberPool{void queue_job(std::function<void()> f){jobs.push_back(f);}};inline FiberPool pool;inline auto* g_fiber_pool=&pool;
struct command_arguments{command_arguments(std::vector<uint64_t>){}};struct command{static command* get(unsigned){static command cmd;return &cmd;}void call(command_arguments&){++legacyCalls;}};
struct player{int id()const{return 0;}bool is_valid()const{return true;}const char* get_name()const{return "Example Player";}};using player_ptr=std::shared_ptr<player>;
struct PlayerService{player_ptr current=std::make_shared<player>();std::map<int,player_ptr> list;const auto& players(){return list;}player_ptr get_self(){return current;}player_ptr get_selected(){return current;}void set_selected(player_ptr value){current=value;}};inline PlayerService players;inline auto* g_player_service=&players;
struct Lua{void draw_gui(unsigned){}};inline Lua lua;inline auto* g_lua_manager=&lua;
struct Notification{std::vector<std::string> messages;void push_warning(std::string,std::string value){messages.push_back(value);}};inline Notification g_notification_service;
struct Translation{std::string_view get_translation(unsigned){return {};}};inline Translation g_translation_service;
struct Settings{struct{bool switched_view=false;}window;} ;inline Settings g;
struct Folder{std::filesystem::path path;auto get_path()const{return path;}};struct FileManager{std::filesystem::path path;Folder get_project_folder(std::string){return {path};}};inline FileManager g_file_manager;
template<size_t N>struct Literal{char value[N];constexpr Literal(const char(&s)[N]){for(size_t i=0;i<N;++i)value[i]=s[i];}};
struct components{
 template<Literal S>static void command_checkbox(std::string_view){}
 template<Literal S>static void command_button(std::vector<uint64_t>,std::string_view){}
 static bool input_text_with_hint(std::string_view label,std::string_view,std::string& text){auto it=inputValues.find(std::string(label));if(it!=inputValues.end())text=it->second;return false;}
 static bool input_text_with_hint(std::string_view,std::string_view,char*,size_t){return false;}
};
namespace self{inline int ped=1,veh=2;}
namespace vehicle{inline void repair(int){}}
namespace mobile::mechanic{inline int get_personal_vehicle(){return 2;}}
struct persist_car_service{static void save_vehicle(int,std::string,std::string){}};
namespace teleport{inline void to_coords(Vector3 pos,Vector3={}){++teleports;lastCoords=pos;}}
struct telelocation{std::string name;float x=0,y=0,z=0,yaw=0,pitch=0,roll=0;};
struct CustomTeleport{std::map<std::string,std::vector<telelocation>> data;auto snapshot(){return data;}bool try_save_unique(std::string category,telelocation value){for(const auto& item:data)for(const auto& p:item.second)if(p.name==value.name)return false;data[category].push_back(value);return true;}bool delete_saved_location(std::string category,std::string name){auto it=data.find(category);if(it==data.end())return false;auto& places=it->second;places.erase(std::remove_if(places.begin(),places.end(),[&](const auto& value){return value.name==name;}),places.end());return true;}};inline CustomTeleport g_custom_teleport_service;
struct view{static void custom_teleport();static void teleport(){++legacyCalls;}static void view_player(){++legacyCalls;}};
namespace mini{inline void DrawBackground(ImDrawList*,ImVec2,ImVec2){}inline void ReloadBackground(){}inline const std::string& BackgroundStatus(){static std::string s="Mock GIF";return s;}}
namespace blip{struct B{Vector3 m_position;float m_rotation=0;};inline B* get_selected_blip(){return nullptr;}}
}
namespace ENTITY{inline Vector3 GET_ENTITY_COORDS(int,int){return {4,5,6};}inline float GET_ENTITY_HEADING(int){return 30;}inline Vector3 GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(int,float x,float y,float z){return {x,y,z};}}
namespace CAM{inline float GET_GAMEPLAY_CAM_RELATIVE_PITCH(){return 0;}inline float GET_GAMEPLAY_CAM_RELATIVE_HEADING(){return 0;}}
