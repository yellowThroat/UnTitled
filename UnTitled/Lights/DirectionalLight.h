#pragma once

class DirectionalLight
{
public:
	DirectionalLight(ExecuteValues* val);
	~DirectionalLight();

	void AddObject(class GameRender* object);

	RenderTarget* Target() { return target; }
	D3DXMATRIX View() { return view; }
	D3DXMATRIX Projection() { return projection; }
	ID3D11ShaderResourceView* SRV()
	{
		return target->GetSRV();
	}

	void Update();
	void Render();

private:
	ExecuteValues* values;
	
	Shader* shader;
	
	vector<class GameRender*> objects;
	
	RenderTarget* target;

	class FixedCamera* light;
	D3DXMATRIX view;
	D3DXMATRIX projection;
};