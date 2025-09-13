#pragma once
#include "pch.h"
#include "Widget.h"

class ExampleWidget : public Widget {
public:
	ExampleWidget(Controller* controller, GUI* gui);
	~ExampleWidget();
	void render() override;
};