#include "stdafx.h"

#include "Terrain.h"

#include "Data.h"
#include "../Utilities/BinaryFile.h"

Landscape::Data::Data(wstring file, float heightRatio)
	: width(0), height(0)
	, vertexCount(0), indexCount(0)
	, vertexBuffer(NULL), indexBuffer(NULL)
	, indexData(NULL), vertices(NULL)
	, heightMap(NULL)
{
	if (file.length() > 0)
		SetHeightMapFile(file);
	else
		SetPlane();

	root = new Quad();
	InitQuad(root, 0, 0, (float)width, (float)height);
}

Landscape::Data::~Data()
{
	Clear();
}

float Landscape::Data::GetHeight(D3DXVECTOR3 position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;
	int a = 0;
	if (x < 0 || x >= width)
		return 0.0f;

	if (z < 0 || z >= height)
		return 0.0f;

	UINT index0 = (width + 1) * z + x;
	UINT index1 = (width + 1) * (z + 1) + x;
	UINT index2 = (width + 1) * z + x + 1;
	UINT index3 = (width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 v0 = vertices[index0].position;
	D3DXVECTOR3 v1 = vertices[index1].position;
	D3DXVECTOR3 v2 = vertices[index2].position;
	D3DXVECTOR3 v3 = vertices[index3].position;

	float deltaX = position.x - v0.x;
	float deltaZ = position.z - v0.z;

	D3DXVECTOR3 temp;
	if (deltaX + deltaZ <= 1)
		temp = v0 + (v2 - v0) * deltaX + (v1 - v0) * deltaZ;
	else
	{
		deltaX = 1 - deltaX;
		deltaZ = 1 - deltaZ;

		temp = v3 + (v2 - v3) * deltaX + (v1 - v3) * deltaZ;
	}

	return temp.y;
}

void Landscape::Data::SetPlane(int x, int y)
{
	if (x <= 0 || y <= 0) return;

	Clear();

	FillVertexData(x, y, 0);
	FillNormalData();

	CreateBuffer();
}

void Landscape::Data::SetHeightMapFile(wstring file, float heightRatio)
{
	if (file.length() < 1) 
		return;


	Clear();

	heightMap = new Texture(file);

	FillVertexData(0, 0, heightRatio);
	FillNormalData();

	CreateBuffer();
}

void Landscape::Data::UpdateVertexData(D3D11_BOX * box)
{
	D3D::GetDC()->UpdateSubresource
	(
		vertexBuffer, 0, box, vertices, sizeof(VertexType) * vertexCount, 0
	);
}

void Landscape::Data::Render()
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void Landscape::Data::SetColor(GridBuffer& grid, BrushBuffer& brush, D3DXCOLOR color)
{
	if (brush.Info.Type == 1)
	{
		if (brush.Info.Select)
		{
			UINT BeforeX = (UINT)brush.Info.Before.x / grid.Grid.Spacing;
			UINT BeforeZ = (UINT)brush.Info.Before.z / grid.Grid.Spacing;
			UINT CurrentX = (UINT)brush.Info.Position.x / grid.Grid.Spacing;
			UINT CurrentZ = (UINT)brush.Info.Position.z / grid.Grid.Spacing;
			UINT minX = min(BeforeX, CurrentX);
			UINT minZ = min(BeforeZ, CurrentZ);
			UINT maxX = max(BeforeX, CurrentX);
			UINT maxZ = max(BeforeZ, CurrentZ);

			for (UINT i = minZ; i <= maxZ + 1; i++)
			{
				for (UINT j = minX; j <= maxX + 1; j++)
				{
					vertices[j + (width + 1) * i].color = color;

				}
			}
		}
	}
	else if (brush.Info.Type == 2)
	{

		UINT minX = (UINT)max(0, (brush.Info.Position.x - brush.Info.Range));
		UINT maxX = (UINT)min(width + 1, (brush.Info.Position.x + brush.Info.Range));
		UINT minZ = (UINT)max(0, (brush.Info.Position.z - brush.Info.Range));
		UINT maxZ = (UINT)min(height + 1, (brush.Info.Position.z + brush.Info.Range));

		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float dx = vertices[j + i*(width + 1)].position.x - brush.Info.Position.x;
				float dy = vertices[j + i*(width + 1)].position.z - brush.Info.Position.z;

				float dist = sqrt(dx*dx + dy*dy);

				if (dist <= brush.Info.Range)
				{
					vertices[j + (width + 1) * i].color = color;

				}
			}
		}

	}
	else if (brush.Info.Type == 3)
	{
		UINT minX = (UINT)max(0, (brush.Info.Position.x - brush.Info.Range));
		UINT maxX = (UINT)min(width + 1, (brush.Info.Position.x + brush.Info.Range));
		UINT minZ = (UINT)max(0, (brush.Info.Position.z - brush.Info.Range));
		UINT maxZ = (UINT)min(height + 1, (brush.Info.Position.z + brush.Info.Range));

		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				vertices[j + (width + 1) * i].color = color;
			}
		}
	}


	UpdateVertexData();
}

