#pragma once
#include "DataTypesEnum.h"

class FlopsMesurement{
public:
    double runTest(DataTypes type, TestType testType);
    const char *getProgramSource(DataTypes type);
    void initMap();
    std::map<DataTypes, std::string> dataMap;
};
