#pragma once
#include "Shape.h"

namespace Shapes
{
	class Segment;

	class Capsule : public Shape
	{
	public:
		Capsule(D3DXVECTOR3 p1, D3DXVECTOR3 p2, float radius);
		~Capsule();
		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;

		bool static Intersect(Capsule * c1, Capsule * c2);

	private:

		float radius;
		Segment* segment;
	};
}
