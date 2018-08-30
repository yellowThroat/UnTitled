#pragma once
#include "GameModel.h"

class MeshCube : public GameModel
{
public:
	MeshCube();
	~MeshCube();

	void Update() override;
	void Render() override;
	void PostRender() override;
	void ShaderFile(Shader* s = NULL)override;

private:
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	float shininess;
	

};