#pragma once
#include "Camera.h"

class TPP : public Camera
{
public:
	TPP(float rotationSpeed = 0.01f);
	~TPP();

	void Update();

private:
	float rotationSpeed;
};