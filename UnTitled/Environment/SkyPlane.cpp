#include "stdafx.h"
#include "./Environment/SkyPlane.h"

SkyPlane::SkyPlane()
	: skyPlane(NULL)
	, vertexCount(0), indexCount(0)
	, vertexBuffer(NULL), indexBuffer(NULL)
	, cloudTexture1(NULL), cloudTexture2(NULL)
	, view(false)
{
	translationSpeed = 0.0001f;

	buffer = new Buffer();
	shader = new Shader(Shaders + L"999_SkyPlane.hlsl");
	world = new WorldBuffer();

	Initialize(Textures + L"cloud0.png", Textures + L"cloud2.png");
	cloudTexture3 = new Texture(Textures + L"cloud1.png");
}

SkyPlane::~SkyPlane()
{
	Shutdown();
}

bool SkyPlane::Initialize(wstring file1, wstring file2)
{
	att.skyPlaneResolution = 50;
	att.skyPlaneWidth = 10.0f;
	att.skyPlaneTop = 0.5f;
	att.skyPlanebottom = 0.0f;
	att.textureRepeat = 2;

	buffer->Data.scale = 0.3f;
	buffer->Data.brightness = 0.5f;

	buffer->Data.translation = 0.0f;

	bool result = InitializeSkyPlane(att);
	if (!result) return false;

	result = InitializeBuffers(att.skyPlaneResolution);
	if (!result) return false;

	result = LoadTextures(file1, file2);
	if (!result) return false;

	return true;
}

void SkyPlane::Shutdown()
{
	SAFE_DELETE(world);
	SAFE_DELETE(buffer);
	SAFE_DELETE(shader);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	ReleaseTextures();
	ShutdownBuffers();
	ShutdownSkyPlane();
}

void SkyPlane::Render()
{
	if(view)
	RenderBuffers();
}

void SkyPlane::Frame()
{
	buffer->Data.translation += translationSpeed;
	if (buffer->Data.translation > 1.0f)
		buffer->Data.translation -= 1.0f;
}

void SkyPlane::PostRender()
{
	ImGui::PushID(1);
	ImGui::Checkbox("view", &view);
	ImGui::PopID();
	ImGui::SliderFloat("speed0", &translationSpeed, 0, 0.005f, "%.5f");
	ImGui::SliderFloat("scale", &buffer->Data.scale, 0, 1.0f);
	ImGui::SliderFloat("brightness", &buffer->Data.brightness, 0, 2.0f);
}

int SkyPlane::GetIndexCount()
{
	return indexCount;
}

ID3D11ShaderResourceView * SkyPlane::GetCloudTexture1()
{
	return cloudTexture1->GetView();
}

ID3D11ShaderResourceView * SkyPlane::GetCloudTExture2()
{
	return cloudTexture2->GetView();
}

float SkyPlane::GetBrightness()
{
	return buffer->Data.brightness;
}

bool SkyPlane::InitializeSkyPlane(int skyPlaneResolution, float skyPlaneWidth,
	float skyPlaneTop, float skyPlaneBottom, int textureRepeat)
{
	float positionX = 0.0f;
	float positionY = 0.0f;
	float positionZ = 0.0f;
	float tu = 0.0f;
	float tv = 0.0f;

	skyPlane = new SkyPlaneType[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
	if (!skyPlane) return false;

	float quadSize = skyPlaneWidth / (float)skyPlaneResolution;

	float radius = skyPlaneWidth / 2.0f;

	float constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	float textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

	for (int j = 0; j <= skyPlaneResolution; j++)
	{
		for (int i = 0; i <= skyPlaneResolution; i++)
		{
			positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);;
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			tu = (float)i* textureDelta;
			tv = (float)j* textureDelta;

			int index = j * (skyPlaneResolution + 1) + i;

			skyPlane[index].x = positionX;
			skyPlane[index].y = positionY;
			skyPlane[index].z = positionZ;
			skyPlane[index].tu = tu;
			skyPlane[index].tv = tv;
		}
	}

	return true;
}

bool SkyPlane::InitializeSkyPlane(SkyPlaneAttribute att)
{
	return InitializeSkyPlane
	(
		att.skyPlaneResolution,
		att.skyPlaneWidth,
		att.skyPlaneTop,
		att.skyPlanebottom,
		att.textureRepeat
	);
}