void Landscape::Data::SetHeight(GridBuffer & grid, BrushBuffer & brush, bool upHeight)
{
	if (brush.Info.Type == 1)
	{
		if (brush.Info.Select)
		{
			UINT BeforeX = (UINT)brush.Info.Before.x / grid.Grid.Spacing;
			UINT BeforeZ = (UINT)brush.Info.Before.z / grid.Grid.Spacing;
			UINT CurrentX = (UINT)brush.Info.Position.x / grid.Grid.Spacing;
			UINT CurrentZ = (UINT)brush.Info.Position.z / grid.Grid.Spacing;
			UINT minX = min(BeforeX, CurrentX);
			UINT minZ = min(BeforeZ, CurrentZ);
			UINT maxX = max(BeforeX, CurrentX);
			UINT maxZ = max(BeforeZ, CurrentZ);

			for (UINT i = minZ; i <= maxZ + 1; i++)
			{
				for (UINT j = minX; j <= maxX + 1; j++)
				{
					if (upHeight)
					{
						vertices[j + (width + 1)*i].position.y += brush.Info.Speed;
					}
					else
					{
						vertices[j + (width + 1)*i].position.y -= brush.Info.Speed;
					}
				}
			}

			for (UINT i = minZ; i <= maxZ + 1; i++)
			{
				for (UINT j = minX; j <= maxX + 1; j++)
				{
					vertices[j + i*(width + 1)].normal = CalculateNormal(j, i);
				}
			}
		}
	}
	else if (brush.Info.Type == 2)
	{
		
		UINT minX = (UINT)max(0, (brush.Info.Position.x - brush.Info.Range));
		UINT maxX = (UINT)min(width + 1, (brush.Info.Position.x + brush.Info.Range));
		UINT minZ = (UINT)max(0, (brush.Info.Position.z - brush.Info.Range));
		UINT maxZ = (UINT)min(height + 1, (brush.Info.Position.z + brush.Info.Range));

		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float dx = vertices[j + i*(width + 1)].position.x - brush.Info.Position.x;
				float dy = vertices[j + i*(width + 1)].position.z - brush.Info.Position.z;

				float dist = sqrt(dx*dx + dy*dy);

				if (dist <= brush.Info.Range)
				{
					if (upHeight)
					{
						vertices[j + i*(width + 1)].position.y += brush.Info.Speed;
					}
					else
					{
						vertices[j + i*(width + 1)].position.y -= brush.Info.Speed;
					}

				}
			}
		}
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float dx = vertices[j + i*(width + 1)].position.x - brush.Info.Position.x;
				float dy = vertices[j + i*(width + 1)].position.z - brush.Info.Position.z;

				float dist = sqrt(dx*dx + dy*dy);

				if (dist <= brush.Info.Range)
				{
					vertices[j + i*(width + 1)].normal = CalculateNormal(j, i);
				}
			}
		}

		
	}
	else if (brush.Info.Type == 3)
	{
		UINT minX = (UINT)max(0, (brush.Info.Position.x - brush.Info.Range));
		UINT maxX = (UINT)min(width + 1, (brush.Info.Position.x + brush.Info.Range));
		UINT minZ = (UINT)max(0, (brush.Info.Position.z - brush.Info.Range));
		UINT maxZ = (UINT)min(height + 1, (brush.Info.Position.z + brush.Info.Range));

		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				if (upHeight)
				{
					vertices[j + i*(width + 1)].position.y += brush.Info.Speed;
				}
				else
				{
					vertices[j + i*(width + 1)].position.y -= brush.Info.Speed;
				}

			}
		}
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				vertices[j + i*(width + 1)].normal = CalculateNormal(j, i);
			}
		}
	}

	UpdateVertexData();
}

