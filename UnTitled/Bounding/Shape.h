#pragma once
struct ExecuteValues;

namespace Shapes
{
	class Shape
	{
	public:
		Shape();
		virtual ~Shape();

		virtual void Update();
		virtual void Render();
		void Clear();
		D3DXVECTOR3 GetPosition() { return _position; }
		D3DXMATRIX GetWorld() { return _world; }
		void SetWorld(D3DXMATRIX mat);
		void SetValues(ExecuteValues* val) { _values = val; }
		void SetColor(D3DXCOLOR color);
		virtual float GetRadius() { return 0.0f; }

	protected:
		virtual void MakeShape() {}
		void MakeBuffer();
		void ChangeData();

	protected:
		class Buffer : public ShaderBuffer
		{
		public:
			Buffer()
				: ShaderBuffer(&Data, sizeof(Struct))
			{
				Data.color = D3DXCOLOR(0, 1, 0, 1);
			}
			
			struct Struct
			{
				D3DXCOLOR color;
			}Data;
		};

	protected:
		D3DXMATRIX		_world;
		WorldBuffer*	_worldBuffer;
		Vertex*			_data;
		UINT*			_indexData;
		ID3D11Buffer*	_buffer;
		ID3D11Buffer*	_indexBuffer;
		Shader*			_shader;
		Buffer*			_color;
		UINT			_vertexCount;
		UINT			_indexCount;
		UINT			_lineCount;

		D3DXVECTOR3		_position;
		float			_rotY;
		ExecuteValues*	_values;
	};

	class BoundingBox
	{
	public:
		BoundingBox();
		~BoundingBox();

		UINT index;
		Shape* box;
		class Model* model;
	};
}
