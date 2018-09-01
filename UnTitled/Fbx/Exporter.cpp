#include "stdafx.h"
#include "Exporter.h"
#include "Type.h"
#include "BoneWeights.h"
#include "Utility.h"
#include "../Utilities/BinaryFile.h"

Fbx::Exporter::Exporter(wstring fbxFile, bool bXna)
{
	this->bXna = bXna;

	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");

	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);


	importer = FbxImporter::Create(manager, "");

	string sFile = String::ToString(fbxFile);
	bool bCheck = importer->Initialize(sFile.c_str(), -1, ios);
	assert(bCheck == true);
	
	bCheck = importer->Import(scene);
	assert(bCheck == true);

	converter = new FbxGeometryConverter(manager);
}

Fbx::Exporter::~Exporter()
{
	SAFE_DELETE(converter);

	ios->Destroy();
	importer->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void Fbx::Exporter::ExportMaterial(wstring saveFolder, wstring saveName)
{
	int count = scene->GetMaterialCount();
	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);

		FbxMaterial* material = new FbxMaterial();
		material->Name = fbxMaterial->GetName();

		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true)
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert *)fbxMaterial;

			material->Ambient = Utility::ToColor(lambert->Ambient, lambert->AmbientFactor);
			material->Diffuse = Utility::ToColor(lambert->Diffuse, lambert->DiffuseFactor);
			material->Emissive = Utility::ToColor(lambert->Emissive, lambert->EmissiveFactor);
		}

		if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId) == true)
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong *)fbxMaterial;

			material->Specular = Utility::ToColor(phong->Specular, phong->SpecularFactor);
			material->Shininess = (float)phong->Shininess;
		}

		
		FbxProperty prop;

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		material->DiffuseFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		material->SpecularFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
		material->EmissiveFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sBump);
		material->NormalFile = Utility::GetTextureFile(prop);

		materials.push_back(material);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	Path::CreateFolders(saveFolder);

	Json::Value root;
	for (FbxMaterial* material : materials)
	{
		Json::Value val;
		Json::SetValue(val, "Name", material->Name);

		string shaderName = "999_Mesh.hlsl";
		Json::SetValue(val, "ShaderName", shaderName);

		Json::SetValue(val, "Ambient", material->Ambient);
		Json::SetValue(val, "Diffuse", material->Diffuse);
		Json::SetValue(val, "Emissive", material->Emissive);
		Json::SetValue(val, "Specular", material->Specular);
		Json::SetValue(val, "Shininess", material->Shininess);

	
		CopyTextureFile(material->DiffuseFile, saveFolder);
		Json::SetValue(val, "DiffuseFile", material->DiffuseFile);

		CopyTextureFile(material->SpecularFile, saveFolder);
		Json::SetValue(val, "SpecularFile", material->SpecularFile);

		CopyTextureFile(material->EmissiveFile, saveFolder);
		Json::SetValue(val, "EmissiveFile", material->EmissiveFile);

		CopyTextureFile(material->NormalFile, saveFolder);
		Json::SetValue(val, "NormalFile", material->NormalFile);

		root[material->Name.c_str()] = val;

		SAFE_DELETE(material);
	}

	Json::WriteData(saveFolder + saveName + L".material", &root);
}

void Fbx::Exporter::ExportMesh(wstring saveFolder, wstring saveName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1);
	ReadSkinData();

	WriteMeshData(saveFolder, saveName);
}

void Fbx::Exporter::ExportAnimation(wstring saveFolder, wstring saveName)
{
	ReadAnimation();

	WriteAnimation(saveFolder, saveName);
}

void Fbx::Exporter::CopyTextureFile(string & textureFile, wstring saveFolder)
{
	if (textureFile.length() < 1)
		return;

	wstring file = String::ToWString(textureFile);
	wstring fileName = Path::GetFileName(file);

	if (Path::ExistFile(textureFile) == true)
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE);

	textureFile = String::ToString(fileName);
}

void Fbx::Exporter::ReadBoneData(FbxNode * node, int index, int parent)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		bool bCheck = false;
		bCheck |= (nodeType == FbxNodeAttribute::eSkeleton);
		bCheck |= (nodeType == FbxNodeAttribute::eMesh);
		bCheck |= (nodeType == FbxNodeAttribute::eNull);
		bCheck |= (nodeType == FbxNodeAttribute::eMarker);

		if (bCheck == true)
		{
			FbxBoneData* bone = new FbxBoneData();
			bone->Index = index;
			bone->Parent = parent;
			bone->Name = node->GetName();
			bone->Transform = Utility::ToMatrix(node->EvaluateLocalTransform(), bXna);
			bone->AbsoluteTransform = Utility::ToMatrix(node->EvaluateGlobalTransform(), bXna);
			boneDatas.push_back(bone);

			if (nodeType == FbxNodeAttribute::eMesh)
			{
				converter->Triangulate(attribute, true, true);

				ReadMeshData(node, index);
			}
		}//if(bCheck)
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ReadBoneData(node->GetChild(i), boneDatas.size(), index);
}

