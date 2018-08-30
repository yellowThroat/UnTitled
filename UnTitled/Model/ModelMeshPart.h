#pragma once
#include "../Interfaces/ICloneable.h"

class ModelMeshPart : public ICloneable
{
public:
	friend class Model;
	friend class Models;
	friend class ModelMesh;

private:
	ModelMeshPart();
	~ModelMeshPart();

	void Binding();
	void Render();

public:
	void Clone(void** clone);

private:
	class ModelMesh* parent;

	wstring materialName;
	Material* material;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	vector<ModelVertexType> vertices;
	vector<UINT> indices;
};