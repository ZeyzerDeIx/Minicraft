#pragma once

#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/Block.h"
#include "PerlinNoise.hpp"

class World;

namespace SIDE {
	constexpr uint8_t NONE = 0b00000000;
	constexpr uint8_t XPOS = 0b00000001;
	constexpr uint8_t XNEG = 0b00000010;
	constexpr uint8_t YPOS = 0b00000100;
	constexpr uint8_t YNEG = 0b00001000;
	constexpr uint8_t ZPOS = 0b00010000;
	constexpr uint8_t ZNEG = 0b00100000;
	constexpr uint8_t ALL  = 0b00111111;
};

constexpr int BLOCK_PER_CHUNK = 16;
constexpr int CHUNK_SIZE = BLOCK_PER_CHUNK;

struct CubeData
{
	BlockId blockId;
	uint8_t visibleSides;
};

class Chunk {
	VertexBuffer<VertexLayout_PositionUV> vb;
	IndexBuffer ib;

	Vector3 size;

	Chunk* xPos;
	Chunk* xNeg;
	Chunk* yPos;
	Chunk* yNeg;
	Chunk* zPos;
	Chunk* zNeg;

public:
	Matrix model;
	std::vector<std::vector<std::vector<CubeData>>> cubesData;

	Chunk(Vector3 pos,
		  Vector2 coord,
		  siv::PerlinNoise& perlin,
		  bool isOnTop = false);

	void Generate(DeviceResources* deviceRes);
	void Draw(DeviceResources* deviceRes);

private:
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, int id);
	void PushCube(Vector3 pos, CubeData& cubeData);
	BlockId GetCubeId(int x, int y, int z);

	friend World;
};