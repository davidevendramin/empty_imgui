#include "Widget.h"
#include "Controller.h"
#include "GUI.h"

Widget::Widget()
{
	this->controller = nullptr;
	this->gui = nullptr;
	this->widgetID = -1;
	this->isOpen = true;
	this->lastFrameDocked = false;
}

Widget::Widget(Controller* controller, GUI* gui)
{
	this->controller = controller;
	this->gui = gui;
	this->widgetID = (int)gui->getWidgetIndex();
	this->isOpen = true;
	this->lastFrameDocked = false;
}

Widget::~Widget()
{
}

void Widget::render() {
	// This function should be overridden by derived classes
}

void Widget::setWidgetID(int id) {
	this->widgetID = id;
}

int Widget::getWidgetID() {
	return this->widgetID;
}

void Widget::setOpen(bool open) {
	this->isOpen = open;
}

bool Widget::getIsOpen() {
	return this->isOpen;
}