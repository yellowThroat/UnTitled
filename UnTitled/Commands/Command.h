#pragma once

class GameEnemy;
namespace Landscape
{
	class Terrain;
}

class Command
{
public:
	virtual ~Command() {}
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};

class cString : public Command
{
public:
	static cString* Create(class GameRender* target, string edit);
	void Undo() override;
	void Redo() override;

	class GameRender* target;
	string edit;
};

class cPosition : public Command
{
public:
	static cPosition* Create(class GameRender* render, D3DXVECTOR3 position);
	void Undo() override;
	void Redo() override;

	GameRender* target;
	D3DXVECTOR3 position;
};

class cRotate: public Command
{
public:
	static cRotate* Create(class GameRender* target, D3DXVECTOR3 rotate);
	void Undo() override;
	void Redo() override;

	GameRender* target;
	D3DXVECTOR3 rotate;
};

class cScale: public Command
{
public:
	static cScale* Create(class GameRender* target, D3DXVECTOR3 scale);
	void Undo() override;
	void Redo() override;

	GameRender* target;
	D3DXVECTOR3 scale;
};

class cReset : public Command
{
public:
	static cReset* Create(class GameRender* target);
	void Undo() override;
	void Redo() override;

	GameRender* target;
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotate;
	D3DXVECTOR3 scale;

};

class cTexture : public Command
{
public:
	static cTexture* Create(Landscape::Terrain* target);

	void Undo() override;
	void Redo() override;

	Landscape::Terrain* target;
	wstring file;
};

class cData : public Command
{
public:
	~cData();
	typedef VertexColorTextureNormal VertexType;
	static cData* Create(Landscape::Terrain* target);

	void Undo() override;
	void Redo() override;

	Landscape::Terrain* target;

	VertexType * vertex0;
	VertexType * vertices0;
	UINT* index0;
	UINT vCount0;
	UINT iCount0;

	VertexType * vertex1;
	VertexType * vertices1;
	UINT* index1;
	UINT vCount1;
	UINT iCount1;
};

class cParenting : public Command
{
public:
	static cParenting* Create(GameRender* target);

	void Undo() override;
	void Redo() override;

	GameRender* target;
	GameRender* parent;
};


class cNew : public Command
{
public:
	~cNew();
	static cNew* Create(GameRender* target);

	void Undo() override;
	void Redo() override;

	GameRender* target;
	bool bDelete;
};

class cDelete : public Command
{
public:
	~cDelete();
	static cDelete* Create(GameRender* target);

	void Undo() override;
	void Redo() override;

	GameRender* target;
	bool bDelete;
};