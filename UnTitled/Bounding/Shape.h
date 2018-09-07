#pragma once
struct ExecuteValues;

namespace Shapes
{
	class Sphere;
	class Box;
	class Frustum;
	class Capsule;
	class Cone;

	enum class ContainmentType
	{
		Disjoint, Contains, Intersects
	};

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

		virtual bool Collide(Shape* shape) = 0;
		virtual bool Collide(Box* shape) { return false; }
		virtual bool Collide(Capsule* shape) { return false; }
		virtual bool Collide(Cone* shape) { return false; }
		virtual bool Collide(Frustum* shape) { return false; }
		virtual bool Collide(Sphere* shape) { return false; }

		virtual ContainmentType Contains(Shape * shape) = 0;
		virtual ContainmentType Contains(Box* box) { return ContainmentType::Disjoint; }
		virtual ContainmentType Contains(Sphere* box) { return ContainmentType::Disjoint; }
		virtual ContainmentType Contains(Frustum* box) { return ContainmentType::Disjoint; }
		virtual ContainmentType Contains(Capsule* box) { return ContainmentType::Disjoint; }

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
		bool valid;		
	};
}
