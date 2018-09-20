#pragma once
#include "Camera.h"

class TPP : public Camera
{
public:
	TPP(float distance = 10.0f, float rotationSpeed = 0.01f);
	~TPP();

	void Update() override;
	void SetTarget(D3DXVECTOR3* target) override;

private:
	void FixMouse();
	float rotationSpeed;
	float distance;
	D3DXVECTOR3 * target;
};