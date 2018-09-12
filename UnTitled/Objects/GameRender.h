#pragma once

class GameRender
{
public:
	struct Storage
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 rotate;
		D3DXVECTOR3 scale;
		char name[32];
	};

	enum class RenderType 
	{
		Unknown = 0,
		Player,
		Zombie,
		Building,
		Terrain, PointLight, SpotLight,
		Etc,
	};

	GameRender();
	virtual ~GameRender();

	virtual void PreRender();
	virtual void Update();
	virtual void Render();
	virtual void PostRender();

	void Enable(bool val);
	bool Enable();
	vector<function<void()>> Enabled;

	void Visible(bool val);
	bool Visible();
	vector<function<void()>> Visibled;

	void RootAxis(D3DXMATRIX& matrix);
	void RootAxis(D3DXVECTOR3& rotate);
	D3DXMATRIX RootAxis();

	void World(D3DXMATRIX& matrix);
	D3DXMATRIX World();

	void Position(D3DXVECTOR3& vec);
	D3DXVECTOR3 Position();

	void Direction(D3DXVECTOR3& vec);
	D3DXVECTOR3 Direction();

	void Up(D3DXVECTOR3& vec);
	D3DXVECTOR3 Up();

	void Right(D3DXVECTOR3& vec);
	D3DXVECTOR3 Right();

	void Rotation(D3DXVECTOR3& vec);
	D3DXVECTOR3 Rotation();

	void Scaling(D3DXVECTOR3& vec);
	D3DXVECTOR3 Scaling();

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	D3DXVECTOR3 CalcVelocity(D3DXVECTOR3& velocity);
	void AddPosition(D3DXVECTOR3 & vec);
	D3DXMATRIX Transformed();

	UINT GetNum() { return num; }
	RenderType GetType() { return type; }

	void Name(string str);
	string Name();

	void SetParent(GameRender* ptr) { parent = ptr; }
	void Parent(GameRender* ptr);
	GameRender* Parent();

	void Selectable(GameRender** r);

	UINT ChildCount();
	GameRender* Child(UINT index);
	void AddChild(GameRender* val);
	void DeleteChild(GameRender* val);
	void Separation(GameRender* val = NULL);
	bool CheckParenting(GameRender* val);
	void ClearChilds();
	void NullChilds();
	void Parenting(D3DXVECTOR3& rotate, D3DXVECTOR3& scale);
	void Separation(D3DXVECTOR3& rotate, D3DXVECTOR3& scale);

	virtual void ShaderFile(Shader* s = NULL) {}
	virtual void InitSettings();

	virtual void Save(class BinaryWriter* w);
	virtual void Load(class BinaryReader* r);

protected:
	D3DXMATRIX GetParentWorld();
	void CalcPosition();

protected:
	bool enable;
	bool visible;

	D3DXMATRIX rootAxis;
	D3DXMATRIX world;
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotate;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 velocity;
private:

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;

protected:
	GameRender* parent;
	vector<GameRender*> childs;
	UINT num;
	bool bShowBounding;

	string name;
	Shader* oShader;
	static UINT numOfRender;
	RenderType type;
	ExecuteValues* values;
public:
	Storage editBuf;
	static class UndoRedoManager* URManager;
	static bool bParenting;
	static GameRender* root;
	GameRender* CreateObject(RenderType type);
	void SetValues(ExecuteValues* val);
	
protected:
	class IsBoneBuffer : public ShaderBuffer
	{
	public:
		IsBoneBuffer()
			: ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.isBone = 1;
		}

		struct Struct
		{
			UINT isBone;
			float Padding[3];
		}Data;
	};

	IsBoneBuffer* isBoneBuffer;
};