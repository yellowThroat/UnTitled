#include "stdafx.h"

#include "Segment.h"

#define EPSILON 0.0000001

Shapes::Segment::Segment(D3DXVECTOR3 p0, D3DXVECTOR3 p1)
	: p0(p0), p1(p1), oP0(p0), oP1(p1)
{
}

Shapes::Segment::~Segment()
{

}

float Shapes::Segment::distSegToSeg(Segment s1, Segment s2)
{
	D3DXVECTOR3	u = s1.p1 - s1.p0;
	D3DXVECTOR3	v = s2.p1 - s2.p0;
	D3DXVECTOR3	w = s1.p0 - s2.p0;
	D3DXVECTOR3	z = s1.p0 - s2.p1;
	D3DXVECTOR3	g = s1.p1 - s2.p0;
	D3DXVECTOR3	h = s1.p1 - s2.p1;
	float		a = D3DXVec3Dot(&u, &u);
	float		b = D3DXVec3Dot(&u, &v);
	float		c = D3DXVec3Dot(&v, &v);
	float		d = D3DXVec3Dot(&u, &w);
	float		e = D3DXVec3Dot(&v, &w);
	float		D = a*c - b*b;
	float		sc, sN, sD = D;
	float		tc, tN, tD = D;

	if (D < EPSILON)
	{
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else
	{
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0f) {
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f) {
		tN = 0.0f;
		if (-d < 0.0f)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {
		tN = tD;
		if ((-d + b) < 0.0f)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}

	sc = (abs(sN) < EPSILON ? 0.0f : sN / sD);
	tc = (abs(tN) < EPSILON ? 0.0f : tN / tD);

	D3DXVECTOR3 dP = w + (u * sc) - (v * tc);

	float distance = D3DXVec3Length(&dP);
	float wDistance = D3DXVec3Length(&w);
	float zDistance = D3DXVec3Length(&z);
	float gDistance = D3DXVec3Length(&g);
	float hDistance = D3DXVec3Length(&h);

	distance = min(distance, wDistance);
	distance = min(distance, zDistance);
	distance = min(distance, gDistance);
	distance = min(distance, hDistance);

	return distance;
}
