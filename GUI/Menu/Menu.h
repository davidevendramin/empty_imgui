#pragma once
#include "pch.h"

class GUI;
class Controller;

class Menu {
private:
	GUI* gui;
	Controller* controller;

public:
	Menu();
	Menu(Controller* controller, GUI* gui);
	~Menu();
	void render();
};