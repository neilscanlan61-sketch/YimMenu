#pragma once
#include <imgui.h>
#include <string>
namespace big::mini {
void ResetBackground(); void ReloadBackground(); void TickBackground(bool playing);
void DrawBackground(ImDrawList* draw,ImVec2 pos,ImVec2 size);
const std::string& BackgroundStatus();
}