void SkyPlane::ShutdownSkyPlane()
{
	SAFE_DELETE_ARRAY(skyPlane);
}

bool SkyPlane::InitializeBuffers(int skyPlaneResolution)
{

	CreateData(skyPlaneResolution);
	CreateBuffer();


	return true;
}

void SkyPlane::CreateData(int skyPlaneResolution)
{
	vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;

	indexCount = vertexCount;

	vertices = new VertexType[vertexCount];

	indices = new DWORD[indexCount];

	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;

	int index = 0;
	for (int j = 0; j < skyPlaneResolution; j++)
	{
		for (int i = 0; i < skyPlaneResolution; i++)
		{
			index1 = j * (skyPlaneResolution + 1) + i;
			index2 = j * (skyPlaneResolution + 1) + (i + 1);
			index3 = (j + 1) * (skyPlaneResolution + 1) + i;
			index4 = (j + 1) * (skyPlaneResolution + 1) + (i + 1);

			vertices[index].position = D3DXVECTOR3(skyPlane[index1].x, skyPlane[index1].y, skyPlane[index1].z);
			vertices[index].uv = D3DXVECTOR2(skyPlane[index1].tu, skyPlane[index1].tv);
			indices[index] = index++;

			vertices[index].position = D3DXVECTOR3(skyPlane[index2].x, skyPlane[index2].y, skyPlane[index2].z);
			vertices[index].uv = D3DXVECTOR2(skyPlane[index2].tu, skyPlane[index2].tv);
			indices[index] = index++;

			vertices[index].position = D3DXVECTOR3(skyPlane[index3].x, skyPlane[index3].y, skyPlane[index3].z);
			vertices[index].uv = D3DXVECTOR2(skyPlane[index3].tu, skyPlane[index3].tv);
			indices[index] = index++;

			vertices[index].position = D3DXVECTOR3(skyPlane[index3].x, skyPlane[index3].y, skyPlane[index3].z);
			vertices[index].uv = D3DXVECTOR2(skyPlane[index3].tu, skyPlane[index3].tv);
			indices[index] = index++;

			vertices[index].position = D3DXVECTOR3(skyPlane[index2].x, skyPlane[index2].y, skyPlane[index2].z);
			vertices[index].uv = D3DXVECTOR2(skyPlane[index2].tu, skyPlane[index2].tv);
			indices[index] = index++;

			vertices[index].position = D3DXVECTOR3(skyPlane[index4].x, skyPlane[index4].y, skyPlane[index4].z);
			vertices[index].uv = D3DXVECTOR2(skyPlane[index4].tu, skyPlane[index4].tv);
			indices[index] = index++;
		}
	}
}

void SkyPlane::CreateBuffer()
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	D3D::Get()->GetDevice()->CreateBuffer(&desc, &vertexData, &vertexBuffer);

	desc.ByteWidth = sizeof(DWORD) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	D3D::Get()->GetDevice()->CreateBuffer(&desc, &indexData, &indexBuffer);
}

void SkyPlane::ShutdownBuffers()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);
}
void SkyPlane::RenderBuffers()
{

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	world->SetVSBuffer(1);

	buffer->SetPSBuffer(3);

	shader->Render();

	cloudTexture1->SetShaderResource(5);
	cloudTexture1->SetShaderSampler(5);

	cloudTexture2->SetShaderResource(6);
	cloudTexture2->SetShaderSampler(6);

	cloudTexture3->SetShaderResource(7);
	cloudTexture3->SetShaderSampler(7);

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

bool SkyPlane::LoadTextures(wstring file1, wstring file2)
{
	cloudTexture1 = new Texture(file1);
	cloudTexture2 = new Texture(file2);

	return true;
}

void SkyPlane::ReleaseTextures()
{
	SAFE_DELETE(cloudTexture1);
	SAFE_DELETE(cloudTexture2);
}

void SkyPlane::SetWorld(D3DXVECTOR3 pos)
{
	D3DXMATRIX mat;
	D3DXMatrixTranslation(&mat, pos.x, pos.y, pos.z);

	world->SetMatrix(mat);
}
