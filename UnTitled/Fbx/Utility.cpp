#include "stdafx.h"
#include "Utility.h"

D3DXMATRIX Fbx::Utility::Negative(bool bXna)
{
	D3DXMATRIX scaleX;
	D3DXMatrixScaling(&scaleX, -1, 1, 1);
	if (bXna == true)
	{
		D3DXMATRIX rotationX;
		D3DXMatrixRotationX(&rotationX, -Math::ToRadian(90));
	
		D3DXMATRIX rotationY;
		D3DXMatrixRotationY(&rotationY, Math::ToRadian(180));
	
		return scaleX * rotationX * rotationY;
	}
	else
	{
		D3DXMATRIX rotation;
		D3DXMatrixRotationY(&rotation, Math::ToRadian(180));
	
		return scaleX;
	}
}

D3DXVECTOR2 Fbx::Utility::ToVector2(FbxVector2 & vec)
{
	return D3DXVECTOR2((float)vec.mData[0], (float)vec.mData[1]);
}

D3DXCOLOR Fbx::Utility::ToColor(FbxVector4 & vec)
{
	return D3DXCOLOR((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], 1);
}

D3DXCOLOR Fbx::Utility::ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor)
{
	FbxDouble3 color = vec;

	D3DXCOLOR result;
	result.r = (float)color.mData[0];
	result.g = (float)color.mData[1];
	result.b = (float)color.mData[2];
	result.a = (float)factor;

	return result;
}

D3DXVECTOR3 Fbx::Utility::ToVector3(FbxVector4 & vec)
{
	return D3DXVECTOR3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
}

D3DXMATRIX Fbx::Utility::ToMatrix(FbxAMatrix & matrix, bool bXna)
{
	FbxVector4 r1 = matrix.GetRow(0);
	FbxVector4 r2 = matrix.GetRow(1);
	FbxVector4 r3 = matrix.GetRow(2);
	FbxVector4 r4 = matrix.GetRow(3);

	D3DXMATRIX temp = D3DXMATRIX
	(
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
		(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
	);
	//return temp;
	return Negative(bXna) * temp * Negative(bXna);
}

D3DXVECTOR3 Fbx::Utility::CalcTangent(D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR2 uv0, D3DXVECTOR2 uv1, D3DXVECTOR2 uv2)
{
	D3DXVECTOR3 e0 = p1 - p0;
	D3DXVECTOR3 e1 = p2 - p0;

	float u0 = uv1.x - uv0.x;
	float u1 = uv2.x - uv0.x;
	float v0 = uv1.y - uv0.y;
	float v1 = uv2.y - uv0.y;

	float r = 1.0f / (u0 * v1 - v0 * u1);

	D3DXVECTOR3 tangent;
	tangent.x = r* (v1 * e0.x - v0 * e1.x);
	tangent.y = r* (v1 * e0.y - v0 * e1.y);
	tangent.z = r* (v1 * e0.z - v0 * e1.z);


	return tangent;
}

string Fbx::Utility::GetTextureFile(FbxProperty & prop)
{
	if (prop.IsValid() == true)
	{
		if (prop.GetSrcObjectCount() > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();

			if (texture != NULL)
				return string(texture->GetFileName());
		}
	}

	return "";
}

string Fbx::Utility::GetMaterialName(FbxMesh * mesh, int polygonIndex, int cpIndex)
{
	FbxNode* node = mesh->GetNode();
	if (node == NULL) return "";

	FbxLayerElementMaterial* material = mesh->GetLayer(0)->GetMaterials();
	if (material == NULL) return "";

	FbxLayerElement::EMappingMode mappingMode = material->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = material->GetReferenceMode();


	int mappingIndex = -1;
	switch (mappingMode)
	{
		case FbxLayerElement::eAllSame: mappingIndex = 0; break;
		case FbxLayerElement::eByPolygon: mappingIndex = polygonIndex; break;
		case FbxLayerElement::eByControlPoint: mappingIndex = cpIndex; break;
		case FbxLayerElement::eByPolygonVertex: mappingIndex = polygonIndex * 3; break;
		default: assert(false); break;
	}

	
	FbxSurfaceMaterial* findMaterial = NULL;
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			findMaterial = node->GetMaterial(mappingIndex);
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArray = material->GetIndexArray();

		if (mappingIndex < indexArray.GetCount())
		{
			int tempIndex = indexArray.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				findMaterial = node->GetMaterial(tempIndex);
		}
	}

	if (findMaterial == NULL) return "";
	
	return findMaterial->GetName();
}

D3DXVECTOR2 Fbx::Utility::GetUv(FbxMesh * mesh, int cpIndex, int uvIndex)
{
	D3DXVECTOR2 result = D3DXVECTOR2(0, 0);

	FbxLayerElementUV* uv = mesh->GetLayer(0)->GetUVs();
	if (uv == NULL)
		return result;

	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	switch (mappingMode)
	{
		case FbxLayerElement::eByControlPoint:
		{
			if (refMode == FbxLayerElement::eDirect)
			{
				result.x = (float)uv->GetDirectArray().GetAt(cpIndex).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(cpIndex).mData[1];
			}
			else if (refMode == FbxLayerElement::eIndexToDirect)
			{
				int index = uv->GetIndexArray().GetAt(cpIndex);

				result.x = (float)uv->GetDirectArray().GetAt(index).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(index).mData[1];
			}
		}
		break;

		case FbxLayerElement::eByPolygonVertex:
		{
			result.x = (float)uv->GetDirectArray().GetAt(uvIndex).mData[0];
			result.y = (float)uv->GetDirectArray().GetAt(uvIndex).mData[1];
		}
		break;
	}
	result.y = 1.0f - result.y;
	
	return result;
}
