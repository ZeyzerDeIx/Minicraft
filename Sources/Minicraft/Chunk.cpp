#include "pch.h"

#include "Chunk.h"

Chunk::Chunk(Vector3 pos, bool isOnTop, Chunk* xPos, Chunk* xNeg, Chunk* yPos, Chunk* yNeg, Chunk* zPos, Chunk* zNeg):
	model(Matrix::CreateTranslation(pos)),
	size(Vector3{BLOCK_PER_CHUNK, BLOCK_PER_CHUNK , BLOCK_PER_CHUNK }),
	xPos(xPos),
	xNeg(xNeg),
	yPos(yPos),
	yNeg(yNeg),
	zPos(zPos),
	zNeg(zNeg)
{
	for (int i = 0; i < size.x; i++) {
		cubesData.emplace_back();
		for (int j = 0; j < size.x; j++) {
			cubesData[i].emplace_back();
			for (int k = 0; k < size.x; k++) {
				cubesData[i][j].push_back({ j == size.x - 1 && isOnTop ? GRASS : DIRT, SIDE::ALL });
			}
		}
	}
}

void Chunk::Generate(DeviceResources* deviceRes) {

	//faces connection
	for (int i = 0; i < size.x; i++) {
		for (int j = 0; j < size.x; j++) {
			for (int k = 0; k < size.x; k++) {
				cubesData[i][j][k].visibleSides ^= SIDE::XPOS * (GetCubeId(i + 1, j, k) != EMPTY);
				cubesData[i][j][k].visibleSides ^= SIDE::XNEG * (GetCubeId(i - 1, j, k) != EMPTY);
				cubesData[i][j][k].visibleSides ^= SIDE::YPOS * (GetCubeId(i, j + 1, k) != EMPTY);
				cubesData[i][j][k].visibleSides ^= SIDE::YNEG * (GetCubeId(i, j - 1, k) != EMPTY);
				cubesData[i][j][k].visibleSides ^= SIDE::ZPOS * (GetCubeId(i, j, k + 1) != EMPTY);
				cubesData[i][j][k].visibleSides ^= SIDE::ZNEG * (GetCubeId(i, j, k - 1) != EMPTY);
			}
		}
	}

	for (int i = 0; i < size.x; i++)
		for (int j = 0; j < size.x; j++)
			for (int k = 0; k < size.x; k++)
				PushCube({ (float)i,(float)j,(float)k }, cubesData[i][j][k]);

	if (vb.data.size() != 0)
	{
		vb.Create(deviceRes);
		ib.Create(deviceRes);
	}
}

void Chunk::Draw(DeviceResources* deviceRes) {
	vb.Apply(deviceRes, 0);
	ib.Apply(deviceRes);
	deviceRes->GetD3DDeviceContext()->DrawIndexed(ib.Size(), 0, 0);
}

Vector4 ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}


void Chunk::PushFace(Vector3 pos, Vector3 up, Vector3 right, int id) {
	Vector2 uv(
		(id % 16) * BLOCK_TEXSIZE,
		(id / 16) * BLOCK_TEXSIZE
	);

	auto a = vb.PushVertex({ ToVec4(pos), uv + Vector2::UnitY * BLOCK_TEXSIZE });
	auto b = vb.PushVertex({ ToVec4(pos + up), uv });
	auto c = vb.PushVertex({ ToVec4(pos + right), uv + Vector2::UnitX * BLOCK_TEXSIZE + Vector2::UnitY * BLOCK_TEXSIZE });
	auto d = vb.PushVertex({ ToVec4(pos + up + right), uv + Vector2::UnitX * BLOCK_TEXSIZE });
	ib.PushTriangle(a, b, c);
	ib.PushTriangle(c, b, d);
}

void Chunk::PushCube(Vector3 pos, CubeData cubeData)
{
	auto& data = BlockData::Get(cubeData.blockId);
	float uvxSide = (data.texIdSide % 16) / BLOCK_TEXSIZE;
	float uvySide = (data.texIdSide / 16) / BLOCK_TEXSIZE;

	if(cubeData.visibleSides & SIDE::ZPOS)
		PushFace(pos + Vector3{ -0.5f, -0.5f, 0.5f },
				Vector3::Up, Vector3::Right, data.texIdSide);

	if(cubeData.visibleSides & SIDE::XPOS)
		PushFace(pos + Vector3{ 0.5f, -0.5f, 0.5f },
				Vector3::Up, Vector3::Forward, data.texIdSide);

	if (cubeData.visibleSides & SIDE::ZNEG)
		PushFace(pos + Vector3{ 0.5f, -0.5f, -0.5f },
				Vector3::Up, Vector3::Left, data.texIdSide);

	if (cubeData.visibleSides & SIDE::XNEG)
		PushFace(pos + Vector3{ -0.5f, -0.5f, -0.5f },
				Vector3::Up, Vector3::Backward, data.texIdSide);

	if (cubeData.visibleSides & SIDE::YPOS)
		PushFace(pos + Vector3{ -0.5f, 0.5f, 0.5f },
				Vector3::Forward, Vector3::Right, data.texIdTop);

	if (cubeData.visibleSides & SIDE::YNEG)
		PushFace(pos + Vector3{ -0.5f, -0.5f,-0.5f },
				Vector3::Backward, Vector3::Right, data.texIdBottom);
}

BlockId Chunk::GetCubeId(int x, int y, int z)
{
	if(cubesData.size() == 0 || cubesData[0].size() == 0 || cubesData[0][0].size() == 0)
		return EMPTY;

    if (x < 0) {
		if (xNeg) return xNeg->GetCubeId(x + BLOCK_PER_CHUNK, y, z);
		else return EMPTY;
    }
    if (x >= cubesData.size()) {
		if (xPos) return xPos->GetCubeId(x - BLOCK_PER_CHUNK, y, z);
		else return EMPTY;
    }
    if (y < 0) {
		if (yNeg) return yNeg->GetCubeId(x, y + BLOCK_PER_CHUNK, z);
		else return EMPTY;
    }
    if (y >= cubesData[0].size()) {
		if (yPos) return yPos->GetCubeId(x, y - BLOCK_PER_CHUNK, z);
		else return EMPTY;
    }
    if (z < 0) {
		if (zNeg) return zNeg->GetCubeId(x, y, z + BLOCK_PER_CHUNK);
		else return EMPTY;
    }
    if (z >= cubesData[0][0].size()) {
		if (zPos) return zPos->GetCubeId(x, y, z - BLOCK_PER_CHUNK);
		else return EMPTY;
    }

	return cubesData[x][y][z].blockId;
}
