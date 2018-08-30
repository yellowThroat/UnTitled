#pragma once
#include "Shape.h"


namespace Shapes
{
	class Sphere;

	enum ContainmentType
	{
		Disjoint, Contains, Intersects
	};
	struct Plane
	{
		D3DXVECTOR3 normal;
		float d;
	};

	struct Ray
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 direction;
	};

	class Frustum : public Shape
	{
	public:
		Frustum() {}
		Frustum(D3DXMATRIX mat);
		~Frustum();
		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;
		void UpdateVertex();
	public:
		bool CheckCube(float, float, float, float);
		ContainmentType Contains(Shapes::Sphere* sphere);
		bool CheckFrustumIntersection(Shapes::Sphere* sphere);
		void UnprojectRectangle(RECT source, D3DXMATRIX projection, D3DXMATRIX view);

	private:
		void SetMatrix(D3DXMATRIX mat);
		D3DXVECTOR3 ComputeIntersection(Plane &pl, Shapes::Ray& ra);
		Shapes::Ray ComputeIntersectionLine(Plane &pl0, Plane& pl1);
	private:
		const int	_nearPlaneIndex		= 0;
		const int	_FarPlaneIndex		= 1;
		const int	_leftPlaneIndex		= 2;
		const int	_RightPlaneIndex	= 3;
		const int	_TopPlaneIndex		= 4;
		const int	_bottomPlaneIndex	= 5;
		const int	_numPlanes			= 6;
		D3DXMATRIX	_mat;
		Plane plane[6];
	};
}
