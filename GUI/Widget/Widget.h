#pragma once
#include "pch.h"

class Controller;
class GUI;

class Widget {
protected:
	Controller* controller;
	GUI* gui;
	int widgetID;
	bool isOpen;
	bool lastFrameDocked;

public:
	Widget();
	Widget(Controller* controller, GUI* gui);
	virtual ~Widget();
	virtual void render();
	void setWidgetID(int id);
	int getWidgetID();
	void setOpen(bool open);
	bool getIsOpen();

};