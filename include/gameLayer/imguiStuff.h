#pragma once

#include "imgui.h"

bool drawImageButtonWithLabelAndCog(ImTextureID image, const char *label,
	const ImVec2 &imageSize, 
	bool &pressedCog, bool &pressedX,
	const ImVec2 &cogSize = {60, 60},
	const ImVec2 &padding = ImVec2(10, 5));