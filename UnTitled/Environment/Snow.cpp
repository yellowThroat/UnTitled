#include "stdafx.h"
#include "Snow.h"

Snow::Snow(WeatherCube & cube, UINT count, ExecuteValues* val)
	: particleCount(count)
	, buffer(NULL), values(val)
{
	SnowMap = new Texture(Textures + L"snow.png");
	shader = new Shader(Shaders + L"998_Snow.hlsl");
	world = new WorldBuffer();


	vertices.assign(count * 4, VertexSnow());
	indices.assign(count * 6, UINT());

	//Create Vertex
	{
		UINT index = 0;
		while (index < particleCount * 4)
		{
			D3DXVECTOR2 scale;
			scale.x = Math::Random(0.5f, 2.0f);
			scale.y = Math::Random(0.5f, 2.0f);

			D3DXVECTOR3 position;
			position.x = Math::Random(0.0f, 1.0f);
			position.y = Math::Random(0.0f, 1.0f);
			position.z = Math::Random(0.0f, 1.0f);
			
			D3DXVECTOR2 random;
			random.x = Math::Random(0.0f, 1000.0f);
			random.y = Math::Random(0.0f, 1000.0f);

			vertices[index++] = VertexSnow(position, D3DXVECTOR2(0, 1), random, scale);
			vertices[index++] = VertexSnow(position, D3DXVECTOR2(0, 0), random, scale);
			vertices[index++] = VertexSnow(position, D3DXVECTOR2(1, 1), random, scale);
			vertices[index++] = VertexSnow(position, D3DXVECTOR2(1, 0), random, scale);
		}//while(
	}

	for (UINT i = 0; i < particleCount; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexSnow) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	buffer = new Buffer();
	buffer->Data.Size = cube.Size;
	buffer->Data.Origin = cube.Origin;
	buffer->Data.Color = D3DXCOLOR(1, 1, 1, 1);

	//Create Blend State
	{
		D3D11_BLEND_DESC desc;
		States::GetBlendDesc(&desc);
		States::CreateBlend(&desc, &alphaBlend[0]);

		desc.RenderTarget[0].BlendEnable = true;
		States::CreateBlend(&desc, &alphaBlend[1]);
	}
}

Snow::~Snow()
{
	SAFE_DELETE(buffer);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(SnowMap);
	SAFE_DELETE(shader);

	SAFE_DELETE(world);
}

void Snow::Render()
{
	buffer->SetVSBuffer(2);

	UINT stride = sizeof(VertexSnow);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SnowMap->SetShaderResource(5);
	SnowMap->SetShaderSampler(5);

	shader->Render();

	world->SetVSBuffer(1);
	D3D::GetDC()->OMSetBlendState(alphaBlend[1], NULL, 0xFF);

	D3D::GetDC()->DrawIndexed(indices.size(), 0, 0);

	D3D::GetDC()->OMSetBlendState(alphaBlend[0], NULL, 0xFF);

}

void Snow::PostRender()
{
	ImGui::SliderFloat("Velocity X", &buffer->Data.Velocity.x, -20, 20);
	ImGui::SliderFloat("Velocity Y", &buffer->Data.Velocity.y, -200, 0);
	ImGui::SliderFloat("Velocity Z", &buffer->Data.Velocity.z, -20, 20);
	ImGui::SliderFloat("Distance", &buffer->Data.DrawDistance, 10, 1500);
	ImGui::ColorEdit3("Color", buffer->Data.Color);
	ImGui::SliderFloat("Width", &buffer->Data.Size.x, 10, 256);
	ImGui::SliderFloat("Height", &buffer->Data.Size.y, 10, 256);
	ImGui::SliderFloat("Length", &buffer->Data.Size.z, 10, 256);
	ImGui::SliderFloat("Turbulence", &buffer->Data.Turbulence, 1.0f, 30.0f);
}

void Snow::Velocity(D3DXVECTOR3 & vec)
{
	buffer->Data.Velocity = vec;
}
