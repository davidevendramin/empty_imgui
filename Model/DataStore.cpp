#include "DataStore.h"

DataStore::DataStore() {
	// fill this->dummyData with some example data
	for (int i = 0; i < 100; i++) {
		DataPoint_t point;
		point.time = static_cast<float>(i);
		point.value = static_cast<float>(std::sin(i * 0.1f) * 50.0f + 50.0f); // Example: sine wave data
		this->dummyData.push_back(point);
	}
}

DataStore::~DataStore() {
}

std::vector<DataPoint_t> DataStore::getData() {
	// Implementation for data retrieval goes here
	return this->dummyData;
}