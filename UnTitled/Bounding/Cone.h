#pragma once
#include "Shape.h"

namespace Shapes
{
	class Cone : public Shape
	{
	public:
		Cone(float angle = 0.5f, float height = 1.0f);
		~Cone();
		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;

		float GetAngle() { return _angle; }
		void SetAngle(float angle);
		float GetHeight() { return _height; }
		void SetHeight(float height);

		bool Collide(Shape* shape);
		Shapes::ContainmentType Contains(Shape* shape);
	private:
		float		_angle;
		float		_height;

	};
}