void Landscape::Data::Smooth(GridBuffer & grid, BrushBuffer & brush)
{
	UINT minX = (UINT)max(0, (brush.Info.Position.x - brush.Info.Range));
	UINT maxX = (UINT)min(width + 1, (brush.Info.Position.x + brush.Info.Range));
	UINT minZ = (UINT)max(0, (brush.Info.Position.z - brush.Info.Range));
	UINT maxZ = (UINT)min(height + 1, (brush.Info.Position.z + brush.Info.Range));

	if (brush.Info.Type == 2)
	{
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float dx = vertices[j + i*(width + 1)].position.x - brush.Info.Position.x;
				float dy = vertices[j + i*(width + 1)].position.z - brush.Info.Position.z;

				float dist = sqrt(dx*dx + dy*dy);

				if (dist <= brush.Info.Range)
				{
					float elapsed;
					float average = GetAverage(j, i);
					float temp = vertices[j + i*(width + 1)].position.y;
					
					
					if (temp > average)
					{
						elapsed = temp - average < brush.Info.Speed ? temp - average : brush.Info.Speed;
						vertices[j + i*(width + 1)].position.y -= elapsed;

					}
				}
			}
		}
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float dx = vertices[j + i*(width + 1)].position.x - brush.Info.Position.x;
				float dy = vertices[j + i*(width + 1)].position.z - brush.Info.Position.z;

				float dist = sqrt(dx*dx + dy*dy);

				if (dist <= brush.Info.Range)
				{
					float average = GetAverage(j, i);
					float temp = vertices[j + i*(width + 1)].position.y;


					if (temp > average)
					{
						vertices[j + i*(width + 1)].normal = CalculateNormal(j, i);
					}
				}
			}
		}
	}
	else if (brush.Info.Type = 3)
	{
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float elapsed;
				float average = GetAverage(j, i);
				float temp = vertices[j + i*(width + 1)].position.y;


				if (temp > average)
				{
					elapsed = temp - average < brush.Info.Speed ? temp - average : brush.Info.Speed;
					vertices[j + i*(width + 1)].position.y -= elapsed;
				}
			}
		}
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float average = GetAverage(j, i);
				float temp = vertices[j + i*(width + 1)].position.y;


				if (temp > average)
				{
					vertices[j + i*(width + 1)].normal = CalculateNormal(j, i);
				}
			}
		}

	}

	UpdateVertexData();
}

void Landscape::Data::Flattening(GridBuffer & grid, BrushBuffer & brush, float Height)
{
	UINT minX = (UINT)max(0, (brush.Info.Position.x - brush.Info.Range));
	UINT maxX = (UINT)min(width + 1, (brush.Info.Position.x + brush.Info.Range));
	UINT minZ = (UINT)max(0, (brush.Info.Position.z - brush.Info.Range));
	UINT maxZ = (UINT)min(height + 1, (brush.Info.Position.z + brush.Info.Range));


	if (brush.Info.Type == 2)
	{
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float dx = vertices[j + i*(width + 1)].position.x - brush.Info.Position.x;
				float dy = vertices[j + i*(width + 1)].position.z - brush.Info.Position.z;

				float dist = sqrt(dx*dx + dy*dy);

				if (dist <= brush.Info.Range)
				{
					vertices[j + i*(width + 1)].position.y = Height;

				}
			}
		}
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				float dx = vertices[j + i*(width + 1)].position.x - brush.Info.Position.x;
				float dy = vertices[j + i*(width + 1)].position.z - brush.Info.Position.z;

				float dist = sqrt(dx*dx + dy*dy);

				if (dist <= brush.Info.Range)
				{
					vertices[j + i*(width + 1)].normal = CalculateNormal(j, i);
				}
			}
		}
	}
	else if (brush.Info.Type = 3)
	{
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				vertices[j + i*(width + 1)].position.y = Height;
			}
		}
		for (UINT i = minZ; i <= maxZ; i++)
		{
			for (UINT j = minX; j <= maxX; j++)
			{
				vertices[j + i*(width + 1)].normal = CalculateNormal(j, i);
			}
		}
	}
	UpdateVertexData();
}

