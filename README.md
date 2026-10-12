# Compact violet UI — first-pass source patch

This is a visual-only modification of the YimMenu-nightly source archive you supplied. It is inspired by your Cherax screenshot, not an exact Cherax replica. It does not include a compiled DLL or the complete original repository.

## Changes

- Near-black purple panels, violet borders and a subtle luminous top edge.
- Rounded windows, child panels, buttons, slider grabs and scrollbars.
- Narrower navigation, smaller fonts and a default UI scale of 0.90.
- Fixed compact content pane with vertical/horizontal scrolling for existing wide screens.
- Pill-switch rendering for shared command-checkbox controls. Ordinary direct ImGui checkboxes remain square and receive the new colors.
- Navigation rows use available panel width and local indentation instead of hardcoded screen-space highlights.
- Player-list positioning and widths updated to avoid overlapping the navigation panel at typical desktop resolutions.
- A preset button in GUI Settings lets existing saved profiles adopt the new colors and scale without clearing other configuration.

## Apply

1. Back up your original source folder.
2. Copy the CONTENTS of `replacement-files` into your original source folder, keeping the directory structure. Replace the matching files and include the new `src/gui/compact_theme.hpp`.
3. Alternatively, from the original project root use `git apply /path/to/violet-ui.patch`. Do not do both methods.
4. Compile using your existing Windows/MSVC project setup. This package does not change build scripts or dependencies.
5. If your old saved colors remain, open Settings > GUI Settings and select `Apply compact violet preset`.

At scale 0.90, the navigation is 180 pixels wide and the content pane is 576 by 360 pixels. The content region may be smaller on limited displays. The sidebar includes nested tabs and scrolls when necessary. The separate player list can extend beneath the main menu. This is not the exact multi-column Self-page arrangement from the screenshot.

## Scope and verification

Reviewed the relevant theme, shared widgets, navigation, window layout, font-size and settings code; not every source file. No changes to feature callbacks, game hooks, network implementations, injection, or anti-cheat behavior. Original licensing applies to the modified files.

Verified the patch applies cleanly to the uploaded archive and that files outside the listed UI changes remain byte-for-byte untouched. This is NOT a Windows compilation or in-game runtime test. The environment does not have the required MSVC/Windows graphics setup, and downloading Dear ImGui for an isolated renderer test failed due to DNS access. No actual in-game visual QA has been performed. Treat these changes as an uncompiled first pass and review/test them in your authorized local environment.

Some original pages were designed for a wider menu. They scroll rather than having their underlying feature layout rewritten. Lua-owned windows retain their independent layouts. This patch does not copy Cherax logos, fonts, proprietary assets, or features.

`manifest.json` records byte-level hashes for original and replacement files. The new header has a null original hash.
