#include "Controller.h"

Controller::Controller() {
	this->dataStore = new DataStore();
}

Controller::~Controller() {
	delete this->dataStore;
}

std::vector<DataPoint_t> Controller::getData() {
	return this->dataStore->getData();
}