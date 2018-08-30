#include "stdafx.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget()
	: texture(NULL), rtv(NULL), srv(NULL)
{
	Create();
}

RenderTarget::~RenderTarget()
{
	Delete();
}

void RenderTarget::Set(D3DXCOLOR color)
{
	D3D::Get()->SetRenderTarget(rtv);
	D3D::Get()->Clear(color, rtv);
}

void RenderTarget::Create(UINT width, UINT height)
{
	Delete();

	
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (width < 1)
		this->width = (UINT)desc.Width;
	else
		this->width = width;
	
	if (height < 1)
		this->height = (UINT)desc.Height;
	else
		this->height = height;


	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = this->width;
	textureDesc.Height = this->height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture);
	assert(SUCCEEDED(hr));


	D3D11_RENDER_TARGET_VIEW_DESC renderDesc;
	ZeroMemory(&renderDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderDesc.Format = textureDesc.Format;
	renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderDesc.Texture2D.MipSlice = 0;

	hr = D3D::GetDevice()->CreateRenderTargetView(texture, &renderDesc, &rtv);
	assert(SUCCEEDED(hr));


	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	viewDesc.Format = textureDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;

	hr = D3D::GetDevice()->CreateShaderResourceView(texture, &viewDesc, &srv);
	assert(SUCCEEDED(hr));
}

void RenderTarget::Delete()
{
	SAFE_RELEASE(srv);
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(texture);
}
