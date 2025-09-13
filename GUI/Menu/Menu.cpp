#include "Menu.h"
#include "GUI.h"
#include "Controller.h"
#include "Widget.h"
#include "ExampleWidget.h"

Menu::Menu() {
	this->gui = nullptr;
	this->controller = nullptr;
}

Menu::Menu(Controller* controller, GUI* gui) {
	this->gui = gui;
	this->controller = controller;
}

Menu::~Menu() {
}

void Menu::render() {
	ImVec2 currentCursorPos;
	ImVec2 windowSize = ImGui::GetMainViewport()->Size;
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 menuButtonColor = style.Colors[ImGuiCol_HeaderHovered];
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;

	ImVec2 window_pos;
	window_pos.x = work_pos.x + work_size.x * 0.5f;
	window_pos.y = work_pos.y + work_size.y * 0.5f;

	ImVec2 window_pos_pivot;
	window_pos_pivot.x = 0.5f;
	window_pos_pivot.y = 0.5f;
	float menuBarHeight = ImGui::GetFrameHeight() + 8.0f;
	bool newColorLayout = false;

	auto menuPadding = style.FramePadding.y;
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Add Widgets")) {
		if (ImGui::MenuItem("Example Widget")) {
			this->gui->addWidget(new ExampleWidget(this->controller, this->gui));
		}

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

}