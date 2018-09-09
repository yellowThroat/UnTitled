#pragma once
#include "Shape.h"

namespace Shapes
{
	class Segment;
	class Box;
	class Sphere;
	class Frustum;
	
	class Capsule : public Shape
	{
	public:
		Capsule(D3DXVECTOR3 p1 = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3 p2 = D3DXVECTOR3(0, 10, 0), float radius = 1.0f);
		~Capsule();
		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;

		void SetWorld(D3DXMATRIX mat) override;
		float GetRadius() override;
		bool static Intersect(Capsule * c1, Capsule * c2);
		
		bool Collide(Shape* shape);
		bool Collide(Sphere* sphere);
		bool Collide(Capsule* capsule);

		Shapes::ContainmentType Contains(Shape* shape);
		Shapes::ContainmentType Contains(Capsule* capsule);
		Shapes::ContainmentType Contains(Sphere* sphere);
		
		Segment* GetSegment() { return segment; }
		Segment* GetSSegment() { return shortSegment; }

	private:

		float radius;
		Segment* segment;
		Segment* shortSegment;
	};
}
