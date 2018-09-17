#pragma once

class DrawGui: public Execute
{
public:
	DrawGui(ExecuteValues* values);
	~DrawGui();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	D3DXMATRIX view;
	class Orthographic* projection;
	ID3D11BlendState* blendState[2];
	Shader* shader;
	
};