void Fbx::Exporter::ReadMeshData(FbxNode * node, int parentBone)
{
	FbxMesh* mesh = node->GetMesh();
	
	vector<FbxVertex *> vertices;
	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		int vertexInPolygon = mesh->GetPolygonSize(p);
		assert(vertexInPolygon == 3);

		for (int vi = 0; vi < vertexInPolygon; vi++)
		{
			FbxVertex* vertex = new FbxVertex;

			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->ControlPoint = cpIndex;
			
			FbxVector4 position = mesh->GetControlPointAt(cpIndex);
			vertex->Vertex.position = Utility::ToVector3(position);
					
			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal);
			normal.Normalize();
			vertex->Vertex.normal = Utility::ToVector3(normal);
	
			vertex->MaterialName = Utility::GetMaterialName(mesh, p, cpIndex);
	
			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->Vertex.uv = Utility::GetUv(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}//for(vi)
		D3DXVECTOR3 v0 = vertices[vertices.size() - 3]->Vertex.position;
		D3DXVECTOR3 v1 = vertices[vertices.size() - 2]->Vertex.position;
		D3DXVECTOR3 v2 = vertices[vertices.size() - 1]->Vertex.position;

		D3DXVECTOR2 uv0 = vertices[vertices.size() - 3]->Vertex.uv;
		D3DXVECTOR2 uv1 = vertices[vertices.size() - 2]->Vertex.uv;
		D3DXVECTOR2 uv2 = vertices[vertices.size() - 1]->Vertex.uv;

		D3DXVECTOR3	tangent = Utility::CalcTangent(v0, v1, v2, uv0, uv1, uv2);

		vertices[vertices.size() - 3]->Vertex.tangent += tangent;
		vertices[vertices.size() - 2]->Vertex.tangent += tangent;
		vertices[vertices.size() - 1]->Vertex.tangent += tangent;

	}//for(p)

	for (UINT i = 0; i < vertices.size(); i++)
	{
		D3DXVECTOR3 n = vertices[i]->Vertex.normal;
		D3DXVECTOR3 t = vertices[i]->Vertex.tangent;

		D3DXVECTOR3 temp = (t - n * D3DXVec3Dot(&n, &t));
		D3DXVec3Normalize(&temp, &temp);

		vertices[i]->Vertex.tangent = temp;
	}
			
	FbxMeshData* meshData = new FbxMeshData();
	meshData->Name = node->GetName();
	meshData->ParentBone = parentBone;
	meshData->Vertices = vertices;
	meshData->Mesh = mesh;
	meshDatas.push_back(meshData);
}

void Fbx::Exporter::ReadSkinData()
{
	for (FbxMeshData* meshData : meshDatas)
	{
		FbxMesh* mesh = meshData->Mesh;

		///////////////////////////////////////////////////////////////////////////////////////

		int deformerCount = mesh->GetDeformerCount();
		vector<FbxBoneWeights> boneWeights(mesh->GetControlPointsCount(), FbxBoneWeights());

		for (int i = 0; i < deformerCount; i++)
		{
			FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);

			FbxSkin* skin = reinterpret_cast<FbxSkin *>(deformer);
			if (skin == NULL) continue;

			for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);
				assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

				string linkName = cluster->GetLink()->GetName();
				UINT boneIndex = GetBoneIndexByName(linkName);


				FbxAMatrix transform;
				FbxAMatrix linkTransform;

				cluster->GetTransformMatrix(transform);
				cluster->GetTransformLinkMatrix(linkTransform);

				boneDatas[boneIndex]->Transform = Utility::ToMatrix(transform, bXna);
				boneDatas[boneIndex]->AbsoluteTransform = Utility::ToMatrix(linkTransform, bXna);


				for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount(); indexCount++)
				{
					int temp = cluster->GetControlPointIndices()[indexCount];
					double* weights = cluster->GetControlPointWeights();

					boneWeights[temp].AddBoneWeight(boneIndex, (float)weights[indexCount]);
				}
			}//for(clusterIndex)
		}//for(joints)

		for (size_t i = 0; i < boneWeights.size(); i++)
			boneWeights[i].Normalize();


		for (FbxVertex* vertex : meshData->Vertices)
		{
			int cpIndex = vertex->ControlPoint;

			FbxBlendWeight weights;
			boneWeights[cpIndex].GetBlendWeights(weights);
			vertex->Vertex.blendIndices = weights.Indices;
			vertex->Vertex.blendWeights = weights.Weights;
		}

		for (int i = 0; i < scene->GetMaterialCount(); i++)
		{
			FbxSurfaceMaterial* material = scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex *> gather;
			for (FbxVertex* temp : meshData->Vertices)
			{
				if (temp->MaterialName == materialName)
					gather.push_back(temp);
			}
			if (gather.size() < 1) continue;


			FbxMeshPartData* meshPart = new FbxMeshPartData();
			meshPart->MaterialName = materialName;

			for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->Vertex;

				meshPart->Vertices.push_back(vertex);
				meshPart->Indices.push_back(meshPart->Indices.size());
			}
			meshData->MeshParts.push_back(meshPart);
		}
	}//for(MeshData)
}

