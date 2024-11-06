#include <imguiStuff.h>
#include <IconsForkAwesome.h>


bool drawImageButtonWithLabelAndCog(ImTextureID image, 
	const char *label, const ImVec2 &imageSize, 
	bool &pressedCog, bool &pressedX,
	const ImVec2 &cogSize, 
	 const ImVec2 &padding)
{
	bool buttonClicked = false;
	pressedCog = false;
	pressedX = false;

	ImGui::BeginGroup();

	auto imageStart = ImGui::GetCursorPos();

	// Draw Image Button
	if (ImGui::ImageButton(image, imageSize, {0,1}, {1,0}))
	{
		buttonClicked = true;
	}

	auto imageEnd = ImGui::GetCursorPos();

	// Label with cog icon
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - padding.y); // Adjusts the padding between image and label
	ImGui::Text("%s", label);
	ImGui::SameLine();

	// Adjust cog icon position to align with the label
	ImGui::SetCursorPosX(imageEnd.x + padding.x + imageSize.x);
	ImGui::SetCursorPosY(imageStart.y);
	if (ImGui::Button(ICON_FK_COG, cogSize))
	{
		pressedCog = true;
	}

	ImGui::SetCursorPosX(imageEnd.x + padding.x + imageSize.x);
	ImGui::SetCursorPosY(imageStart.y + cogSize.x + 10);
	if (ImGui::Button(ICON_FK_TIMES_CIRCLE_O, cogSize))
	{
		pressedX = true;
	}

	ImGui::EndGroup();

	return buttonClicked;
}