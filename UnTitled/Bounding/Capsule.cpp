#include "stdafx.h"

#include "Capsule.h"
#include "Segment.h"

Shapes::Capsule::Capsule(D3DXVECTOR3 p1, D3DXVECTOR3 p2, float radius)
	: radius(radius)
{
	segment = new Segment(p1, p2);
	MakeShape();
	MakeBuffer();
}

Shapes::Capsule::~Capsule()
{
	SAFE_DELETE(segment);
}

void Shapes::Capsule::Update()
{
	D3DXVec3TransformCoord(&segment->p0, &segment->oP0, &_world);
	D3DXVec3TransformCoord(&segment->p1, &segment->oP1, &_world);
	
	Shapes::Shape::Update();
}

void Shapes::Capsule::Render()
{
	Shapes::Shape::Render();
}

void Shapes::Capsule::MakeShape()
{
	const int CapsuleResolution = 30;
	const float CapsuleLength = D3DXVec3Length(&(segment->p1 - segment->p0));
	_lineCount = (CapsuleResolution + 1) *2;
	_vertexCount = _lineCount * 2;
	_indexCount = _vertexCount;

	_data = new Vertex[_vertexCount];

	float step = 2.0f * (float)D3DX_PI / CapsuleResolution;

	int index = 0;
	
	Vertex V;
	for (float i = 0.0f; i < 2 * D3DX_PI; i+= step)
	{
		V.position = D3DXVECTOR3(cosf(i), sinf(i), 0.0f);
		V.position *= radius;

		if (i <= D3DX_PI) V.position.y += CapsuleLength - radius;
		else V.position.y += radius;

		_data[index++] = V;


		V.position = D3DXVECTOR3(cosf(i + step), sinf(i + step), 0.0f);
		V.position *= radius;

		if (i + step <= D3DX_PI) V.position.y += CapsuleLength- radius;
		else V.position.y += radius;
		_data[index++] = V;
	}

	_data[index - 1] = _data[0];


	for (float i = 0; i < 2 * D3DX_PI; i+=step)
	{
		V.position = D3DXVECTOR3(0.0f, sinf(i), cosf(i));
		V.position *= radius; 

		if (i <= D3DX_PI) V.position.y += CapsuleLength - radius;
		else V.position.y += radius;
		_data[index++] = V;

		
		V.position = D3DXVECTOR3(0.0f, sinf(i + step), cosf(i + step));
		V.position *= radius; 

		if (i + step <= D3DX_PI) V.position.y += CapsuleLength - radius;
		else V.position.y += radius;
		_data[index++] = V;
	}
	_data[index - 1] = _data[_vertexCount / 2];

	D3DXMATRIX A;
	float angle;
	D3DXVECTOR3 v = segment->p1 - segment->p0;
	D3DXVECTOR3 right;
	D3DXVec3Normalize(&v, &v);
	D3DXVec3Cross(&right, &D3DXVECTOR3(0, 1, 0), &v);

	angle = D3DXVec3Dot(&v, &D3DXVECTOR3(0, 1, 0));
	angle = Math::Clamp(angle, -1.0f, 1.0f);
	angle = acosf(angle);

	D3DXMatrixRotationAxis(&A, &right, angle);

	for (UINT i = 0; i < _vertexCount; i++)
	{
		D3DXVec3TransformCoord(&_data[i].position, &_data[i].position, &A);
		_data[i].position += segment->p0;
	}

	_indexData = new UINT[_indexCount];
	for (UINT i = 0; i < _indexCount; i++)
		_indexData[i] = i;
}

bool Shapes::Capsule::Intersect(Capsule * c1, Capsule * c2)
{
	if (c1->radius + c2->radius > Segment::distSegToSeg(*c1->segment, *c2->segment)) {
		return true;
	}

	return false;
}

