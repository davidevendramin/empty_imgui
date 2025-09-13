#include "ExampleWidget.h"
#include "Controller.h"

ExampleWidget::ExampleWidget(Controller* controller, GUI* gui) : Widget(controller, gui) {
}

ExampleWidget::~ExampleWidget() {
}

void ExampleWidget::render() {
	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
	std::string title = "Example Widget##" + std::to_string(this->widgetID);
	if (ImGui::Begin(title.c_str(), &this->isOpen, ImGuiWindowFlags_NoCollapse)) {
		std::vector<DataPoint_t> data = this->controller->getData();
		// plot data with ImPlot
		if (ImPlot::BeginPlot("Example Plot", ImVec2(-1, -1))) {
			std::vector<float> x(data.size());
			std::vector<float> y(data.size());
			for (size_t i = 0; i < data.size(); i++) {
				x[i] = data[i].time;
				y[i] = data[i].value;
			}
			ImPlot::PlotLine("Data", x.data(), y.data(), (int)x.size());

			ImPlot::EndPlot();
		}

		ImGui::End();
		return;
	}
	ImGui::End();
}