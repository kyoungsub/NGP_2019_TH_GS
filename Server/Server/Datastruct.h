#pragma once
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <random>



#pragma pack(1)
struct InitData {
	float mass;
	float sizeX, sizeY, sizeZ;
	float coef_Frict;
};
#pragma pack()