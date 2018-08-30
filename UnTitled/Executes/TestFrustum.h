#pragma once


class TestFrustum : public Execute
{
public:
	TestFrustum(ExecuteValues* values);
	~TestFrustum();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	vector<D3DXVECTOR3> positions;

	class MeshCube* cube;
	class MeshPlane* plane;
	class Frustum * frustum;
	class FixedCamera* fixed;
	class Perspective* perspective;
};