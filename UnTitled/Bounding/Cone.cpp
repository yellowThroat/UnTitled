#include "stdafx.h"

#include "Cone.h"

Shapes::Cone::Cone(float angle, float height)
	: _angle(angle)
	, _height(height)
{
	MakeShape();
	MakeBuffer();
}

Shapes::Cone::~Cone()
{

}

void Shapes::Cone::Update()
{
	Shapes::Shape::Update();
}

void Shapes::Cone::Render()
{
	Shapes::Shape::Render();
}

void Shapes::Cone::MakeShape()
{
	_lineCount = 32;
	_vertexCount = _lineCount * 2 + 1;
	_indexCount = _vertexCount + 7;

	_data = new Vertex[_vertexCount];

	float h = tanf(_angle) * _height;
	float step = 2.0f * (float)D3DX_PI / 32;

	int index = 0;

	Vertex V;
	V.position = D3DXVECTOR3(0, 0, 0);
	_data[index++] = V;
	for (float i = 0.0f; i < 2 * D3DX_PI; i+= step)
	{
		V.position = D3DXVECTOR3(cosf(i), sinf(i), 0.0f);
		V.position *= h;
		V.position.z = _height;
		_data[index++] = V;

		V.position = D3DXVECTOR3(cosf(i + step), sinf(i + step), 0.0f);
		V.position *= h;
		V.position.z = _height;
		_data[index++] = V;
	}


	_indexData = new UINT[_indexCount];
	for (UINT i = 0; i < _vertexCount - 1; i++)
		_indexData[i] = i + 1;

	_indexData[_vertexCount - 1] = 1;
	_indexData[_vertexCount + 0] = 0;
	_indexData[_vertexCount + 1] = 17;
	_indexData[_vertexCount + 2] = 0;
	_indexData[_vertexCount + 3] = 33;
	_indexData[_vertexCount + 4] = 0;
	_indexData[_vertexCount + 5] = 49;
	_indexData[_vertexCount + 6] = 0;

}

void Shapes::Cone::SetAngle(float angle)
{
	_angle = angle;
	Clear();

	MakeShape();
	MakeBuffer();
}

void Shapes::Cone::SetHeight(float height)
{
	_height = height;
	Clear();

	MakeShape();
	MakeBuffer();
}

bool Shapes::Cone::Collide(Shape * shape)
{
	return shape->Collide(this);
}

Shapes::ContainmentType Shapes::Cone::Contains(Shape * shape)
{
	return shape->Contains(this);
}
