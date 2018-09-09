#include "stdafx.h"

#include "Sphere.h"
#include "Capsule.h"
#include "Segment.h"

Shapes::Sphere::Sphere(float radius)
	: _radius(radius)
{
	MakeShape();
	MakeBuffer();
}

Shapes::Sphere::~Sphere()
{

}

void Shapes::Sphere::Update()
{
	Shapes::Shape::Update();
}

void Shapes::Sphere::Render()
{
	Shapes::Shape::Render();
}

void Shapes::Sphere::MakeShape()
{
	const int sphereResolution = 30;
	_lineCount = (sphereResolution + 1) *3;
	_vertexCount = _lineCount * 2;
	_indexCount = _vertexCount;

	_data = new Vertex[_vertexCount];

	float step = 2.0f * (float)D3DX_PI / sphereResolution;

	int index = 0;

	Vertex V;
	for (float i = 0.0f; i < 2 * D3DX_PI; i+= step)
	{
		V.position = D3DXVECTOR3(cosf(i), sinf(i), 0.0f);
		V.position *= _radius;
		_data[index++] = V;

		V.position = D3DXVECTOR3(cosf(i + step), sinf(i + step), 0.0f);
		V.position *= _radius;
		_data[index++] = V;
	}

	for (float i = 0.0f; i < 2 * D3DX_PI; i += step)
	{
		V.position = D3DXVECTOR3(cosf(i), 0.0f, sinf(i));
		V.position *= _radius; 
		_data[index++] = V;
		
		V.position = D3DXVECTOR3(cosf(i + step), 0.0f, sinf(i + step)); 
		V.position *= _radius; 
		_data[index++] = V;
	}

	for (float i = 0; i < 2 * D3DX_PI; i+=step)
	{
		V.position = D3DXVECTOR3(0.0f, cosf(i), sinf(i));
		V.position *= _radius; 
		_data[index++] = V;
		
		V.position = D3DXVECTOR3(0.0f, cosf(i + step), sinf(i + step));
		V.position *= _radius; 
		_data[index++] = V;
	}

	_indexData = new UINT[_indexCount];
	for (UINT i = 0; i < _indexCount; i++)
	{
		_indexData[i] = i;
	}
}

float Shapes::Sphere::GetRadius()
{
	D3DXVECTOR3 S, T;
	D3DXQUATERNION Q;
	D3DXMatrixDecompose(&S, &Q, &T, &_world);
	return _radius * fabsf(S.x);
}

void Shapes::Sphere::SetRadius(float radius)
{
	for (UINT i = 0; i < _vertexCount; i++)
	{
		_data[i].position /= _radius;
		_data[i].position *= radius;
	}

	ChangeData();

	_radius = radius;
}

bool Shapes::Sphere::Collide(Shape * shape)
{
	return shape->Collide(this);
}

bool Shapes::Sphere::Collide(Sphere * sphere)
{
	ContainmentType type = Contains(sphere);

	switch (type)
	{
	case Shapes::ContainmentType::Disjoint:
		return false;
		break;
	case Shapes::ContainmentType::Contains:
	case Shapes::ContainmentType::Intersects:
		return true;
		break;
	}

	return false;
}

bool Shapes::Sphere::Collide(Capsule * capsule)
{
	ContainmentType type = Contains(capsule);
	switch (type)
	{
	case Shapes::ContainmentType::Disjoint:
		return false;
		break;
	case Shapes::ContainmentType::Contains:
	case Shapes::ContainmentType::Intersects:
		return true;
		break;
	}
	return false;
}

Shapes::ContainmentType Shapes::Sphere::Contains(Shape * shape)
{
	return shape->Contains(this);
}

Shapes::ContainmentType Shapes::Sphere::Contains(Capsule * capsule)
{
	D3DXVECTOR3 p0 = capsule->GetSSegment()->p0;
	D3DXVECTOR3 p1 = capsule->GetSSegment()->p1;

	float distance = Math::DistancePointFromLine(p0, p1, _position);

	float sumRadius = GetRadius() + capsule->GetRadius();

	if (distance > sumRadius) return ContainmentType::Disjoint;
	else
	{
		if (distance + GetRadius() <= capsule->GetRadius())
			return ContainmentType::Contains;

		return ContainmentType::Intersects;
	}


	return ContainmentType::Disjoint;
}

Shapes::ContainmentType Shapes::Sphere::Contains(Sphere * sphere)
{
	D3DXVECTOR3 p0 = GetPosition();
	D3DXVECTOR3 p1 = sphere->GetPosition();

	float distance = Math::Distance(p0, p1);
	float sumRadius = GetRadius() + sphere->GetRadius();

	if (distance > sumRadius) return ContainmentType::Disjoint;
	else
	{
		float min;
		min = GetRadius() <= sphere->GetRadius() ? GetRadius() : sphere->GetRadius();
		if (distance + min <= sumRadius - min)
			return ContainmentType::Contains;

		return ContainmentType::Intersects;
	}
	return ContainmentType::Disjoint;
}
