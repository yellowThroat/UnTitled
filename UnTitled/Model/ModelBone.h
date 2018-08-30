#pragma once
#include "../Interfaces/ICloneable.h"

class ModelBone : public ICloneable
{
public:
	friend class Model;
	friend class Models;

private:
	ModelBone();
	~ModelBone();

public:
	void Clone(void** clone);

	int Index() { return index; }
	int ParentIndex() { return parentIndex; }

	wstring Name() { return name; }

	ModelBone* Parent() { return parent; }

	void Transform(D3DXMATRIX& transform)
	{
		this->transform = transform;
	}

	void AbsoluteTransform(D3DXMATRIX& transform)
	{
		this->absoluteTransform = transform;
	}

	D3DXMATRIX& Transform()
	{
		return transform;
	}

	D3DXMATRIX& AbsoluteTransform()
	{
		return absoluteTransform;
	}

	UINT ChildCount() { return childs.size(); }
	ModelBone* Child(UINT index) { return childs[index]; }

private:
	int	index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	vector<ModelBone *> childs;

	D3DXMATRIX transform;
	D3DXMATRIX absoluteTransform;
};