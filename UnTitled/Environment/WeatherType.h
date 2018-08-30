#pragma once
#include "stdafx.h"

struct WeatherCube
{
	D3DXVECTOR3 Origin = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 Size = D3DXVECTOR3(10, 10, 10);

	WeatherCube(D3DXVECTOR3& origin, D3DXVECTOR3& size)
		: Origin(origin), Size(size)
	{

	}
};

struct VertexRain
{
	D3DXVECTOR3 Position;
	D3DXVECTOR2 Uv;
	D3DXVECTOR2 Scale;

	VertexRain() {}

	VertexRain
	(
		D3DXVECTOR3& Position,
		D3DXVECTOR2& Uv,
		D3DXVECTOR2& Scale
	)
		: Position(Position)
		, Uv(Uv)
		, Scale(Scale)
	{}
};

struct VertexSnow
{
	D3DXVECTOR3 Position;
	D3DXVECTOR2 Uv;
	D3DXVECTOR2 Random;
	D3DXVECTOR2 Scale;

	VertexSnow() {}

	VertexSnow
	(
		D3DXVECTOR3& Position,
		D3DXVECTOR2& Uv,
		D3DXVECTOR2& random,
		D3DXVECTOR2& Scale
	)
		: Position(Position)
		, Uv(Uv)
		, Random(random)
		, Scale(Scale)
	{}
};

