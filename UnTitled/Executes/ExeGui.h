#pragma once
#include "Execute.h"

class ExeGui : public Execute
{
public:
	ExeGui(ExecuteValues* values);
	~ExeGui();

	void Update();

	void PreRender() {}
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	void ChangeShowGlobalLightWindow();
	void ChangeShowEnvironmentWindow();
	void ChangeShowLandscapeWindow();
	void ChangeShowSystemInfoWindow();
	void ChangeShowDemoWindow();
};