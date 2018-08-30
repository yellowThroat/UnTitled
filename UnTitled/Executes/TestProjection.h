#pragma once


class TestProjection : public Execute
{
public:
	TestProjection(ExecuteValues* values);
	~TestProjection();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	vector<D3DXVECTOR3> positions;

	class MeshCube* cube;
	class MeshSphere* sphere;
	class MeshPlane* plane;

	class FixedCamera* camera;
	class Perspective* perspective;

	ViewProjectionBuffer* buffer;
};