void Landscape::Data::CopyData(VertexType** data, VertexType** data1, UINT** index)
{
	*data1 = new VertexType[(width + 1) * (height + 1)];
	memcpy(*data1, vertices, (width + 1) * (height + 1) * sizeof(VertexType));
	memcpy(*index, indexData, indexCount * sizeof(UINT));
}

void Landscape::Data::SetData(VertexType * data, VertexType* data1, UINT * index, UINT vCount, UINT iCount)
{
	Clear();

	vertexCount = vCount;
	indexCount = iCount;

	indexData = new UINT[indexCount];
	vertices = new VertexType[vertexCount];

	memcpy(vertices, data1, sizeof(VertexType)* (width + 1) * (height + 1));
	memcpy(indexData, index, sizeof(UINT)* indexCount);

	FillNormalData();

	CreateBuffer();
}

bool Landscape::Data::GetPosition(ExecuteValues* values, D3DXMATRIX world, OUT D3DXVECTOR3* location)
{
	D3DXMATRIX W = world;
	W._41 = -world._41;
	W._42 = -world._42;
	W._43 = -world._43;
	D3DXVECTOR3 start;
	values->MainCamera->GetPosition(&start);
	D3DXVec3TransformCoord(&start, &start, &W);
	D3DXVECTOR3 direction;
	direction = values->MainCamera->GetDirection(values->Viewport, values->Perspective);

	D3DXVECTOR4 tmp;
	GetQuad(root, start, direction, tmp);

	for (UINT z = (UINT)tmp.y; z < (UINT)tmp.w; z++)
	{
		for (UINT x = (UINT)tmp.x; x < (UINT)tmp.z; x++)
		{
			UINT index0 = (width + 1) * z + x;
			UINT index1 = (width + 1) * (z + 1) + x;
			UINT index2 = (width + 1) * z + x + 1;
			UINT index3 = (width + 1) * (z + 1) + x + 1;

			D3DXVECTOR3 v0 = vertices[index0].position;
			D3DXVECTOR3 v1 = vertices[index1].position;
			D3DXVECTOR3 v2 = vertices[index2].position;
			D3DXVECTOR3 v3 = vertices[index3].position;

			float u, v, distance;
			if (D3DXIntersectTri(&v0, &v1, &v2, &start, &direction, &u, &v, &distance) == TRUE)
			{
				*location = v0 + (v1 - v0) * u + (v2 - v0) * v;
				
				return true;
			}

			if (D3DXIntersectTri(&v3, &v1, &v2, &start, &direction, &u, &v, &distance) == TRUE)
			{
				*location = v3 + (v1 - v3) * u + (v2 - v3) * v;

				return true;
			}
		}//for(x)
	}//for(z)

	*location = D3DXVECTOR3(-5, -5, -5);
	return false;
}

void Landscape::Data::SaveData(BinaryWriter * w)
{
	w->UInt(width);
	w->UInt(height);
	w->UInt(vertexCount);
	w->UInt(indexCount);	

	w->Byte(&indexData[0], sizeof(UINT) * indexCount);
	w->Byte(&vertices[0], sizeof(VertexType) * (width + 1) * (height + 1));
}

void Landscape::Data::LoadData(BinaryReader * r)
{
	Clear();

	width = r->UInt();
	height = r->UInt();
	vertexCount = r->UInt();
	indexCount = r->UInt();

	indexData = new UINT[indexCount];
	vertices = new VertexType[vertexCount];

	void* ptr;

	ptr = (void *)&(indexData[0]);
	r->Byte(&ptr, sizeof(UINT) * indexCount);

	ptr = (void *)&(vertices[0]);
	r->Byte(&ptr, sizeof(VertexType) * (vertexCount));

	FillNormalData();

	CreateBuffer();
}