void Fbx::Exporter::WriteMeshData(wstring saveFolder, wstring saveName)
{
	Path::CreateFolders(saveFolder);

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".mesh");

	///////////////////////////////////////////////////////////////////////////////////////////////

	w->UInt(boneDatas.size());
	for (FbxBoneData* bone : boneDatas)
	{
		w->Int(bone->Index);
		w->String(bone->Name);

		w->Int(bone->Parent);
		w->Matrix(bone->Transform);
		w->Matrix(bone->AbsoluteTransform);

		SAFE_DELETE(bone);
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////

	w->UInt(meshDatas.size());
	for (FbxMeshData* meshData : meshDatas)
	{
		w->String(meshData->Name);
		w->Int(meshData->ParentBone);

		w->UInt(meshData->MeshParts.size());
		for (FbxMeshPartData* part : meshData->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->Vertices.size());
			w->Byte(&part->Vertices[0], sizeof(ModelVertexType) * part->Vertices.size());

			w->UInt(part->Indices.size());
			w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size());

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData);
	}
		
	w->Close();
	SAFE_DELETE(w);
}

UINT Fbx::Exporter::GetBoneIndexByName(string name)
{
	for (size_t i = 0; i < boneDatas.size(); i++)
	{
		if (boneDatas[i]->Name == name)
			return i;
	}

	return -1;
}

void Fbx::Exporter::ReadAnimation()
{
	FbxTime::EMode mode = scene->GetGlobalSettings().GetTimeMode();
	float frameRate = (float)FbxTime::GetFrameRate(mode);

	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		FbxAnimation* animation = new FbxAnimation();
		animation->FrameRate = frameRate;

		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		animation->Name = takeInfo->mName.Buffer();
		

		FbxTimeSpan span = takeInfo->mLocalTimeSpan;
		int start = (int)span.GetStart().GetFrameCount();
		int end = (int)span.GetStop().GetFrameCount();

		if (start < end)
			ReadAnimation(animation, scene->GetRootNode(), start, end);
		
		animation->TotalFrame = (int)animation->Keyframes[0]->Transforms.size();

		animDatas.push_back(animation);
	}//for(i)
}

void Fbx::Exporter::ReadAnimation(FbxAnimation * animation, FbxNode * node, int start, int end)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			FbxKeyframe* keyframe = new FbxKeyframe();
			keyframe->BoneName = node->GetName();

			for(int i = start; i <= end; i++)
			{
				FbxTime animationTime;
				animationTime.SetFrame(i);

				FbxAMatrix matrix = node->EvaluateLocalTransform(animationTime);
				D3DXMATRIX transform = Utility::ToMatrix(matrix, bXna);

				FbxKeyframeData data;
				data.Transform = transform;
				data.Translation = D3DXVECTOR3(transform._41, transform._42, transform._43);
				D3DXQuaternionRotationMatrix(&data.Rotation, &transform);

				FbxVector4 scale = node->EvaluateLocalScaling(animationTime);
				data.Scale = Utility::ToVector3(scale);

				keyframe->Transforms.push_back(data);
			}
			
			animation->Keyframes.push_back(keyframe);
		}//if(nodeType)
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ReadAnimation(animation, node->GetChild(i), start, end);
}

void Fbx::Exporter::WriteAnimation(wstring saveFolder, wstring saveName)
{
	Path::CreateFolders(saveFolder);

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".anim");

	w->UInt(animDatas.size());
	for (FbxAnimation* animation : animDatas)
	{
		w->String(animation->Name);

		w->Int(animation->TotalFrame);
		w->Float(animation->FrameRate);

		w->UInt(animation->Keyframes.size());
		for (FbxKeyframe* frame : animation->Keyframes)
		{
			w->String(frame->BoneName);

			w->UInt(frame->Transforms.size());
			w->Byte(&frame->Transforms[0], sizeof(FbxKeyframeData) * frame->Transforms.size());

			SAFE_DELETE(frame);
		}

		SAFE_DELETE(animation);
	}

	w->Close();
	SAFE_DELETE(w);
}
