#include "stdafx.h"
#include "BillBoard.h"

Shader* BillBoard::shader = NULL;
int BillBoard::reference = 0;
unordered_map<wstring, Texture*> BillBoard::textures;

BillBoard::BillBoard(D3DXVECTOR3 position, int instanceCount, float radius)
	: vertexBuffer(NULL), instanceBuffer(NULL)
	, layout(NULL)
	, position(position), instanceCount(instanceCount)
	, radius(radius)
	, texture(NULL)
{
	if (!shader)
		shader = new Shader(Shaders + L"BillBoard.hlsl");
	Create();

	reference++;
}

BillBoard::~BillBoard()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(instanceBuffer);

	
	if (!--reference)
	{
		SAFE_DELETE(shader);
		for (auto t : textures)
			SAFE_DELETE(t.second);
		textures.clear();
	}
}

void BillBoard::Update()
{
}

void BillBoard::Render()
{
	unsigned int strides[2] = { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offsets[2] = { 0,0 };

	ID3D11Buffer* bufferPointers[2] = { vertexBuffer, instanceBuffer };

	D3D::GetDC()->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	texture->SetShaderResource(10);
	texture->SetShaderSampler(10);

	D3D::GetDC()->IASetInputLayout(layout);
	D3D::GetDC()->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void BillBoard::SetTexture(wstring file)
{
	if (textures.count(file) < 1)
		textures[file] = new Texture(file);

	texture = textures[file];
}

void BillBoard::AddTexture(wstring file)
{
	if (file == L"") return;

	if (textures.count(file) < 1)
		textures[file] = new Texture(file);
}

void BillBoard::Create()
{
	vertexCount = 6;
	VertexType* V = new VertexType[vertexCount];
	V[0].uv = D3DXVECTOR2(0, 1);
	V[1].uv = D3DXVECTOR2(0, 0);
	V[2].uv = D3DXVECTOR2(1, 1);
	V[3].uv = D3DXVECTOR2(1, 1);
	V[4].uv = D3DXVECTOR2(0, 0);
	V[5].uv = D3DXVECTOR2(1, 0);

	for (int i = 0; i < 6; i++)
	{
		D3DXVECTOR2 temp;
		temp.x = V[i].uv.x - 0.5f;
		temp.y = 1.0f - V[i].uv.y;

		V[i].position = D3DXVECTOR3(0, 0, 0);
		V[i].position.x += temp.x;
		V[i].position.y += temp.y;
	}

	InstanceType* instances = new InstanceType[instanceCount];

	for (int i = 0; i < instanceCount; i++)
	{
		instances[i].position = position;

		if (i == 0) continue;
		while (true)
		{
			float ranX = Math::Random(-radius, radius);
			float ranZ = Math::Random(-radius, radius);

			if (ranX * ranX + ranZ * ranZ > radius * radius) continue;

			instances[i].position.x += ranX;
			instances[i].position.z += ranZ;

			break;
		}
	}

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = V;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));
	
	desc.ByteWidth = sizeof(InstanceType) * instanceCount;
	
	data.pSysMem = instances;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &instanceBuffer);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(V);
	SAFE_DELETE_ARRAY(instances);

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 1;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 1;
	polygonLayout[2].AlignedByteOffset = 0;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[2].InstanceDataStepRate = 1;

	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	D3D::GetDevice()->CreateInputLayout(
		polygonLayout,
		numElements,
		shader->GetVertexBlob()->GetBufferPointer(),
		shader->GetVertexBlob()->GetBufferSize(), 
		&layout
	);

}