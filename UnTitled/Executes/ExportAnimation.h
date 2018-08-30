#pragma once

namespace Xml
{
	class XMLElement;
}

class ExportAnimation : public Execute
{
public:
	ExportAnimation(ExecuteValues* values);
	~ExportAnimation();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	
};