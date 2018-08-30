#include "stdafx.h"
#include "Fog.h"

Fog::Fog()
{
	buffer = new Buffer();
}

Fog::~Fog()
{
	SAFE_DELETE(buffer);
}

void Fog::Update()
{
}

void Fog::Render()
{
	ImGui::ColorEdit4("Color", buffer->Data.Color);
	ImGui::SliderFloat("Density", &buffer->Data.Density, 0.001f, 0.05f);

	buffer->SetPSBuffer(12);
}
