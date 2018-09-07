#include "stdafx.h"
#include "Grid.h"

Grid::Grid()
	: _buffer(NULL), _indexBuffer(NULL)
	, _world(NULL), _data(NULL), _index(NULL)
{
	_world = new WorldBuffer();
	_shader = new Shader(Shaders + L"Grid.hlsl");

	D3D11_DEPTH_STENCIL_DESC desc;
	States::GetDepthStencilDesc(&desc);
	States::CreateDepthStencil(&desc, &depthStencilState[0]);

	desc.DepthEnable = false;
	States::CreateDepthStencil(&desc, &depthStencilState[1]);

	//Create Blend State
	{
		D3D11_BLEND_DESC desc;
		States::GetBlendDesc(&desc);
		States::CreateBlend(&desc, &alphaBlend[0]);

		desc.RenderTarget[0].BlendEnable = true;
		States::CreateBlend(&desc, &alphaBlend[1]);
	}

	CreateData();
	CreateBuffer();
}

Grid::~Grid()
{
	SAFE_RELEASE(_buffer);
	SAFE_RELEASE(_indexBuffer);
	SAFE_RELEASE(depthStencilState[0]);
	SAFE_RELEASE(depthStencilState[1]);
	SAFE_RELEASE(alphaBlend[0]);
	SAFE_RELEASE(alphaBlend[1]);

	SAFE_DELETE(_world);
	SAFE_DELETE(_shader);

	SAFE_DELETE_ARRAY(_data);
	SAFE_DELETE_ARRAY(_index);
}

void Grid::Render()
{
	D3D::GetDC()->OMSetDepthStencilState(depthStencilState[1], 1);
	D3D::GetDC()->OMSetBlendState(alphaBlend[1], NULL, 0xFF);

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &_buffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	_world->SetVSBuffer(1);
	_shader->Render();

	D3D::GetDC()->DrawIndexed(_indexCount, 0, 0);

	D3D::GetDC()->OMSetDepthStencilState(depthStencilState[0], 1);
	D3D::GetDC()->OMSetBlendState(alphaBlend[0], NULL, 0xFF);

}

void Grid::CreateData()
{
	int width = 400;
	int height = 400;

	_vertexCount = (width + height + 2) * 2;
	_indexCount = _vertexCount;

	_data = new VertexColor[_vertexCount];

	UINT index = 0;

	VertexColor V;
	V.color = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

	for (int i = -width / 2; i <= width /2; i++)
	{
		V.position = D3DXVECTOR3((float)i, 0, (float)height / 2);
		_data[index++] = V;
		V.position = D3DXVECTOR3((float)i, 0, -(float)height / 2);
		_data[index++] = V;

		if (i % 5 == 0)
		{
			_data[index - 2].color = D3DXCOLOR(1, 1, 1, 1);
			_data[index - 1].color = D3DXCOLOR(1, 1, 1, 1);
		}

		if (i == 0)
		{
			_data[index - 2].color = D3DXCOLOR(1, 0, 0, 1);
			_data[index - 1].color = D3DXCOLOR(1, 0, 0, 1);
		}
	}

	for (int i = -height / 2; i <= height / 2; i++)
	{
		V.position = D3DXVECTOR3(-(float)width /2, 0, (float)i);
		_data[index++] = V;
		V.position = D3DXVECTOR3((float)width /2, 0, (float)i);
		_data[index++] = V;

		if (i % 5 == 0)
		{
			_data[index - 2].color = D3DXCOLOR(1, 1, 1, 1);
			_data[index - 1].color = D3DXCOLOR(1, 1, 1, 1);
		}

		if (i == 0)
		{
			_data[index - 2].color = D3DXCOLOR(0, 0, 1, 1);
			_data[index - 1].color = D3DXCOLOR(0, 0, 1, 1);
		}
	}

	_index = new UINT[_indexCount];

	for (UINT i = 0; i < _indexCount; i++)
		_index[i] = i;
}

void Grid::CreateBuffer()
{
	// 버텍스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * _vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = _data;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &_buffer);
		assert(SUCCEEDED(hr));
	}
	// 인덱스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * _indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = _index;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &_indexBuffer);
		assert(SUCCEEDED(hr));
	}
}
