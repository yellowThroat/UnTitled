#pragma once

class Reflection
{
public:
	Reflection(ExecuteValues* values);
	~Reflection();

	void Update();

	void PreRender();
	void Render();

	void Add(class GameRender* object);

private:
	ExecuteValues* values;

	Shader* shader;
	class Mirror* mirror;

	RenderTarget* target;
	vector<class GameRender *> objects;

	class GameModel* plane;

	ID3D11SamplerState* sampler;

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			D3DXMatrixIdentity(&Data.Reflection);
		}

		void Matrix(D3DXMATRIX& matrix)
		{
			Data.Reflection = matrix;

			D3DXMatrixTranspose(&Data.Reflection, &Data.Reflection);
		}

	private:
		struct Struct
		{
			D3DXMATRIX Reflection;
		} Data;
	};

	Buffer* buffer;
};