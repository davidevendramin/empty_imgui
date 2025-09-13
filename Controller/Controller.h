#pragma once
#include "pch.h"
#include "DataStore.h"

class Controller {
private:
	DataStore* dataStore;
public:
	Controller();
	~Controller();
	std::vector<DataPoint_t> getData();

	Controller& operator=(Controller&& other) noexcept {
		if (this != &other) {
			this->dataStore = std::move(other.dataStore);
		}
		return *this;
	}

};