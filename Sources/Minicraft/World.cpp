#include "pch.h"

#include "World.h"
#include "Minicraft/PerlinNoise.h"

void World::Generate(DeviceResources* deviceRes) {


	for(int i = 0 ; i < WORLD_SIZE ; i++) {
		chunks.emplace_back();
		for (int j = 0; j < WORLD_SIZE; j++) {
			chunks[i].emplace_back();
			for (int k = 0; k < WORLD_SIZE; k++) {
				chunks[i][j].emplace_back(Vector3{(float)(i*CHUNK_SIZE), (float)(j*CHUNK_SIZE), (float)(k*CHUNK_SIZE)}, j == WORLD_SIZE-1);
			}
		}
	}
	
	for (int i = 0; i < WORLD_SIZE; i++) {
		for (int j = 0; j < WORLD_SIZE; j++) {
			for (int k = 0; k < WORLD_SIZE; k++) {
				chunks[i][j][k].xPos = GetChunk(i + 1, j, k);
				chunks[i][j][k].xNeg = GetChunk(i - 1, j, k);
				chunks[i][j][k].yPos = GetChunk(i, j + 1, k);
				chunks[i][j][k].yNeg = GetChunk(i, j - 1, k);
				chunks[i][j][k].zPos = GetChunk(i, j, k + 1);
				chunks[i][j][k].zNeg = GetChunk(i, j, k - 1);
				chunks[i][j][k].Generate(deviceRes);
			}
		}
	}

	constantBufferModel.Create(deviceRes);
}

void World::Draw(DeviceResources* deviceRes) {
	constantBufferModel.ApplyToVS(deviceRes, 0);
	for(auto rect: chunks) {
		for(auto line : rect) {
			for (auto cube : line) {
				constantBufferModel.data.model = cube.model.Transpose();
				constantBufferModel.UpdateBuffer(deviceRes);

				cube.Draw(deviceRes);
			}
		}
	}
}

Chunk* World::GetChunk(int x, int y, int z)
{
	if (chunks.size() == 0 || chunks[0].size() == 0 || chunks[0][0].size() == 0 || 
		x < 0 || x >= chunks.size() ||
		y < 0 || y >= chunks[0].size() ||
		z < 0 || z >= chunks[0][0].size())
		return nullptr;

	return &chunks[x][y][z];
}
