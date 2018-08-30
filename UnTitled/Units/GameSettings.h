#pragma once
struct ExecuteValues;

namespace Xml
{
	class XMLDocument;
	class XMLElement;
}

class GameSettings
{
public:
	GameSettings(ExecuteValues* values);
	~GameSettings();

	
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	
	void FixCamera(bool val) { fixCamera = val; }
	
private:
	void CreateEmpty();
	void CreateEnvironment();
	void CreatePlayer();
	void CreateTerrain();
	void CreateLights(UINT type);
	void CreateObject(UINT type);
	void FixedCamera();

private:
	void InputHandle();
	void DeleteCurrent();
	void SaveProject(wstring file);
	void LoadProject(wstring file);

	class GameRender* current;

private:
	ExecuteValues* values;
	bool fixCamera;
};
