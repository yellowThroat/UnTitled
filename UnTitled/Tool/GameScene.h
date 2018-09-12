#pragma once

struct ExecuteValues;

class GameScene
{
public:
	GameScene(ExecuteValues* values);
	~GameScene();

	virtual void Update() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

protected:

	ExecuteValues* values;
};