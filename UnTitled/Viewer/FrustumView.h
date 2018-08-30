#pragma once

class FrustumView
{
public:
	FrustumView(float zFar, Camera* camera, Perspective* perpective);
	~FrustumView();

	void Build();
	void Update();

	bool ContainPoint(float x, float y, float z);
	bool ContainPoint(D3DXVECTOR3 position);

private:
	D3DXPLANE planes[6];

	float zFar;

	Camera* camera;
	Perspective* perspective;

};