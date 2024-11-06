#include <imguiStuff.h>
#include <IconsForkAwesome.h>


bool drawImageButtonWithLabelAndCog(ImTextureID image, 
	const char *label, const ImVec2 &imageSize, const ImVec2 &cogSize, 
	 const ImVec2 &padding)
{
	bool buttonClicked = false;
	bool cogClicked = false;

	ImGui::BeginGroup();

	auto imageStart = ImGui::GetCursorPos();

	// Draw Image Button
	if (ImGui::ImageButton(image, imageSize))
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
		cogClicked = true;
	}

	ImGui::EndGroup();

	return buttonClicked;
}