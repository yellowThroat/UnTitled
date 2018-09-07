#pragma once

class DrawEnvironment: public Execute
{
public:
	DrawEnvironment(ExecuteValues* values);
	~DrawEnvironment();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	class Sky* sky;
	class Grid* grid;
};
