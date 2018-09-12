#pragma once
class ExecuteValues;
class Canvas
{
public:
	Canvas(ExecuteValues* val);
	~Canvas();

	void Update();
	void Render();

	void Create(float w = 0, float h = 0, float sX = 0, float sY = 0);

private:
	ExecuteValues* values;
	D3DXMATRIX view;
	class Orthographic* projection;
	class Model* quad;
	Shader* shader;

	float width;
	float height;
	float startX;
	float startY;
};