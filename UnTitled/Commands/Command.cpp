#include "stdafx.h"
#include "Command.h"
#include "../Objects/GameRender.h"
#include "../Landscape/Terrain.h"
#include "../Landscape/Data.h"

cString * cString::Create(GameRender* target, string edit)
{
	cString * pCommand = new cString();
	pCommand->target = target;
	pCommand->edit = edit;
	return pCommand;
}

void cString::Undo()
{
	string temp = target->Name();
	target->Name(edit);
	edit = temp;
}

void cString::Redo()
{
	string temp = target->Name();
	target->Name(edit);
	edit = temp;
}

cPosition * cPosition::Create(GameRender * target, D3DXVECTOR3 position)
{
	cPosition* pCommand = new cPosition();
	pCommand->target = target;
	pCommand->position = position;

	return pCommand;
}

void cPosition::Undo()
{
	D3DXVECTOR3 v = target->Position();
	target->Position(position);
	position = v;
}

void cPosition::Redo()
{
	D3DXVECTOR3 v = target->Position();
	target->Position(position);
	position = v;
}

cRotate * cRotate::Create(GameRender * target, D3DXVECTOR3 rotate)
{
	cRotate* pCommand = new cRotate();
	pCommand->target = target;
	pCommand->rotate = rotate;

	return pCommand;
}

void cRotate::Undo()
{
	D3DXVECTOR3 v = target->Rotation();
	target->Rotation(rotate);
	rotate = v;
}

void cRotate::Redo()
{
	D3DXVECTOR3 v = target->Rotation();
	target->Rotation(rotate);
	rotate = v;
}

cScale * cScale::Create(GameRender * target, D3DXVECTOR3 scale)
{
	cScale* pCommand = new cScale();
	pCommand->target = target;
	pCommand->scale = scale;

	return pCommand;
}

void cScale::Undo()
{
	D3DXVECTOR3 v = target->Scaling();
	target->Scaling(scale);
	scale = v;
}

void cScale::Redo()
{
	D3DXVECTOR3 v = target->Scaling();
	target->Scaling(scale);
	scale = v;
}

cReset * cReset::Create(GameRender * target)
{
	cReset* pCommand = new cReset();
	pCommand->target = target;
	pCommand->position = target->Position();
	pCommand->rotate = target->Rotation();
	pCommand->scale = target->Scaling();
	
	return pCommand;
}

void cReset::Undo()
{
	target->Position(position);
	target->Rotation(rotate);
	target->Scaling(scale);
}

void cReset::Redo()
{
	D3DXVECTOR3 v = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 s = D3DXVECTOR3(1, 1, 1);
	target->Position(v);
	target->Rotation(v);
	target->Scaling(s);
}

cTexture * cTexture::Create(Landscape::Terrain * target)
{
	cTexture * pCommand = new cTexture();
	pCommand->target = target;
	if (target->GetMaterial()->GetDiffuseMap())
		pCommand->file = target->GetMaterial()->GetDiffuseMap()->GetFile();
	else pCommand->file = L"";
	return pCommand;
}

void cTexture::Undo()
{
	wstring tFile;
	tFile = target->GetMaterial()->GetDiffuseMap()->GetFile();
	target->GetMaterial()->SetDiffuseMap(file);
	file = tFile;
}

void cTexture::Redo()
{
	wstring tFile;
	tFile = target->GetMaterial()->GetDiffuseMap()->GetFile();
	target->GetMaterial()->SetDiffuseMap(file);
	file = tFile;
}

cData::~cData()
{
	SAFE_DELETE_ARRAY(vertex0);
	SAFE_DELETE_ARRAY(vertex1);
	SAFE_DELETE_ARRAY(index0);
	SAFE_DELETE_ARRAY(index1);
}

cData * cData::Create(Landscape::Terrain * target)
{
	cData* pCommand = new cData();
	pCommand->target = target;
	pCommand->vCount0 = target->GetData()->GetVertexCount();
	pCommand->iCount0 = target->GetData()->GetIndexCount();
	pCommand->vertex0 = new VertexType[pCommand->vCount0];
	pCommand->index0 = new UINT[pCommand->iCount0];
	target->GetData()->CopyData(&pCommand->vertex0, &pCommand->vertices0, &pCommand->index0);
	return pCommand;
}

void cData::Undo()
{
	if (!vertex1)
	{
		vCount1 = target->GetData()->GetVertexCount();
		iCount1 = target->GetData()->GetIndexCount();
		vertex1 = new VertexType[vCount1];
		index1 = new UINT[iCount1];

		target->GetData()->CopyData(&vertex1, &vertices1, &index1);
	}

	target->GetData()->SetData(vertex0, vertices0, index0, vCount0, iCount0);
}

void cData::Redo()
{
	target->GetData()->SetData(vertex1, vertices1, index1, vCount1, iCount1);
}

cParenting * cParenting::Create(GameRender * target)
{
	cParenting * pCommand = new cParenting();
	pCommand->target = target;
	pCommand->parent = target->Parent();
	return pCommand;
}

void cParenting::Undo()
{
	GameRender* tParent = target->Parent();
	if (parent)
		parent->AddChild(target);
	else target->Separation(NULL);
	parent = tParent;
}

void cParenting::Redo()
{
	GameRender* tParent = target->Parent();
	if (parent)
		parent->AddChild(target);
	else target->Separation(NULL);
	parent = tParent;
}

cNew::~cNew()
{
	if (bDelete)
		SAFE_DELETE(target);
}

cNew * cNew::Create(GameRender * target)
{
	cNew* pCommand = new cNew();
	pCommand->target = target;
	pCommand->bDelete = false;
	return pCommand;
}

void cNew::Undo()
{
	target->Separation(NULL);
	bDelete = true;
}

void cNew::Redo()
{
	GameRender::root->AddChild(target);
	bDelete = false;
}

cDelete::~cDelete()
{
	if (bDelete)
	{
		SAFE_DELETE(target);
	}
}

cDelete * cDelete::Create(GameRender * target)
{
	cDelete* pCommand = new cDelete();
	pCommand->target = target;
	pCommand->bDelete = true;

	return pCommand;
}

void cDelete::Undo()
{
	GameRender::root->AddChild(target);
	bDelete = false;
}

void cDelete::Redo()
{
	target->Separation(NULL);
	bDelete = true;
}
