#include "ClickTheme.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <system_error>
#include <stdexcept>
#include <utility>

namespace big::ClickUI
{
    static Theme s_Theme;
    static std::filesystem::path s_Folder;
    static std::string s_Status, s_Name = "Purple Glow";
    static bool s_Initialized = false;
    struct ColorField { const char* Name; ImVec4 Theme::* Member; };
    static constexpr ColorField colors[] = {
        {"Background", &Theme::Background}, {"Surface", &Theme::Surface},
        {"Accent", &Theme::Accent}, {"Selection", &Theme::Selection},
        {"Text", &Theme::Text}, {"Muted", &Theme::Muted},
        {"Border", &Theme::Border}, {"Header", &Theme::Header}, {"BackgroundLow", &Theme::BackgroundLow},
        {"Button", &Theme::Button}, {"ButtonHover", &Theme::ButtonHover}};
    struct FloatField { const char* Name; float Theme::* Member; float Min, Max; };
    static constexpr FloatField sizes[] = {
        {"Width", &Theme::Width, 560, 1280}, {"Height", &Theme::Height, 420, 1000},
        {"RowHeight", &Theme::RowHeight, 36, 64}, {"Rounding", &Theme::Rounding, 0, 16},
        {"PlayerWidth", &Theme::PlayerWidth, 160, 280}, {"Scanlines", &Theme::Scanlines, 0, 0.25f},
        {"LightIntensity", &Theme::LightIntensity, 0, 0.6f},
        {"BackgroundEnabled", &Theme::BackgroundEnabled, 0, 1},
        {"BackgroundOpacity", &Theme::BackgroundOpacity, 0, 1},
        {"BackgroundSpeed", &Theme::BackgroundSpeed, 0.1f, 3}};

