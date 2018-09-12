#pragma once
#include "GameScene.h"

namespace Landscape
{
	class Terrain;
}
class InGame : public GameScene
{
public:
	InGame(ExecuteValues* values);
	~InGame();

	void Update();
	void PreRender();
	void Render();
	void PostRender();

	void Clear();

	void TakeRootInfo();
	void TakeInfo(class GameRender* object);
	class Eve* GetPlayer() { return eve; }
	D3DXVECTOR3* Target();

private:
	class Eve* eve;
	vector<class Character*> monsters;
	Landscape::Terrain* terrain;
	class DirectionalLight * light;
	ViewProjectionBuffer* shadowBuffer;
};