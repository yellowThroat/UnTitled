#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;
	
	return r1 + val;
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}

float Math::Clamp(float value, float min, float max)
{
	value = value > max ? max : value;
	value = value < min ? min : value;

	return value;
}

float Math::Distance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2)
{
	D3DXVECTOR3 vec = vec1 - vec2;
	float result = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

	return sqrtf(result);
}

float Math::DistanceSquared(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2)
{
	D3DXVECTOR3 vec = vec1 - vec2;

	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

void Math::RadianFromDirection(float& x, float& y, D3DXVECTOR3 dir)
{
	D3DXVECTOR3 X = dir;
	X.y = 0;
	D3DXVec3Normalize(&X, &X);
	x = acosf(D3DXVec3Dot(&dir, &X));
	if (dir.y > 0) x = -x;

	D3DXVECTOR3 Y = D3DXVECTOR3(0, 0, 1);
	y = acosf(D3DXVec3Dot(&X, &Y));

	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &Y, &X);
	if (cross.y < 0) y = -y;
}
