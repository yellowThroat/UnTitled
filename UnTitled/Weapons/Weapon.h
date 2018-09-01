#pragma once

class Weapon 
{
public:
	Weapon();
	~Weapon();

	virtual void Update();
	virtual void Render();

	void World(D3DXMATRIX val) { world = val; }

protected:
	class Model* model;
	D3DXMATRIX world;
};