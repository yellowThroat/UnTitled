#include "stdafx.h"
#include "ModelBone.h"

ModelBone::ModelBone()
	: parent(NULL)
{
}

ModelBone::~ModelBone()
{
}

void ModelBone::Clone(void ** clone)
{
	ModelBone* bone = new ModelBone();
	bone->index = this->index;
	bone->name = this->name;
	bone->transform = this->transform;
	bone->absoluteTransform = this->absoluteTransform;
	bone->parentIndex = this->parentIndex;

	*clone = bone;
}
