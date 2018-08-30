#include "stdafx.h"

#include "Frustum.h"
#include "Sphere.h"

Shapes::Frustum::Frustum(D3DXMATRIX mat)
{
	SetMatrix(mat);
	MakeShape();
	MakeBuffer();
}

Shapes::Frustum::~Frustum()
{

}

void Shapes::Frustum::Update()
{
	Shapes::Shape::Update();
}

void Shapes::Frustum::Render()
{
	Shapes::Shape::Render();
}

void Shapes::Frustum::MakeShape()
{
	_lineCount = 12;
	_vertexCount = 8;
	_indexCount = _lineCount * 2;


	_data = new VertexColor[_vertexCount];

	VertexColor V;
	V.color = D3DXCOLOR(0, 0, 1, 1);

	Shapes::Ray ray = ComputeIntersectionLine(plane[0], plane[2]);
	V.position = ComputeIntersection(plane[4], ray); _data[0] = V;
	V.position = ComputeIntersection(plane[5], ray); _data[3] = V;

	ray = ComputeIntersectionLine(plane[3], plane[0]);
	V.position = ComputeIntersection(plane[4], ray); _data[1] = V;
	V.position = ComputeIntersection(plane[5], ray); _data[2] = V;

	ray = ComputeIntersectionLine(plane[2], plane[1]);
	V.position = ComputeIntersection(plane[4], ray); _data[4] = V;
	V.position = ComputeIntersection(plane[5], ray); _data[7] = V;

	ray = ComputeIntersectionLine(plane[1], plane[3]);
	V.position = ComputeIntersection(plane[4], ray); _data[5] = V;
	V.position = ComputeIntersection(plane[5], ray); _data[6] = V;

	_indexData = new UINT[_indexCount]
	{ 0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7 };
}

void Shapes::Frustum::UpdateVertex()
{
	VertexColor V;
	V.color = D3DXCOLOR(0, 0, 1, 1);

	Shapes::Ray ray = ComputeIntersectionLine(plane[0], plane[2]);
	V.position = ComputeIntersection(plane[4], ray); _data[0] = V;
	V.position = ComputeIntersection(plane[5], ray); _data[3] = V;

	ray = ComputeIntersectionLine(plane[3], plane[0]);
	V.position = ComputeIntersection(plane[4], ray); _data[1] = V;
	V.position = ComputeIntersection(plane[5], ray); _data[2] = V;

	ray = ComputeIntersectionLine(plane[2], plane[1]);
	V.position = ComputeIntersection(plane[4], ray); _data[4] = V;
	V.position = ComputeIntersection(plane[5], ray); _data[7] = V;

	ray = ComputeIntersectionLine(plane[1], plane[3]);
	V.position = ComputeIntersection(plane[4], ray); _data[5] = V;
	V.position = ComputeIntersection(plane[5], ray); _data[6] = V;

	D3D::GetDC()->UpdateSubresource
	(
		_buffer, 0, NULL, _data, sizeof(VertexColor) * _vertexCount, 0
	);

}

bool Shapes::Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	D3DXPLANE pl[6];
	for (int i = 0; i < 6; i++)
	{
		pl[i].a = plane[i].normal.x;
		pl[i].b = plane[i].normal.y;
		pl[i].c = plane[i].normal.z;
		pl[i].d = plane[i].d;
	}

	for (int i = 0; i < 6; i++)
	{
		
		if (D3DXPlaneDotCoord(&pl[i], &D3DXVECTOR3(xCenter - radius, yCenter - radius, zCenter - radius)) >= 0.0f)
			continue;
		if (D3DXPlaneDotCoord(&pl[i], &D3DXVECTOR3(xCenter + radius, yCenter - radius, zCenter - radius)) >= 0.0f)
			continue;
		if (D3DXPlaneDotCoord(&pl[i], &D3DXVECTOR3(xCenter - radius, yCenter + radius, zCenter - radius)) >= 0.0f)
			continue;
		if (D3DXPlaneDotCoord(&pl[i], &D3DXVECTOR3(xCenter + radius, yCenter + radius, zCenter - radius)) >= 0.0f)
			continue;
		if (D3DXPlaneDotCoord(&pl[i], &D3DXVECTOR3(xCenter - radius, yCenter - radius, zCenter + radius)) >= 0.0f)
			continue;
		if (D3DXPlaneDotCoord(&pl[i], &D3DXVECTOR3(xCenter + radius, yCenter - radius, zCenter + radius)) >= 0.0f)
			continue;
		if (D3DXPlaneDotCoord(&pl[i], &D3DXVECTOR3(xCenter - radius, yCenter + radius, zCenter + radius)) >= 0.0f)
			continue;
		if (D3DXPlaneDotCoord(&pl[i], &D3DXVECTOR3(xCenter + radius, yCenter + radius, zCenter + radius)) >= 0.0f)
			continue;

		return false;
	}
	return true;
}