    static bool ValidName(const std::string& name)
    {
        if (name.empty() || name.size() > 48 || name.front() == ' ' || name.back() == ' ') return false;
        for (unsigned char c : name)
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == ' ' || c == '-' || c == '_')) return false;
        std::string upper = name;
        std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c) {
            return c >= 'a' && c <= 'z' ? char(c - 'a' + 'A') : char(c);
        });
        if (upper == "CON" || upper == "PRN" || upper == "AUX" || upper == "NUL") return false;
        if (upper.size() == 4 && (upper.substr(0, 3) == "COM" || upper.substr(0, 3) == "LPT")
            && upper[3] >= '1' && upper[3] <= '9') return false;
        return true;
    }

    static void Validate(const Theme& theme)
    {
        for (const auto& field : colors)
        {
            const auto c = theme.*(field.Member);
            for (float v : {c.x, c.y, c.z, c.w})
                if (!std::isfinite(v) || v < 0 || v > 1) throw std::runtime_error("Colors must be between 0 and 1.");
        }
        for (const auto& field : sizes)
        {
            const float v = theme.*(field.Member);
            if (!std::isfinite(v) || v < field.Min || v > field.Max)
                throw std::runtime_error("Theme dimensions are outside the supported range.");
        }
    }

    static void Write(const std::filesystem::path& path, const Theme& theme)
    {
        Validate(theme);
        std::ofstream file(path, std::ios::trunc);
        file.imbue(std::locale::classic());
        if (!file) throw std::runtime_error("Cannot write theme file.");
        file << "# YimMenuV2 click theme v1; RGBA values are 0..1\n" << std::setprecision(9);
        for (const auto& field : colors)
        {
            const auto c = theme.*(field.Member);
            file << field.Name << ' ' << c.x << ' ' << c.y << ' ' << c.z << ' ' << c.w << '\n';
        }
        for (const auto& field : sizes) file << field.Name << ' ' << theme.*(field.Member) << '\n';
        file.flush();
        if (!file) throw std::runtime_error("Writing the theme failed.");
    }

    static Theme Read(const std::filesystem::path& path)
    {
        std::ifstream file(path);
        if (!file) throw std::runtime_error("Cannot open theme file.");
        if (std::filesystem::file_size(path) > 16384) throw std::runtime_error("Theme file is too large.");
        Theme result; // Missing keys inherit the built-in default, never stale previous values.
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream input(line);
            input.imbue(std::locale::classic());
            std::string key;
            if (!(input >> key) || key[0] == '#') continue;
            bool found = false;
            for (const auto& field : colors) if (key == field.Name)
            {
                auto& c = result.*(field.Member);
                if (!(input >> c.x >> c.y >> c.z >> c.w)) throw std::runtime_error("Invalid RGBA entry: " + key);
                found = true;
            }
            for (const auto& field : sizes) if (key == field.Name)
            {
                if (!(input >> result.*(field.Member))) throw std::runtime_error("Invalid dimension: " + key);
                found = true;
            }
            std::string extra;
            if (!found || (input >> extra)) throw std::runtime_error("Unknown key or extra values: " + key);
        }
        if (file.bad()) throw std::runtime_error("Reading the theme failed.");
        Validate(result);
        return result;
    }

    Theme& CurrentTheme() { return s_Theme; }
    const std::filesystem::path& ThemeFolder() { return s_Folder; }
    const std::string& ThemeStatus() { return s_Status; }
    const std::string& ActiveThemeName() { return s_Name; }

    bool LoadTheme(const std::string& name)
    {
        try
        {
            if (s_Folder.empty() || !ValidName(name)) throw std::runtime_error("Invalid theme name or unavailable folder.");
            const auto candidate = Read(s_Folder / (name + ".theme"));
            std::ofstream active(s_Folder / "active.txt", std::ios::trunc);
            active << name << '\n'; active.flush();
            if (!active) throw std::runtime_error("Cannot persist active theme selection.");
            s_Theme = candidate; s_Name = name; s_Status = "Loaded: " + name;
            return true;
        }
        catch (const std::exception& e) { s_Status = e.what(); return false; }
    }

    bool SaveTheme(const std::string& name)
    {
        try
        {
            if (s_Folder.empty() || !ValidName(name)) throw std::runtime_error("Use 1-48 letters, digits, spaces, - or _ for the name.");
            Write(s_Folder / (name + ".theme"), s_Theme);
            return LoadTheme(name);
        }
        catch (const std::exception& e) { s_Status = e.what(); return false; }
    }

    std::vector<std::string> ThemeNames()
    {
        std::vector<std::string> result;
        try
        {
            if (s_Folder.empty()) return result;
            for (const auto& entry : std::filesystem::directory_iterator(s_Folder))
                if (entry.is_regular_file() && entry.path().extension() == ".theme" && ValidName(entry.path().stem().string()))
                    result.push_back(entry.path().stem().string());
            std::sort(result.begin(), result.end());
        }
        catch (const std::exception& e) { s_Status = e.what(); }
        return result;
    }

    void InitializeThemes(const std::filesystem::path& folder)
    {
        if (s_Initialized) return;
        try
        {
            std::filesystem::create_directories(folder);
            s_Folder = folder;
            Theme purple;
            Theme red; red.Accent = ImVec4(1, 0.14f, 0.18f, 1); red.Selection = ImVec4(0.48f, 0.02f, 0.03f, 1);
            red.BackgroundLow = ImVec4(0.55f, 0, 0.025f, 1); red.Button = ImVec4(0.45f, 0.03f, 0.045f, 1);
            red.ButtonHover = ImVec4(0.7f, 0.04f, 0.055f, 1); red.Border = ImVec4(0.7f, 0.07f, 0.10f, 1);
            Theme mono; mono.Accent = ImVec4(0.95f, 0.95f, 0.95f, 1); mono.Selection = ImVec4(0.25f, 0.25f, 0.25f, 1);
            mono.BackgroundLow = ImVec4(0.12f, 0.12f, 0.12f, 1); mono.Button = ImVec4(0.29f, 0.29f, 0.29f, 1);
            mono.ButtonHover = ImVec4(0.42f, 0.42f, 0.42f, 1); mono.Border = ImVec4(0.4f, 0.4f, 0.4f, 1);
            for (const auto& preset : {std::pair{"Purple Glow", purple}, std::pair{"Red Glow", red}, std::pair{"Monochrome", mono}})
            {
                const auto path = folder / (std::string(preset.first) + ".theme");
                if (!std::filesystem::exists(path)) Write(path, preset.second);
            }
            Theme compact;
            compact.Accent=ImVec4(220.0f/255,45.0f/255,45.0f/255,1);
            compact.Background=ImVec4(20.0f/255,20.0f/255,23.0f/255,240.0f/255);
            compact.BackgroundLow=ImVec4(70.0f/255,25.0f/255,25.0f/255,210.0f/255);
            compact.Surface=ImVec4(0,0,0,0.78f); compact.Button=ImVec4(.09f,.09f,.09f,1);
            compact.ButtonHover=ImVec4(.86f,.176f,.196f,.59f);
            compact.Border=ImVec4(.86f,.176f,.196f,.71f); compact.Header=ImVec4(.396f,.02f,.02f,.72f);
            compact.Selection=ImVec4(.86f,.176f,.196f,.39f);
            compact.Text=ImVec4(.941f,.941f,.941f,1); compact.Muted=ImVec4(.65f,.65f,.65f,1);
            compact.Width=720; compact.Height=560; compact.Rounding=5; compact.BackgroundEnabled=1; compact.Scanlines=0;
            const auto compactFile=folder / "Rockstar Compact.theme";
            if (!std::filesystem::exists(compactFile)) Write(compactFile,compact);
            Theme mini=compact; mini.Width=640; mini.Height=500; mini.Rounding=8; mini.PlayerWidth=160;
            mini.BackgroundOpacity=0.25f; mini.LightIntensity=0.18f;
            const auto miniFile=folder / "Rockstar Mini.theme";
            if (!std::filesystem::exists(miniFile)) Write(miniFile,mini);
            std::string selected = "Rockstar Mini";
            std::ifstream active(folder / "active.txt");
            if (active) std::getline(active, selected);
            s_Initialized = true;
            // If an edited preset is invalid, keep the in-memory safe default and report the error.
            LoadTheme(selected);
        }
        catch (const std::exception& e) { s_Status = e.what(); }
    }
}
