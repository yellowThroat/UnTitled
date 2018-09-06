#pragma once
#include "Shape.h"

namespace Shapes
{
	class Segment
	{
	public:
		Segment(D3DXVECTOR3 p0, D3DXVECTOR3 p1);
		~Segment();

		static float distSegToSeg(Segment s1, Segment s2);

		D3DXVECTOR3 p0;
		D3DXVECTOR3 p1;
		D3DXVECTOR3 oP0;
		D3DXVECTOR3 oP1;

	};
}
