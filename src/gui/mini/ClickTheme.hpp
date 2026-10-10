#pragma once
#include <imgui.h>
#include <filesystem>
#include <string>
#include <vector>

namespace big::ClickUI
{
    struct Theme
    {
        ImVec4 Background{0.025f, 0.02f, 0.035f, 0.98f};
        ImVec4 Surface{0.025f, 0.02f, 0.04f, 0.96f};
        ImVec4 Accent{0.90f, 0.15f, 0.85f, 1.0f};
        ImVec4 Selection{0.42f, 0.01f, 0.65f, 1.0f};
        ImVec4 Text{0.95f, 0.94f, 0.98f, 1.0f};
        ImVec4 Muted{0.69f, 0.67f, 0.75f, 1.0f};
        ImVec4 Border{0.59f, 0.12f, 0.74f, 1.0f};
        ImVec4 Header{0.055f, 0.02f, 0.07f, 1.0f};
        ImVec4 BackgroundLow{0.53f, 0.0f, 0.90f, 1.0f};
        ImVec4 Button{0.55f, 0.05f, 0.48f, 1.0f};
        ImVec4 ButtonHover{0.75f, 0.12f, 0.65f, 1.0f};
        float Scanlines = 0.025f;
        float BackgroundEnabled = 0.0f;
        float BackgroundOpacity = 0.35f;
        float BackgroundSpeed = 1.0f;
        float PlayerWidth = 180.0f;
        float Width = 640.0f;
        float Height = 500.0f;
        float RowHeight = 44.0f;
        float Rounding = 8.0f;
        float LightIntensity = 0.18f;
    };
    Theme& CurrentTheme();
    // Initialize once from the project's configured data directory, not the DLL directory.
    void InitializeThemes(const std::filesystem::path& folder);
    const std::filesystem::path& ThemeFolder();
    std::vector<std::string> ThemeNames();
    bool LoadTheme(const std::string& name);
    bool SaveTheme(const std::string& name);
    const std::string& ThemeStatus();
    const std::string& ActiveThemeName();
    void DrawThemeEditor();
}
