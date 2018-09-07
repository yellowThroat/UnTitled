#pragma once
namespace Shapes
{
	class Shape;
}

class DrawModel : public Execute
{
public:
	DrawModel(ExecuteValues* values);
	~DrawModel();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	void Test();

	class GameSettings* settings;
	class Eve* eve;
	class Zombie* zombie;
};