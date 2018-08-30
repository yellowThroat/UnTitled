#pragma once
#include "GameModel.h"

class MeshSphere : public GameModel
{
public:
	MeshSphere();
	~MeshSphere();

	void Update() override;
	void Render() override;
	void PostRender() override;
	void ShaderFile(Shader* s = NULL) override;
private:
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	float shininess;
	

};