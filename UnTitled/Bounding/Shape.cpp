#include "stdafx.h"
#include "Shape.h"

Shapes::Shape::Shape()
	: _rotY(0.0f)
{
	_shader = new Shader(Shaders + L"Shape.hlsl");
	_worldBuffer = new WorldBuffer();
	_color = new Buffer();
	D3DXMatrixIdentity(&_world);
	SetWorld(_world);
}

Shapes::Shape::~Shape()
{
	SAFE_DELETE(_worldBuffer);
	SAFE_DELETE(_shader);
	SAFE_DELETE(_color);
	Clear();
}

void Shapes::Shape::Update()
{
}

void Shapes::Shape::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &_buffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	_worldBuffer->SetVSBuffer(1);
	_shader->Render();
	_color->SetPSBuffer(5);

	D3D::GetDC()->DrawIndexed(_indexCount, 0, 0);
}

void Shapes::Shape::Clear()
{
	SAFE_DELETE_ARRAY(_data);
	SAFE_DELETE_ARRAY(_indexData);

	SAFE_RELEASE(_buffer);
	SAFE_RELEASE(_indexBuffer);
}

void Shapes::Shape::SetWorld(D3DXMATRIX mat)
{
	_worldBuffer->SetMatrix(mat);
	_world = mat;
	_position.x = _world._41;
	_position.y = _world._42;
	_position.z = _world._43;
}

void Shapes::Shape::SetColor(D3DXCOLOR color)
{
	_color->Data.color = color;
}

void Shapes::Shape::MakeBuffer()
{
	// 버텍스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex) * _vertexCount;
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
		data.pSysMem = _indexData;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &_indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Shapes::Shape::ChangeData()
{
	D3D::GetDC()->UpdateSubresource
	(
		_buffer, 0, NULL, _data, sizeof(Vertex) * _vertexCount, 0
	);
}

Shapes::BoundingBox::BoundingBox()
	: index(0)
	, box(NULL)
	, model(NULL)
{
}

Shapes::BoundingBox::~BoundingBox()
{
	SAFE_DELETE(box);
	SAFE_DELETE(model);
}
