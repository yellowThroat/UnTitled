#pragma once
#include "GameScene.h"

struct ExecuteValues;

class GameSettings : public GameScene
{
public:
	enum class MonsterType
	{
		Unknown = -1,
		Zombie,
		Count,
	};

public:
	GameSettings(ExecuteValues* values);
	~GameSettings();

	
	void Update();
	void PreRender();
	void Render();
	void PostRender();

public:
	void ClearRoot();
	void SaveProject(wstring file);
	void LoadProject(wstring file);

private:
	void CreateEmpty();
	void CreateEnvironment();
	void CreatePlayer();
	void CreateMonsters(MonsterType type);
	void CreateTerrain();
	void CreateLights(UINT type);
	void CreateObject(UINT type);

private:
	void InputHandle();
	void DeleteCurrent();

	class GameRender* current;
};
