#include "stdafx.h"
#include "Ocean.h"

Ocean::Ocean(ExecuteValues * values, UINT w, UINT h, float thick)
	: values(values), thick(thick), bView(false)
{
	heightMap = new Texture(Contents + L"landscape/HeightMap256.png");
	worldBuffer = new WorldBuffer();
	
	material = new Material(Shaders + L"998_Ocean.hlsl");
	material->SetDiffuse(D3DXCOLOR(0.600f, 0.721f, 0.937f, 0.627f));
	material->SetAmbient(D3DXCOLOR(0.337f, 0.462f, 0.490f, 1.0f));
	material->SetNormalMap(Contents + L"Landscape/WaveNormal.png");
	material->SetShininess(3.0f);

	width = (UINT)(w / thick);
	height = (UINT)(h / thick);

	UINT count = (width + 1) * (height + 1);
	vertices.assign(count, OceanVertexType());

	UINT index = 0;
	for (UINT y = 0; y <= height; y++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			vertices[index].position.x = (float)x * thick;
			vertices[index].position.y = 0.0f;
			vertices[index].position.z = (float)y * thick;

			vertices[index].uv.x = (float)x / (float)width;
			vertices[index].uv.y = (float)y / (float)width;

			index++;
		}
	}

	UINT indexCount = width * height * 6;
	indices.assign(indexCount, UINT());

	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			indices[index + 0] = (width + 1) * z + x;
			indices[index + 1] = (width + 1) * (z + 1) + x;
			indices[index + 2] = (width + 1) * z + x + 1;
			indices[index + 3] = (width + 1) * z + x + 1;
			indices[index + 4] = (width + 1) * (z + 1) + x;
			indices[index + 5] = (width + 1) * (z + 1) + (x + 1);

			index += 6;
		}
	}

	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(OceanVertexType) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc = { 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &rasterizer[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &rasterizer[1]);
	}

	{
		D3D11_BLEND_DESC desc;
		States::GetBlendDesc(&desc);
		States::CreateBlend(&desc, &blend[0]);

		desc.RenderTarget[0].BlendEnable = true;
		States::CreateBlend(&desc, &blend[1]);
	}

	
	vsBuffer = new VsBuffer();
	
	psBuffer = new PsBuffer();
	psBuffer->Data.OceanSize = D3DXVECTOR2(width * thick, height * thick);
}

Ocean::~Ocean()
{
	SAFE_DELETE(material);
	SAFE_DELETE(worldBuffer);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_RELEASE(rasterizer[0]);
	SAFE_RELEASE(rasterizer[1]);
}

void Ocean::Update()
{
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, 0, 3, 0);
	worldBuffer->SetMatrix(T);
}

void Ocean::Render()
{
	if (!bView) return;
	vsBuffer->SetVSBuffer(12);
	psBuffer->SetPSBuffer(2);

	worldBuffer->SetVSBuffer(1);
	
	material->SetBuffer();

	heightMap->SetShaderResource(10);
	heightMap->SetShaderSampler(10);


	if (bWireframe == true)
		D3D::GetDC()->RSSetState(rasterizer[1]);

	D3D::GetDC()->OMSetBlendState(blend[1], NULL, 0xFF);

	
	UINT stride = sizeof(OceanVertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->DrawIndexed(indices.size(), 0, 0);


	D3D::GetDC()->RSSetState(rasterizer[0]);
	D3D::GetDC()->OMSetBlendState(blend[0], NULL, 0xFF);
}

void Ocean::PostRender()
{
	ImGui::Checkbox("View", &bView);
	ImGui::SliderFloat("frequency", &vsBuffer->Data.WaveFrequency, 0.01f, 0.1f);
	ImGui::SliderFloat("amplitude", &vsBuffer->Data.WaveAmplitude, 0.5f, 3.0f);
	ImGui::SliderFloat2("TextureScale", vsBuffer->Data.TextureScale, 10, 20);
	ImGui::SliderFloat2("BumpSpeed", vsBuffer->Data.BumpSpeed, 0.0f, 0.1f);
	ImGui::SliderFloat("BumpHeight", &vsBuffer->Data.BumpHeight, 0.01f, 1.5f);
	ImGui::Separator();
	ImGui::ColorEdit4("Ambient", (float *)material->GetAmbient());
	ImGui::Separator();
	ImGui::ColorEdit3("shallow", psBuffer->Data.ShallowColor);
	ImGui::ColorEdit3("deep", psBuffer->Data.DeepColor);
	ImGui::SliderFloat("fresnelbias", &psBuffer->Data.FresnelBias, 0.5f, 3.0f);
	ImGui::SliderFloat("fresnel power", &psBuffer->Data.FresnelPower, 0.1f, 3.0f);
	ImGui::SliderFloat("fresnel amount", &psBuffer->Data.FresnelAmount, 0.1f, 3.0f);
	if (ImGui::SliderFloat("specular amount", &psBuffer->Data.SpecularAmount, 0.1f, 5.0f))
		material->SetShininess(psBuffer->Data.SpecularAmount);
	ImGui::SliderFloat("shoreblend", &psBuffer->Data.ShoreBlend, 10.0f, 100.0f);
}
