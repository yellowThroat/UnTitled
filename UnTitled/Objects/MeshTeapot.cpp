#include "stdafx.h"
#include "MeshTeapot.h"

MeshTeapot::MeshTeapot()
	: GameModel(Models + L"Meshes/Teapot.material", Models + L"Meshes/Teapot.mesh")
{
	oShader = new Shader(Shaders + L"999_Idle.hlsl");

	ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1);
	diffuse = D3DXCOLOR(0, 0, 0, 1);
	specular = D3DXCOLOR(0, 0, 0, 1);
	shininess = 15.0f;

	for (Material* material : model->Materials())
	{
		material->SetShader(oShader);
	}

	name = "Teapot";
	strcpy_s(editBuf.name, name.c_str());
}

MeshTeapot::~MeshTeapot()
{
}

void MeshTeapot::Update()
{
	__super::Update();

	for (Material* material : model->Materials())
	{
		material->SetAmbient(ambient);
		material->SetDiffuse(diffuse);
		material->SetSpecular(specular);
		material->SetShininess(shininess);
	}
}

void MeshTeapot::Render()
{
	isBoneBuffer->SetVSBuffer(4);

	__super::Render();
}

void MeshTeapot::PostRender()
{
	GameRender::PostRender();

	ImGui::Begin("Inspector");

	if (ImGui::CollapsingHeader("Color"))
	{
		ImGui::SliderFloat3("Ambient", ambient, 0, 1);
		ImGui::SliderFloat3("Diffuse", diffuse, 0, 1);
		ImGui::SliderFloat3("Specular", specular, 0, 1);
		ImGui::SliderFloat("Shininess", &shininess, 0.1f, 30);
	}

	ImGui::End();
}

void MeshTeapot::ShaderFile(Shader* s)
{
	if (!s)
	{
		for (Material* material : model->Materials())
			material->SetShader(oShader);

		return;
	}

	for (Material* material : model->Materials())
		material->SetShader(s);
}