void Landscape::Data::InitQuad(Quad * quad, float x, float z, float w, float h)
{
	quad->v1 = D3DXVECTOR3(x, 0, z);
	quad->v2 = D3DXVECTOR3(x + w, 0, z);
	quad->v3 = D3DXVECTOR3(x, 0, z + h);
	quad->v4 = D3DXVECTOR3(x + w, 0, z + h);

	float size = w * h;

	if (size > 32 * 32)
	{
		for (int i = 0; i < 4; i++)
		{
			float offsetX = i % 2 == 0 ? 0 : w / 2.0f;
			float offsetZ = i % 4 <  2 ? 0 : h / 2.0f;

			quad->quad[i] = new Quad();

			InitQuad(quad->quad[i], x + offsetX, z + offsetZ, w / 2.0f, h / 2.0f);
		}	
	}
}

BOOL Landscape::Data::GetQuad(Quad * quad, D3DXVECTOR3 start, D3DXVECTOR3 direction, D3DXVECTOR4& q)
{
	BOOL b = 0;
	float u, v, d;
	b |= D3DXIntersectTri(&quad->v1, &quad->v3, &quad->v2, &start, &direction, &u, &v, &d);
	b |= D3DXIntersectTri(&quad->v2, &quad->v3, &quad->v4, &start, &direction, &u, &v, &d);

	if (b)
	{
		if (quad->quad[0])
		{
			for (int i = 0; i < 4; i++)
			{
				if (GetQuad(quad->quad[i], start, direction, q))
					return true;
			}
		}
		else
		{
			q.x = quad->v1.x;
			q.y = quad->v1.z;
			q.z = quad->v4.x;
			q.w = quad->v4.z;
		}			
	}

	return b;
}

float Landscape::Data::GetAverage(UINT x, UINT y)
{
	float average = 0;
	int num = 0;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (y + j >= 0 && x + i >= 0 &&
				x + i < width + 1 &&
				y + j < height + 1)
			{
				average += vertices[(x + i) + (y + j)*(width + 1)].position.y;
				num++;
			}
		}
	}
	average /= num;
	return average;
}

