#include "stdafx.h"
#include "WeaponTrail.h"

WeaponTrail::WeaponTrail(UINT size, UINT length)
	: size(size)
	, interval(0.0f)
	, elapsed(0.0f)
{
	shader = new Shader(Shaders + L"WeaponTrail.hlsl");
	texture = new Texture(Images + L"fire.png");
	vsBuffer = new Buffer(size, length);

	D3D11_BLEND_DESC desc;
	States::GetBlendDesc(&desc);
	States::CreateBlend(&desc, &alphaBlend[0]);

	desc.RenderTarget[0].BlendEnable = true;
	States::CreateBlend(&desc, &alphaBlend[1]);

	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &rasterizer[0]);

		desc.CullMode = D3D11_CULL_NONE;
		States::CreateRasterizer(&desc, &rasterizer[1]);
	}

	CreateVertex();
	CreateBuffer();
}

WeaponTrail::~WeaponTrail()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(texture);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(buffer);
	SAFE_RELEASE(indexBuffer);
}

void WeaponTrail::Update()
{
	elapsed -= Time::Delta();

	if (elapsed < 0) elapsed = 0.0f;
}

void WeaponTrail::Render()
{
	if (ImGui::SliderInt("Weapon Trail Size", (int*)&vsBuffer->Data.size, 2, 100))
	{
		Resize(vsBuffer->Data.size, vsBuffer->Data.length);
	}

	if (ImGui::SliderInt("Weapon Trail Length", (int*)&vsBuffer->Data.length, 2, 100))
	{
		Resize(vsBuffer->Data.size, vsBuffer->Data.length);
	}
	UINT stride = sizeof(VertexTrail);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	texture->SetShaderResource(9);
	shader->Render();
	vsBuffer->SetVSBuffer(9);

	D3D::GetDC()->RSSetState(rasterizer[1]);
	D3D::GetDC()->OMSetBlendState(alphaBlend[1], NULL, 0xFF);

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

	D3D::GetDC()->RSSetState(rasterizer[0]);
	D3D::GetDC()->OMSetBlendState(alphaBlend[0], NULL, 0xFF);
}

void WeaponTrail::CreateVertex()
{
	vertexCount = size * 2;

	vertices = new VertexTrail[vertexCount];
	
	VertexTrail V;
	UINT index = 0;
	for (UINT i = 0; i < size; i++)
	{
		V.order = i;
		V.uv.x = (float)i / (float)(size - 1);
		V.uv.y = 0.0f;
		vertices[index++] = V;
		
		V.uv.y = 1.0f;
		vertices[index++] = V;
	}

	index = 0;
	indexCount = (size -1) * 6;
	indices = new UINT[indexCount];
	for (UINT i = 0; i < size - 1; i++)
	{
		indices[index++] = i * 2 + 1;
		indices[index++] = i * 2 + 0;
		indices[index++] = i * 2 + 3;
		indices[index++] = i * 2 + 3;
		indices[index++] = i * 2 + 0;
		indices[index++] = i * 2 + 2;
	}

}

void WeaponTrail::CreateBuffer()
{
	// 버텍스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTrail) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA resource_data;
		ZeroMemory(&resource_data, sizeof(D3D11_SUBRESOURCE_DATA));
		resource_data.pSysMem = vertices;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &resource_data, &buffer);
		assert(SUCCEEDED(hr));
	}
	// 인덱스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA resource_data;
		ZeroMemory(&resource_data, sizeof(D3D11_SUBRESOURCE_DATA));
		resource_data.pSysMem = indices;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &resource_data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

}

void WeaponTrail::Resize(UINT size, UINT length)
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
	SAFE_RELEASE(buffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_DELETE(vsBuffer);

	this->size = size;
	vsBuffer = new Buffer(size, length);

	CreateVertex();
	CreateBuffer();
}

void WeaponTrail::SetData(D3DXVECTOR3 bottom, D3DXVECTOR3 top)
{
	vsBuffer->Data.start--;
	if (vsBuffer->Data.start < 0) vsBuffer->Data.start = vsBuffer->Data.length - 1;

	vsBuffer->Data.Bottom[vsBuffer->Data.start].x = bottom.x;
	vsBuffer->Data.Bottom[vsBuffer->Data.start].y = bottom.y;
	vsBuffer->Data.Bottom[vsBuffer->Data.start].z = bottom.z;

	vsBuffer->Data.top[vsBuffer->Data.start].x = top.x;
	vsBuffer->Data.top[vsBuffer->Data.start].y = top.y;
	vsBuffer->Data.top[vsBuffer->Data.start].z = top.z;
}
