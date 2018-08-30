#pragma once
#include "Shape.h"

namespace Shapes
{
	class Cone : public Shape
	{
	public:
		Cone(D3DXVECTOR3 center = D3DXVECTOR3(0, 0, 0), float angle = 0.5f, float height = 1.0f);
		~Cone();
		virtual void Update() override;
		virtual void Render() override;
		virtual void MakeShape() override;

		D3DXVECTOR3 GetCenter();
		float GetAngle() { return _angle; }
		void SetAngle(float angle);
		float GetHeight() { return _height; }
		void SetHeight(float height);
	private:
		D3DXVECTOR3 _center;
		float		_angle;
		float		_height;

	};
}