void Landscape::Data::Clear()
{
	SAFE_DELETE(heightMap);

	SAFE_DELETE_ARRAY(indexData);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

D3DXVECTOR3 Landscape::Data::CalculateNormal(int w, int h)
{
	D3DXVECTOR3 n = D3DXVECTOR3(0, 0, 0);
	int index0 = w + (width + 1)* (h - 1);
	int index1 = (w + 1) + (width + 1)*(h - 1);
	int index2 = (w - 1) + (width + 1)* h;
	int index3 = w + (width + 1) * h;
	int index4 = (w + 1) + (width + 1) * h;
	int index5 = (w - 1) + (width + 1) * (h + 1);
	int index6 = w + (width + 1) * (h + 1);

	D3DXVECTOR3 d0, d1, normal;

	if (w > 0 && h > 0)
	{
		d0 = vertices[index0].position - vertices[index3].position;
		d1 = vertices[index2].position - vertices[index3].position;
		normal;
		D3DXVec3Cross(&normal, &d0, &d1);
		n += normal;
	}

	if (w < (int)width && h > 0)
	{
		d0 = vertices[index0].position - vertices[index1].position;
		d1 = vertices[index3].position - vertices[index1].position;
		D3DXVec3Cross(&normal, &d0, &d1);
		n += normal;

		d0 = vertices[index1].position - vertices[index4].position;
		d1 = vertices[index3].position - vertices[index4].position;
		D3DXVec3Cross(&normal, &d0, &d1);
		n += normal;
	}

	if (w > 0 && h < (int)height)
	{
		d0 = vertices[index2].position - vertices[index3].position;
		d1 = vertices[index5].position - vertices[index3].position;
		D3DXVec3Cross(&normal, &d0, &d1);
		n += normal;

		d0 = vertices[index3].position - vertices[index6].position;
		d1 = vertices[index5].position - vertices[index6].position;
		D3DXVec3Cross(&normal, &d0, &d1);
		n += normal;
	}

	if (w < (int)width && h < (int)height)
	{
		d0 = vertices[index3].position - vertices[index4].position;
		d1 = vertices[index6].position - vertices[index4].position;
		D3DXVec3Cross(&normal, &d0, &d1);
		n += normal;
	}

	D3DXVec3Normalize(&n, &n);
	return n;
}

void Landscape::Data::FillVertexData(int a, int b, float heightRatio)
{
	vector<D3DXCOLOR> pixels;
	D3D11_TEXTURE2D_DESC desc;

	width = a;
	height = b;

	if (heightMap)
	{
		desc = heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &pixels);

		width = desc.Width - 1;
		height = desc.Height - 1;
	}

	vertexCount = (width + 1) * (height + 1);
	vertices = new VertexType[vertexCount];

	UINT index = 0;
	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			vertices[index].color = D3DXCOLOR(0, 0, 0, 0);

			vertices[index].position.x = (float)x;

			if (heightMap)
				vertices[index].position.y = (float)(pixels[index].r * 255.0f) / heightRatio;
			else
				vertices[index].position.y = 0;

			vertices[index].position.z = (float)z;

			vertices[index].uv.x = x / (float)width * 2;
			vertices[index].uv.y = z / (float)height * 2;

			vertices[index].normal = D3DXVECTOR3(0, 0, 0);

			index++;
		}//for(x)
	}//for(y)	


	indexCount = width * height * 6;
	indexData = new UINT[indexCount];

	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			int index0 = (width + 1) * z + x;
			int index1 = (width + 1) * (z + 1) + x;
			int index2 = (width + 1) * z + x + 1;
			int index3 = (width + 1) * z + x + 1;
			int index4 = (width + 1) * (z + 1) + x;
			int index5 = (width + 1) * (z + 1) + (x + 1);

			indexData[index++] = index0;
			indexData[index++] = index1;
			indexData[index++] = index2;
			indexData[index++] = index3;
			indexData[index++] = index4;
			indexData[index++] = index5;
		}
	}
}

void Landscape::Data::FillNormalData()
{
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			int index0 = (width + 1) * z + x;
			int index1 = (width + 1) * (z + 1) + x;
			int index2 = (width + 1) * z + x + 1;
			int index3 = (width + 1) * z + x + 1;
			int index4 = (width + 1) * (z + 1) + x;
			int index5 = (width + 1) * (z + 1) + (x + 1);

			D3DXVECTOR3 v0 = vertices[index0].position;
			D3DXVECTOR3 v1 = vertices[index1].position;
			D3DXVECTOR3 v2 = vertices[index2].position;
			D3DXVECTOR3 v3 = vertices[index3].position;
			D3DXVECTOR3 v4 = vertices[index4].position;
			D3DXVECTOR3 v5 = vertices[index5].position;

			D3DXVECTOR3 d1 = v1 - v0;
			D3DXVECTOR3 d2 = v2 - v0;
			D3DXVECTOR3 d3 = v4 - v3;
			D3DXVECTOR3 d4 = v5 - v3;

			D3DXVECTOR3 n0, n1;
			D3DXVec3Cross(&n0, &d1, &d2);
			D3DXVec3Cross(&n1, &d3, &d4);
			
			D3DXVec3Normalize(&vertices[index0].normal, &(vertices[index0].normal + n0));
			D3DXVec3Normalize(&vertices[index1].normal, &(vertices[index1].normal + n0));
			D3DXVec3Normalize(&vertices[index2].normal, &(vertices[index2].normal + n0));
			D3DXVec3Normalize(&vertices[index3].normal, &(vertices[index3].normal + n1));
			D3DXVec3Normalize(&vertices[index4].normal, &(vertices[index4].normal + n1));
			D3DXVec3Normalize(&vertices[index5].normal, &(vertices[index5].normal + n1));
		}
	}
}

void Landscape::Data::CreateBuffer()
{
	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexType) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data = { 0 };
		data.pSysMem = indexData;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}