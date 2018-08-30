#pragma once
#include "Camera.h"

class FixedCamera : public Camera
{
public:
	FixedCamera();
	~FixedCamera();

	void Update();

};