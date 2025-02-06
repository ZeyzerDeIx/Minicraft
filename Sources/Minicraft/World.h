#pragma once

#include "Minicraft/Block.h"
#include "Minicraft/Chunk.h"

constexpr auto WORLD_SIZE = 3;

class World {
	BlockId data[WORLD_SIZE * WORLD_SIZE * WORLD_SIZE];
	std::vector<std::vector<std::vector<Chunk>>> chunks;

	struct ModelData {
		Matrix model;
	};
	ConstantBuffer<ModelData> constantBufferModel;
public:
	void Generate(DeviceResources* deviceRes);
	void Draw(DeviceResources* deviceRes);

private:
	Chunk* GetChunk(int x, int y, int z);
};