Shapes::ContainmentType Shapes::Frustum::Contains(Shapes::Sphere* sphere)
{
	D3DXVECTOR3 center = sphere->GetCenter();
	float radius = sphere->GetRadius();
	int num = 0;
	for (int i = 0; i < 6; i++)
	{
		Shapes::Plane p = plane[i];
		float x = p.normal.x * center.x
			+ p.normal.y * center.y 
			+ p.normal.z * center.z;
		float d = x + p.d;
		if (d > radius)
		{
			return Shapes::ContainmentType::Disjoint;
		}
		if (d < -radius) num++;
	}
	if (num != 6)
	{
		return Shapes::ContainmentType::Intersects;
	}
	return Shapes::ContainmentType::Contains;
}

bool Shapes::Frustum::CheckFrustumIntersection(Shapes::Sphere* sphere)
{
	ContainmentType con = Contains(sphere);
	if (con == ContainmentType::Contains ||
		con == ContainmentType::Intersects) return true;
	return false;
}

void Shapes::Frustum::SetMatrix(D3DXMATRIX mat)
{
	_mat = mat;

	plane[2].normal.x = -_mat._14 - _mat._11;
	plane[2].normal.y = -_mat._24 - _mat._21;
	plane[2].normal.z = -_mat._34 - _mat._31;
	plane[2].d = -_mat._44 - _mat._41;

	plane[3].normal.x = -_mat._14 + _mat._11;
	plane[3].normal.y = -_mat._24 + _mat._21;
	plane[3].normal.z = -_mat._34 + _mat._31;
	plane[3].d = -_mat._44 + _mat._41;

	plane[4].normal.x = -_mat._14 + _mat._12;
	plane[4].normal.y = -_mat._24 + _mat._22;
	plane[4].normal.z = -_mat._34 + _mat._32;
	plane[4].d = -_mat._44 + _mat._42;

	plane[5].normal.x = -_mat._14 - _mat._12;
	plane[5].normal.y = -_mat._24 - _mat._22;
	plane[5].normal.z = -_mat._34 - _mat._32;
	plane[5].d = -_mat._44 - _mat._42;

	plane[0].normal.x = -_mat._13;
	plane[0].normal.y = -_mat._23;
	plane[0].normal.z = -_mat._33;
	plane[0].d = -_mat._43;

	plane[1].normal.x = -_mat._14 + _mat._13;
	plane[1].normal.y = -_mat._24 + _mat._23;
	plane[1].normal.z = -_mat._34 + _mat._33;
	plane[1].d = -_mat._44 + _mat._43;

	for (int i = 0; i < 6; i++)
	{
		float single = D3DXVec3Length(&plane[i].normal);
		plane[i].normal /= single;
		plane[i].d /= single;
	}
}

D3DXVECTOR3 Shapes::Frustum::ComputeIntersection(Plane & pl, Shapes::Ray & ra)
{
	float d = (-pl.d - D3DXVec3Dot(&pl.normal, &ra.position)) /
		D3DXVec3Dot(&pl.normal, &ra.direction);
	return ra.position + (ra.direction *d);
	
}

Shapes::Ray Shapes::Frustum::ComputeIntersectionLine(Plane & pl0, Plane & pl1)
{
	Shapes::Ray ray;
	D3DXVec3Cross(&ray.direction, &pl0.normal, &pl1.normal);
	float single = D3DXVec3LengthSq(&ray.direction);

	D3DXVECTOR3 tmp = -pl0.d * pl1.normal + pl1.d * pl0.normal;

	D3DXVec3Cross(&ray.position, &tmp, &ray.direction);
	return ray;
}

void Shapes::Frustum::UnprojectRectangle(RECT source, D3DXMATRIX projection, D3DXMATRIX view)
{
	D3DXVECTOR2 regionCenterScreen = 
		D3DXVECTOR2(((float)source.left + (float)source.right / 2),
		((float)source.top + (float)source.bottom / 2));

	D3DXMATRIX regionProjMatrix = projection;

	float Width = _values->Viewport->GetWidth();
	float Height = _values->Viewport->GetHeight();

	regionProjMatrix._11 /= ((float)source.right / Width);
	regionProjMatrix._22 /= ((float)source.bottom / Height);

	regionProjMatrix._31 = 
		-(regionCenterScreen.x - (Width / 2.0f)) / ((float)source.right / 2.0f);

	regionProjMatrix._32 =
		(regionCenterScreen.y - (Height / 2.0f)) / ((float)source.bottom / 2.0f);

	D3DXMATRIX mat = view * regionProjMatrix;

	SetMatrix(mat);
	UpdateVertex();
}

