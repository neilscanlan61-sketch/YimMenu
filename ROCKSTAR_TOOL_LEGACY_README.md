# Rockstar Tool Mini — Legacy UI port

Ported to the uploaded YimMenu-master Legacy source, NOT YimMenuV2.
The main window uses the previous Mini visual design: 640 x 500, ROCKSTAR TOOL header,
mouse-click icon rail, rounded controls, dark/red cards, static accent lights and animated GIF.
This is a source ZIP, not a compiled DLL, and not a guarantee of pixel-identical feature pages.
Legacy has a different navigation tree, feature set and D3D11 renderer.

## Included

- Replaced the old separate heading/navigation/main/player windows with one compact Mini shell.
- Same Mini .theme format, custom-name loader, refresh button and animated-GIF switch.
- Theme/Favorites buttons in the header; section search, persistent favorite sections and player search.
- Legacy-registered feature callbacks and Lua views remain reachable through tabs/search.
- Vehicle Main uses Tools / Globals / Misc cards. Legacy Controls keeps the complete original vehicle page.
- Teleport: Quick Travel, Directional, Saved. Legacy Extras preserves original coordinate/IPL/vehicle controls.
- Saved: category-scoped search, explicit/double-click teleport, current-location or selected-blip save,
  captured fiber inputs and category-specific delete confirmation. Original telelocations.json format retained.
- Snapshot reads and guarded service writes; corrected the old dangling-pointer lookup.
  The legacy public map/pointer API is retained for compatibility and is not safe to hold across mutations.
- Phone up/down and mouse scrolling reserved in all three PAD groups while the menu is open.
- WIC GIF decoding adapted to native D3D11 textures, including transparency/disposal, timing,
  texture reset and paused playback while the menu is closed. This is NOT a copied D3D12 renderer.
- Supplied Rockstar Mini / Rojo Mini .theme + .gif pairs; previous colors, fonts and layout reused.

## Important feature differences

This is a UI port, not a port of YimMenuV2 game offsets or every V2 command.
This Legacy source does not contain the same savepersonalvehicle command. Vehicle Main therefore
exposes **Save Current Vehicle (Local)** using its existing local JSON persistence service;
it is clearly labeled and is NOT an online personal-vehicle conversion.
No Free Shopping implementation or fake toggle was added. No new Savage camera guard was added.
Existing Legacy protection, session and startup checks were left intact.
The uploaded source's existing FSL requirement and game-version compatibility still apply.
This update does not remove those requirements or establish compatibility with current game builds.

## Use / build

1. Use this complete Legacy source tree in place of the uploaded Legacy tree.
   Do not apply the earlier YimMenuV2 patches to it.
2. Build using its existing Windows/MSVC prerequisites or its existing GitHub workflow.
   CMake target and DLL output remain **YimMenu** / **YimMenu.dll** for release-workflow compatibility.
   The visible main UI branding is **ROCKSTAR TOOL**.
3. Copy docs/rockstar-mini/Rockstar Mini.theme and Rockstar Mini.gif (and/or Rojo Mini pair) into:
   `%APPDATA%\YimMenu\themes\click-ui\`
4. Open the menu using your existing hotkey, click the paintbrush, Refresh themes / GIF,
   then choose Rockstar Mini and enable Animated GIF background.
5. For another theme, copy My Theme.theme and My Theme.gif to that folder,
   enter My Theme without an extension, and click Load custom theme.

Matching active-theme-name.gif is tried first, then background.gif. Missing/invalid GIFs use a
color background with a visible status message. Main window only; toast GIFs are not imported.
Limits: 16 MiB compressed, 1024 x 1024, 180 frames, 64 MiB decoded RGBA.
Edit .theme files for advanced dimensions/colors/LightIntensity. LightIntensity=0 disables lights.
Your old Legacy settings, Lua API and data directory remain unchanged; the new theme controls
replace the old visual color/size editors, while overlay options remain accessible.

## Validation

PASS: real MiniUI.cpp, MiniTravel.cpp, view_custom_teleport.cpp against a fake game/ImGui,
including routing/callbacks, wide/narrow/clipped windows, stack/style restoration, favorites,
Mini theme initialization, copied deferred save/teleport/delete actions and category filtering.
PASS: portable GIF timing/compositing and theme parser; source patch/ZIP byte checks.
Illustrative HTML previews are design references, not compiled Legacy screenshots.

NOT TESTED: complete MSVC/Windows build, WIC decode on Windows, D3D11 uploads,
real JSON disk persistence, GTA behavior or the uploaded Legacy build's game compatibility.
Build and test before distributing. No compiled DLL or claim of in-game validation is included.
