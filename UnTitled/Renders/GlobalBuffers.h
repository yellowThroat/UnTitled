#pragma once
#include "stdafx.h"

class WorldBuffer : public ShaderBuffer
{
public:
	WorldBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.World);
	}

	void SetMatrix(D3DXMATRIX mat)
	{
		Data.World = mat;
		D3DXMatrixTranspose(&Data.World, &Data.World);
	}

	struct Struct
	{
		D3DXMATRIX World;
	};

private:
	Struct Data;
};

class ViewProjectionBuffer : public ShaderBuffer
{
public:
	ViewProjectionBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.View);
		D3DXMatrixIdentity(&Data.Projection);
	}

	void SetView(D3DXMATRIX& mat)
	{
		Data.View = mat;
		D3DXMatrixInverse(&Data.ViewInverse, NULL, &Data.View);

		D3DXMatrixTranspose(&Data.View, &Data.View);
		D3DXMatrixTranspose(&Data.ViewInverse, &Data.ViewInverse);
	}

	void SetProjection(D3DXMATRIX& mat)
	{
		Data.Projection = mat;
		D3DXMatrixTranspose(&Data.Projection, &Data.Projection);
	}

	struct Struct
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewInverse;
	};

private:
	Struct Data;
};

class LightBuffer : public ShaderBuffer
{
public:
	LightBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.Direction = D3DXVECTOR3(-1, -1, -1);
		Data.Dir = D3DXVECTOR3(-1, -1, -1);
		Data.Color = D3DXCOLOR(1, 1, 1, 1);

		Data.Overcast = 1.0f;
		
	}

	struct Struct
	{
		D3DXCOLOR Color;
		D3DXVECTOR3 Direction;
		float Overcast;

		D3DXVECTOR3 Dir;
		float Intensity;
	};

	Struct Data;
};

class TimeBuffer : public ShaderBuffer
{
public:
	TimeBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.ElapsedTime = 0.0f;
		Data.RunningTime = 0.0f;
	}

	struct Struct
	{
		float ElapsedTime;
		float RunningTime;

		float Padding[2];
	};

	Struct Data;
};