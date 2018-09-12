#pragma once

class DirectionalLight
{
public:
	DirectionalLight(ExecuteValues* val);
	~DirectionalLight();

	void AddObject(class GameRender* object);

	RenderTarget* Target() { return target; }
	void Position(D3DXVECTOR3* position) { this->position = position; }
	D3DXMATRIX View() { return view; }
	D3DXMATRIX Projection() { return projection; }
	ID3D11ShaderResourceView* SRV()
	{
		return target->GetSRV();
	}

	void Update();
	void Render();
	void EraseObject(UINT num);
	void Clear();

private:
	ExecuteValues* values;
	
	Shader* shader;
	
	vector<class GameRender*> objects;
	
	RenderTarget* target;

	class FixedCamera* light;
	D3DXMATRIX view;
	D3DXMATRIX projection;
	D3DXVECTOR3* position;
};