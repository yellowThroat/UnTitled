#pragma once

class DrawModel : public Execute
{
public:
	enum class SceneType
	{
		Unknown,
		LevelEditor,
		InGame,
		Count,
	};

public:
	DrawModel(ExecuteValues* values);
	~DrawModel();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	void InputHandle();
	void RunGame();
	void RunEditor();

	class GameSettings* settings;
	class InGame* inGame;

	class GameScene* currentScene;
	SceneType currentSceneType;
};