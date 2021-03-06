#pragma once
class Math
{
public:
	static const float PI;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);

	static float Clamp(float value, float min, float max);

	static float Distance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);
	static float DistanceSquared(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);
	static float DistancePointFromLine(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2, D3DXVECTOR3 vec3);
	static void RadianFromDirection(float& x, float& y, D3DXVECTOR3 dir);
	static float AngleToRadianY(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);
	static float AngleToDegreeY(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);
};