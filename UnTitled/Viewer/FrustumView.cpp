#include "stdafx.h"
#include "FrustumView.h"


FrustumView::FrustumView(float zFar, Camera* camera, Perspective* perspective)
	:zFar(zFar), camera(camera), perspective(perspective)
{
}

FrustumView::~FrustumView()
{
}

void FrustumView::Build()
{
	D3DXMATRIX V, P;
	camera->GetMatrix(&V);
	perspective->GetMatrix(&P);

	float zNear = -P._43 / P._33;
	float radius = zFar / (zFar - zNear);

	P._33 = radius;
	P._43 = -radius * zNear;

	D3DXMATRIX mat;
	D3DXMatrixMultiply(&mat, &V, &P);

	//근 평면
	planes[0].a = mat._14 + mat._13;
	planes[0].b = mat._24 + mat._23;
	planes[0].c = mat._34 + mat._33;
	planes[0].d = mat._44 + mat._43;

	//원 평면
	planes[1].a = mat._14 - mat._13;
	planes[1].b = mat._24 - mat._23;
	planes[1].c = mat._34 - mat._33;
	planes[1].d = mat._44 - mat._43;

	//좌 평면
	planes[2].a = mat._14 + mat._11;
	planes[2].b = mat._24 + mat._21;
	planes[2].c = mat._34 + mat._31;
	planes[2].d = mat._44 + mat._41;

	//우 평면
	planes[3].a = mat._14 - mat._11;
	planes[3].b = mat._24 - mat._21;
	planes[3].c = mat._34 - mat._31;
	planes[3].d = mat._44 - mat._41;

	//상 평면
	planes[4].a = mat._14 - mat._12;
	planes[4].b = mat._24 - mat._22;
	planes[4].c = mat._34 - mat._32;
	planes[4].d = mat._44 - mat._42;

	//하 평면
	planes[5].a = mat._14 + mat._12;
	planes[5].b = mat._24 + mat._22;
	planes[5].c = mat._34 + mat._32;
	planes[5].d = mat._44 + mat._42;

	for (int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&planes[i], &planes[i]);

}

void FrustumView::Update()
{
	Build();
	
}

bool FrustumView::ContainPoint(float x, float y, float z)
{
	return ContainPoint(D3DXVECTOR3(x, y, z));
}

bool FrustumView::ContainPoint(D3DXVECTOR3 position)
{
	for (int i = 0; i < 6; i++)
		if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
			return false;

	return true;
}

