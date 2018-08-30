#pragma once

class ExportMesh : public Execute
{
public:
	ExportMesh(ExecuteValues* values);
	~ExportMesh();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	
};