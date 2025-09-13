#pragma once
#include "pch.h"

typedef struct DataPoint {
	float value;
	float time;
} DataPoint_t;

class DataStore {
private:
	std::vector<DataPoint_t> dummyData;
public:
	DataStore();
	~DataStore();
	std::vector<DataPoint_t> getData();
};