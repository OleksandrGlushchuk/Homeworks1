#pragma once
#include <cstdint>
struct TriangleIndices
{
	TriangleIndices(){}
	TriangleIndices(uint32_t i0, uint32_t i1, uint32_t i2)
	{
		indices[0] = i0; indices[1] = i1; indices[2] = i2;
	}
	uint32_t indices[3];
};