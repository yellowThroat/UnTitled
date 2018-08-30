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
		D3DXMATRIX GetWorld() { return _world; }
		void SetWorld(D3DXMATRIX mat);
		void SetPosition(D3DXVECTOR3 position)
		{
			_position = position;
		}
		void SetValues(ExecuteValues* val) { _values = val; }
		
	protected:
		virtual void MakeShape() {}
		void MakeBuffer();

	protected:
		D3DXMATRIX		_world;
		WorldBuffer*	_worldBuffer;
		VertexColor*	_data;
		UINT*			_indexData;
		ID3D11Buffer*	_buffer;
		ID3D11Buffer*	_indexBuffer;
		Shader*			_shader;
		UINT			_vertexCount;
		UINT			_indexCount;
		UINT			_lineCount;

		D3DXVECTOR3		_position;
		float			_rotY;
		ExecuteValues*	_values;
	};
}
