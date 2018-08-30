#pragma once

class Fog
{
public:
	Fog();
	~Fog();

	void Update();
	void Render();
private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer()
			: ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.Color = D3DXCOLOR(0.75f, 0.75f, 0.75f, 1);
			Data.Density = 0.05f;
		}

		struct Struct
		{
			D3DXCOLOR Color;
			float Density;
			float Padding[3];
		}Data;
	};

	Buffer* buffer;
	
};