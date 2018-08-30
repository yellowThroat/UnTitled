#pragma once
#include "Shape.h"

namespace Shapes
{
	class Sphere;

	class Box : public Shape
	{
	public:
		Box(D3DXVECTOR3 min, D3DXVECTOR3 max);
		~Box();
		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;

		bool Intersect(Sphere* sphere);

	private:
		D3DXVECTOR3 _min;
		D3DXVECTOR3 _max;

	};
}
