#pragma once
#include "Shape.h"

namespace Shapes
{
	class Sphere : public Shape
	{
	public:
		Sphere(float radius = 1.0f);
		~Sphere();

		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;

		float GetRadius() { return _radius; }
		void SetRadius(float radius);
	private:
		float		_radius;
	};
}
