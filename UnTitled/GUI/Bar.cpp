#include "stdafx.h"
#include "Bar.h"

Bar::Bar(wstring bottom, wstring top, D3DXVECTOR3* target)
	: shader(NULL), world(NULL)
	, data(NULL), indexData(NULL)
	, buffer(NULL), indexBuffer(NULL)
	, vsBuffer(NULL)
	, bottom(NULL), top(NULL)
	, target(target)
	, maxRatio(1.0f)
{
	world = new WorldBuffer();
	vsBuffer = new Buffer();
	shader = new Shader(Shaders + L"hpBar.hlsl");
	CreateVertex();
	CreateBuffer();
	SetTexture(bottom, top);

	D3D11_BLEND_DESC desc;
	States::GetBlendDesc(&desc);
	States::CreateBlend(&desc, &alphaBlend[0]);

	desc.RenderTarget[0].BlendEnable = true;
	States::CreateBlend(&desc, &alphaBlend[1]);
}

Bar::~Bar()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(world);
	SAFE_DELETE(bottom);
	SAFE_DELETE(top);

	SAFE_RELEASE(buffer);
	SAFE_RELEASE(indexBuffer);
	
	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(indexData);
}

void Bar::Update()
{
	if (target)
		vsBuffer->Data.Position = *target;
	vsBuffer->Data.Ratio -= Time::Delta();
	if (vsBuffer->Data.Ratio <= maxRatio) vsBuffer->Data.Ratio = maxRatio;
}

void Bar::Render()
{
	UINT stride = sizeof(VertexBar);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bottom->SetShaderResource(9);
	top->SetShaderResource(10);

	world->SetVSBuffer(1);
	shader->Render();
	vsBuffer->SetVSBuffer(9);

	D3D::GetDC()->OMSetBlendState(alphaBlend[1], NULL, 0xFF);

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

	D3D::GetDC()->OMSetBlendState(alphaBlend[0], NULL, 0xFF);

}

void Bar::SetTarget(D3DXVECTOR3 * target)
{
	this->target = target;
}

void Bar::SetRatio(float val)
{
	//vsBuffer->Data.Ratio = val;
	maxRatio = val;
}

void Bar::CreateVertex()
{
	vertexCount = 12;
	indexCount = vertexCount;

	data = new VertexBar[vertexCount];

	VertexBar V;
	V.position = D3DXVECTOR3(0, 0, 0);
	V.uv = D3DXVECTOR2(0, 0);
	V.isBottom = 1;
	for (UINT i = 0; i < vertexCount; i++)
	{
		if (i == 6) V.isBottom = 0;

		data[i] = V;
	}

	data[0].uv = D3DXVECTOR2(0, 1);
	data[1].uv = D3DXVECTOR2(0, 0);
	data[2].uv = D3DXVECTOR2(1, 1);
	data[3].uv = D3DXVECTOR2(1, 1);
	data[4].uv = D3DXVECTOR2(0, 0);
	data[5].uv = D3DXVECTOR2(1, 0);

	data[6].uv = D3DXVECTOR2(0, 1);
	data[7].uv = D3DXVECTOR2(0, 0);
	data[8].uv = D3DXVECTOR2(1, 1);
	data[9].uv = D3DXVECTOR2(1, 1);
	data[10].uv = D3DXVECTOR2(0, 0);
	data[11].uv = D3DXVECTOR2(1, 0);

	indexData = new UINT[indexCount];
	for (UINT i = 0; i < indexCount; i++)
		indexData[i] = i;
}

void Bar::CreateBuffer()
{
	// 버텍스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexBar) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA resource_data;
		ZeroMemory(&resource_data, sizeof(D3D11_SUBRESOURCE_DATA));
		resource_data.pSysMem = data;

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
		resource_data.pSysMem = indexData;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &resource_data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

}

void Bar::SetTexture(wstring bottom, wstring top)
{
	SAFE_DELETE(this->bottom);
	SAFE_DELETE(this->top);

	this->bottom = new Texture(bottom);
	this->top = new Texture(top);
}
