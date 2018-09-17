#pragma once

class Canvas
{
public:
	Canvas();
	~Canvas();

	void Update();
	void Render();

	void Create(float w = 0, float h = 0, float sX = 0, float sY = 0);
	void SetTexture(wstring file);

private:
	class Model* quad;

	float width;
	float height;
	float startX;
	float startY;
};