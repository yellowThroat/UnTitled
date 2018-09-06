#include "stdafx.h"

#include "Box.h"
#include "Sphere.h"

Shapes::Box::Box(D3DXVECTOR3 min, D3DXVECTOR3 max)
	: _min(min), _max(max)
{
	MakeShape();
	MakeBuffer();
}

Shapes::Box::~Box()
{

}

void Shapes::Box::Update()
{
	Shapes::Shape::Update();
}

void Shapes::Box::Render()
{
	Shapes::Shape::Render();
}

void Shapes::Box::MakeShape()
{
	_lineCount = 12;
	_vertexCount = 8;
	_indexCount = _lineCount * 2;

	_data = new VertexColor[8];
	VertexColor V;
	V.color = D3DXCOLOR(1, 0, 0, 1);
	V.position = D3DXVECTOR3(_min.x, _min.y, _min.z); _data[0] = V;
	V.position = D3DXVECTOR3(_min.x, _max.y, _min.z); _data[1] = V;
	V.position = D3DXVECTOR3(_max.x, _min.y, _min.z); _data[2] = V;
	V.position = D3DXVECTOR3(_max.x, _max.y, _min.z); _data[3] = V;
	V.position = D3DXVECTOR3(_min.x, _min.y, _max.z); _data[4] = V;
	V.position = D3DXVECTOR3(_min.x, _max.y, _max.z); _data[5] = V;
	V.position = D3DXVECTOR3(_max.x, _min.y, _max.z); _data[6] = V;
	V.position = D3DXVECTOR3(_max.x, _max.y, _max.z); _data[7] = V;

	_indexData = new UINT[24]
	{ 0,1,0,2,0,4,3,1,3,2,3,7,5,1,5,7,5,4,6,4,6,2,6,7 };
}

bool Shapes::Box::Intersect(Sphere * sphere)
{
	float single;
	D3DXVECTOR3 v = sphere->GetPosition();
	D3DXVECTOR3 min, max;
	D3DXVec3TransformCoord(&min, &_min, &_world);
	D3DXVec3TransformCoord(&max, &_max, &_world);
	v.x = Math::Clamp(v.x, min.x, max.x);
	v.y = Math::Clamp(v.y, min.y, max.y);
	v.z = Math::Clamp(v.z, min.z, max.z);
	D3DXVECTOR3 vec = sphere->GetPosition() - v;
	single = D3DXVec3LengthSq(&vec);
	if (single <= sphere->GetRadius() * sphere->GetRadius())
		return true;

	return false;
}
