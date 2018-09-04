#pragma once

class Camera
{
public:
	enum class CameraType
	{
		Free,
		FristPersonPoint,
		ThirdPersonPoint,
		Count,
	};

public:
	~Camera();

	void GetPosition(D3DXVECTOR3* vec)
	{
		*vec = position;
	}

	void SetPosition(float x, float y, float z)
	{
		position = D3DXVECTOR3(x, y, z);
		UpdateView();
	}

	void GetRotation(D3DXVECTOR2* vec)
	{
		*vec = rotation;
	}

	void GetRotationDegree(D3DXVECTOR2* vec)
	{
		*vec = rotation * 180.0f / (float)D3DX_PI;
	}

	void SetRotation(float x, float y)
	{
		rotation = D3DXVECTOR2(x, y);
		UpdateRotation();
	}

	void SetRotationDegree(float x, float y)
	{
		//rotation = D3DXVECTOR2(x, y) * (float)D3DX_PI / 180.0f;
		rotation = D3DXVECTOR2(x, y) * 0.01745328f;
		UpdateRotation();
	}

	void GetMatrix(D3DXMATRIX* view)
	{
		*view = matView;
	}

	void GetForward(D3DXVECTOR3* vec)
	{
		*vec = forward;
	}

	void GetRight(D3DXVECTOR3* vec)
	{
		*vec = right;
	}

	void GetUp(D3DXVECTOR3* vec)
	{
		*vec = up;
	}

	void CopyInfo(Camera* camera);
	virtual void SetTarget(D3DXVECTOR3 * target) {}

	D3DXVECTOR3 GetDirection(class Viewport* vp, class Perspective* perspective);
	D3DXVECTOR3 GetDirection(D3DXVECTOR3 & direction);
	virtual void Update() = 0;

	static vector<Camera*> cameras;

protected:
	Camera();

	virtual void UpdateMove();
	virtual void UpdateRotation();
	virtual void UpdateView();

private:
	D3DXVECTOR3 position;
	D3DXVECTOR2 rotation;

	D3DXVECTOR3 forward; //���溤��
	D3DXVECTOR3 right; //��������
	D3DXVECTOR3 up; //��溤��

	D3DXMATRIX matRotation;
	D3DXMATRIX matView;
};