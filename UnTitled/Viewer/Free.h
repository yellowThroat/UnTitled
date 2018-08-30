#pragma once
#include "Camera.h"

class Free : public Camera
{
public:
	Free(float moveSpeed = 20, float rotationSpeed = 2.5f);
	~Free();

	void Update();

private:
	float moveSpeed;
	float rotationSpeed;
};