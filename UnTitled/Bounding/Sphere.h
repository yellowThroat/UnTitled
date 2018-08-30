#pragma once
#include "Shape.h"

namespace Shapes
{
	class Sphere : public Shape
	{
	public:
		Sphere(D3DXVECTOR3 center = D3DXVECTOR3(0, 0, 0), float radius = 0.5f);
		~Sphere();
		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;

		D3DXVECTOR3 GetCenter();
		float GetRadius() { return _radius; }
		void SetRadius(float radius);
	private:
		D3DXVECTOR3 _center;
		float		_radius;

	};
}
