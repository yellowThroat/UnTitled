#pragma once
#include "Shape.h"

namespace Shapes
{
	class Capsule;
	class Box;
	class Frustum;
	
	class Sphere : public Shape
	{
	public:
		Sphere(float radius = 1.0f);
		~Sphere();

		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;

		float GetRadius() override;
		void SetRadius(float radius);
		
		bool Collide(Shape* shape);
		bool Collide(Sphere* sphere);
		bool Collide(Capsule* capsule);

		ContainmentType Contains(Shape* shape);
		ContainmentType Contains(Capsule* capsule);
		ContainmentType Contains(Sphere* sphere);

	private:
		float		_radius;
	};
}
