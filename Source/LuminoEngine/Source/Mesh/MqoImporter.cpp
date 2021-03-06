﻿/*
	Mqo ファイル Note
	- 面は時計回りを表とする。
*/
#include "../Internal.h"
#include <iostream>
#include <Lumino/Text/Encoding.h>
#include <Lumino/IO/StreamReader.h>
#include <Lumino/IO/FileManager.h>
#include "ModelManager.h"
#include "MqoImporter.h"

LN_NAMESPACE_BEGIN
namespace detail {

// 三角形ポリゴンの法線を求める
Vector3 TriangleNormal(const Vector3& p0, const Vector3& p1, const Vector3& p2)
{
	Vector3 v10 = p1 - p0;
	Vector3 v20 = p2 - p0;
	Vector3 nor = Vector3::cross(v20, v10);
	return Vector3::normalize(nor);
}

float DotAngleTo01Angle(float angle)
{
	return std::acosf(angle);//std::cosf( 1.0f - ((angle + 1.0f) / 2.0f));//((angle * -1.0f) + 1.0f) / 2.0f;
}


//==============================================================================
// MqoObject
//==============================================================================

//------------------------------------------------------------------------------
void MqoObject::Smoothing()
{
	// 先に三角形化しなければならない
	// 面法線を求める
	// 頂点に、それを参照している面をつめる
	for (int i = 0; i < m_mqoFaceList.getCount(); i++)
	{
		MqoFace* face = &m_mqoFaceList[i];
		if (face->vertexCount == 3)
		{
		}
		else if (face->vertexCount == 4)
		{
			face->vertexCount = 3;

			MqoFace newFace;
			newFace.vertexCount = 3;
			newFace.materialIndex = face->materialIndex;
			newFace.vertexIndices[0] = face->vertexIndices[0];
			newFace.vertexIndices[1] = face->vertexIndices[2];
			newFace.vertexIndices[2] = face->vertexIndices[3];
			newFace.uv[0] = face->uv[0];
			newFace.uv[1] = face->uv[2];
			newFace.uv[2] = face->uv[3];
			newFace.colors[0] = face->colors[0];
			newFace.colors[1] = face->colors[2];
			newFace.colors[2] = face->colors[3];
			m_mqoFaceList.add(newFace);
			face = &m_mqoFaceList[i];	// 再配置に備える
		}
		else
		{
			LN_NOTIMPLEMENTED();
		}

		// ついでに面法線を計算しておく
		MqoVertex& v0 = m_mqoVertexList[face->vertexIndices[0]];
		MqoVertex& v1 = m_mqoVertexList[face->vertexIndices[1]];
		MqoVertex& v2 = m_mqoVertexList[face->vertexIndices[2]];
		//v0.referenced.Add(MqoFacePointRef{ i, 0, 0 });
		//v1.referenced.Add(MqoFacePointRef{ i, 1, 0 });
		//v2.referenced.Add(MqoFacePointRef{ i, 2, 0 });
		face->normal = TriangleNormal(v0.position, v1.position, v2.position);


		face->points[0].vertexIndex = face->vertexIndices[0];
		face->points[1].vertexIndex = face->vertexIndices[1];
		face->points[2].vertexIndex = face->vertexIndices[2];
	}


	for (int i = 0; i < m_mqoFaceList.getCount(); i++)
	{
		MqoFace* face = &m_mqoFaceList[i];
		face->points[0].face = face;
		face->points[1].face = face;
		face->points[2].face = face;
	}


	makeMqoFaceRefsAndEdge();
	makeMqoFacePointNormals();

	// マテリアルインデックスでソート
	// (MqoVertex::referencedFaceIndices はもう使えなくなる)
	std::sort(m_mqoFaceList.begin(), m_mqoFaceList.end(), [](const MqoFace& lhs, const MqoFace& rhs) { return lhs.materialIndex < rhs.materialIndex; });

}

//------------------------------------------------------------------------------
Ref<MeshResource> MqoObject::createMeshResource()
{
	auto mesh = Ref<MeshResource>::makeRef();
	mesh->initialize(EngineDomain::getGraphicsManager(), MeshCreationFlags::None);
	mesh->setName(m_name);

	// 頂点バッファを作る
	// (頂点バッファにつめる頂点はグループが基点となる)
	mesh->resizeVertexBuffer(m_mqoFaceList.getCount() * 3);
	int iVertex = 0;
	for (int iFace = 0; iFace < m_mqoFaceList.getCount(); iFace++)
	{
		MqoFace* mqoFace = &m_mqoFaceList[iFace];
		LN_ASSERT(mqoFace->vertexCount == 3);
		for (int iPoint = 0; iPoint < mqoFace->vertexCount; iPoint++)
		{
			auto& point = mqoFace->points[iPoint];
			auto* group = point.group;
			if (group->outputVertexIndex < 0)	// 重複登録の対策。まだ1度も走査されていなければ頂点バッファに書き込む
			{
				auto& v = m_mqoVertexList[group->vertexIndex];
				Vector3 normal = group->normal;
				//if (m_flipZCoord) normal.z *= -1.0f;


				mesh->setPosition(iVertex, v.position);
				mesh->setNormal(iVertex, normal);
				mesh->setUV(iVertex, mqoFace->uv[iPoint]);
				mesh->setColor(iVertex, Color::White);	// TODO: 頂点色
				group->outputVertexIndex = iVertex;
				iVertex++;
			}
		}
	}

	// インデックスバッファを作る
	mesh->resizeIndexBuffer(m_mqoFaceList.getCount() * 3);
	int iIndex = 0;
	auto* firstGroup = &m_importer->m_mqoFacePointGroupBuffer.getAt(0);
	for (int iFace = 0; iFace < m_mqoFaceList.getCount(); iFace++)
	{
		MqoFace* mqoFace = &m_mqoFaceList[iFace];
		LN_ASSERT(mqoFace->vertexCount == 3);
		for (int iPoint = 0; iPoint < mqoFace->vertexCount; iPoint++)
		{
			int i = mqoFace->points[iPoint].group->outputVertexIndex;//mqoFace->points[iPoint].group - firstGroup;
			LN_ASSERT(i >= 0);

			mesh->setIndex(iIndex, i/* iFace * 3 + iPoint*/);
			iIndex++;
		}
	}



	//mesh->resizeVertexBuffer(m_mqoFacePointGroupBuffer.GetCount());
	//for (int iGroup = 0; iGroup < m_mqoFacePointGroupBuffer.GetCount(); iGroup++)
	//{
	//	auto& group = m_mqoFacePointGroupBuffer.GetAt(iGroup);
	//	auto& v = m_mqoVertexList[group.vertexIndex];

	//	Vector3 normal = group.normal;
	//	if (m_flipZCoord) normal.z *= -1.0f;

	//	mesh->setPosition(group.vertexIndex, v.position);
	//	mesh->setNormal(group.vertexIndex, normal);
	//	mesh->setUV(group.vertexIndex, m_mqoFaceList[ref.faceIndex].uv[ref.pointIndex]);
	//	mesh->setColor(group.vertexIndex, Color::White);	// TODO: 頂点色

	//	//Vector3& normal = m_mqoFaceList[ref.faceIndex].vertexNormals[ref.pointIndex];
	////		//if (m_flipZCoord) normal.z *= -1.0f;

	////		mesh->setPosition(ref.meshVertexNumber, vertex.position);
	////		mesh->setNormal(ref.meshVertexNumber, Vector3::UnitY/*normal*/);
	////		mesh->setUV(ref.meshVertexNumber, m_mqoFaceList[ref.faceIndex].uv[ref.pointIndex]);
	////		// TODO: 頂点色
	////		mesh->setColor(ref.meshVertexNumber, Color::White);
	//}

	// インデックスバッファを作る
	//mesh->resizeIndexBuffer(m_meshIndexCount);

	//// インデックスバッファを作りつつ、MqoVertex::referenced を作る
	{
		MeshAttribute sec;
		sec.MaterialIndex = 0;
		sec.StartIndex = 0;
		sec.PrimitiveNum = 0;
		sec.primitiveType = PrimitiveType_TriangleList;

		int next = 0;
		for (int iFace = 0; iFace < m_mqoFaceList.getCount(); iFace++)
		{
			const MqoFace& face = m_mqoFaceList[iFace];
			if (face.materialIndex != sec.MaterialIndex)
			{
				mesh->addMeshSection(sec);
				sec.MaterialIndex = face.materialIndex;
				sec.StartIndex = next;
				sec.PrimitiveNum = 0;
			}

			//next += AddFaceIndices(mesh, next, iFace);
			sec.PrimitiveNum += face.vertexCount - 2;
		}
		mesh->addMeshSection(sec);
	}

	//// 頂点バッファを作る
	//mesh->resizeVertexBuffer(m_meshVertexCount);
	//for (MqoVertex& vertex : m_mqoVertexList)
	//{
	//	if (m_flipZCoord) vertex.position.z *= -1.0f;

	//	for (MqoFacePointRef& ref : vertex.referenced)
	//	{
	//		//Vector3& normal = m_mqoFaceList[ref.faceIndex].vertexNormals[ref.pointIndex];
	//		//if (m_flipZCoord) normal.z *= -1.0f;

	//		mesh->setPosition(ref.meshVertexNumber, vertex.position);
	//		mesh->setNormal(ref.meshVertexNumber, Vector3::UnitY/*normal*/);
	//		mesh->setUV(ref.meshVertexNumber, m_mqoFaceList[ref.faceIndex].uv[ref.pointIndex]);
	//		// TODO: 頂点色
	//		mesh->setColor(ref.meshVertexNumber, Color::White);
	//	}
	//}

	return mesh;
}

//------------------------------------------------------------------------------
void MqoObject::makeMqoFaceRefsAndEdge()
{
	// 各面3頂点として MqoFaceRef,MqoEdge を作っておく
	m_importer->m_mqoFaceRefBuffer.resize(m_mqoFaceList.getCount() * 3);
	m_importer->m_mqoEdgeBuffer.resize(m_mqoFaceList.getCount() * 3);
	m_importer->m_mqoFacePointGroupBuffer.clearAndReserve(m_mqoFaceList.getCount() * 3);
	int mqoFaceRefBufferUsed = 0;
	int mqoEdgeBufferUsed = 0;

	// 各 MqoVertex に、その頂点を参照する面を示す MqoFaceRef を詰めていく
	for (int iFace = 0; iFace < m_mqoFaceList.getCount(); iFace++)
	{
		MqoFace* mqoFace = &m_mqoFaceList[iFace];

		LN_ASSERT(mqoFace->vertexCount == 3);
		for (int iVertex = 0; iVertex < mqoFace->vertexCount; iVertex++)
		{
			MqoFaceRef* newRef = &m_importer->m_mqoFaceRefBuffer[mqoFaceRefBufferUsed];
			mqoFaceRefBufferUsed++;

			MqoVertex* mqoVertex = &m_mqoVertexList[mqoFace->vertexIndices[iVertex]];
			newRef->face = mqoFace;
			newRef->next = mqoVertex->faces;
			mqoVertex->faces = newRef;
		}

		// エッジ情報を作る
		{
			MqoEdge* e1 = &m_importer->m_mqoEdgeBuffer[mqoEdgeBufferUsed];
			memset(e1, 0, sizeof(MqoEdge));
			mqoEdgeBufferUsed++;
			e1->ownerFace = mqoFace;
			e1->point0 = &mqoFace->points[0];
			e1->point1 = &mqoFace->points[1];
			mqoFace->edges[0] = e1;

			MqoEdge* e2 = &m_importer->m_mqoEdgeBuffer[mqoEdgeBufferUsed];
			memset(e2, 0, sizeof(MqoEdge));
			mqoEdgeBufferUsed++;
			e2->ownerFace = mqoFace;
			e2->point0 = &mqoFace->points[1];
			e2->point1 = &mqoFace->points[2];
			mqoFace->edges[1] = e2;

			MqoEdge* e3 = &m_importer->m_mqoEdgeBuffer[mqoEdgeBufferUsed];
			memset(e3, 0, sizeof(MqoEdge));
			mqoEdgeBufferUsed++;
			e3->ownerFace = mqoFace;
			e3->point0 = &mqoFace->points[2];
			e3->point1 = &mqoFace->points[0];
			mqoFace->edges[2] = e3;
		}
	}

	// 隣接辺を検索する
	for (int iEdge = 0; iEdge < m_importer->m_mqoEdgeBuffer.getCount(); iEdge++)
	{
		MqoEdge* edge = &m_importer->m_mqoEdgeBuffer[iEdge];

		// もう見つかっている場合は検索しなくてよい
		if (edge->adjacent != nullptr) continue;

		// edge に関係しそうな面をすべて捜査する
		MqoFace* ownerFace = edge->ownerFace;
		for (int iVertex = 0; iVertex < ownerFace->vertexCount; iVertex++)
		{
			MqoVertex* vertex = &m_mqoVertexList[ownerFace->vertexIndices[iVertex]];
			MqoFaceRef* faceRef = vertex->faces;
			while (faceRef != nullptr)
			{
				MqoFace* otherFace = faceRef->face;
				if (otherFace != ownerFace)
				{
					// ここから、edge に関係しそうな面のうち1つの処理

					for (int iOtherEdge = 0; iOtherEdge < 3; iOtherEdge++)
					{
						MqoEdge* otherEdge = otherFace->edges[iOtherEdge];
						int t0 = edge->point0->vertexIndex;//edge->ownerFace->vertexIndices[edge->index0];
						int t1 = edge->point1->vertexIndex;// edge->ownerFace->vertexIndices[edge->index1];
						int o0 = otherEdge->point0->vertexIndex;//otherEdge->ownerFace->vertexIndices[otherEdge->index0];
						int o1 = otherEdge->point1->vertexIndex; //otherEdge->ownerFace->vertexIndices[otherEdge->index1];

						if ((o0 == t0 && o1 == t1))
						{
							// 同じ頂点を参照する Edge を見つけた
							edge->adjacent = otherEdge;
							edge->adjacentPoint0 = otherEdge->point0;
							edge->adjacentPoint1 = otherEdge->point1;
							// 相手方にもついでに設定
							otherEdge->adjacent = edge;
							otherEdge->adjacentPoint0 = edge->point0;
							otherEdge->adjacentPoint1 = edge->point1;
							break;
						}
						else if (o1 == t0 && o0 == t1)	// こっちが一般的。↑の if に入る場合は裏表反転しているかもしれない。
						{
							// 同じ頂点を参照する Edge を見つけた
							edge->adjacent = otherEdge;
							edge->adjacentPoint0 = otherEdge->point1;
							edge->adjacentPoint1 = otherEdge->point0;
							// 相手方にもついでに設定
							otherEdge->adjacent = edge;
							otherEdge->adjacentPoint0 = edge->point1;
							otherEdge->adjacentPoint1 = edge->point0;
							break;
						}
					}
				}
				if (edge->adjacent != nullptr) break;

				faceRef = faceRef->next;
			}
		}
	}
}

//------------------------------------------------------------------------------
void MqoObject::makeMqoFacePointNormals()
{
	//float smoothThr = Math::DegreesToRadians(90.0f);
	float freq = 59.5;
	float smoothThr = (-1.0 * (freq / 90.0)) + 1.0f;	// 0～180 を、1～-1と考える

	for (int iEdge = 0; iEdge < m_importer->m_mqoEdgeBuffer.getCount(); iEdge++)
	{
		MqoEdge* edge = &m_importer->m_mqoEdgeBuffer[iEdge];

		bool smoothing = false;
		if (edge->adjacent != nullptr)
		{

			float d = Vector3::dot(edge->ownerFace->normal, edge->adjacent->ownerFace->normal);
			//if (DotAngleTo01Angle(d) < smoothThr)
			if (d >= smoothThr)
			{
				// スムージングする。エッジ両端の点に対してグループを作る
				makeMqoFacePointGroup(edge->point0, edge->adjacentPoint0);
				makeMqoFacePointGroup(edge->point1, edge->adjacentPoint1);
				smoothing = true;
			}
		}
		if (!smoothing)
		{
			// スムージングされない。独立した法線となる。
			if (edge->point0->group == nullptr)
			{
				edge->point0->group = m_importer->m_mqoFacePointGroupBuffer.request(1);
				edge->point0->group->vertexIndex = edge->point0->vertexIndex;
				//edge->point0->group->normal = edge->ownerFace->normal;
				edge->point0->group->pointsCount = 1;
				//edge->point0->group->points = m_mqoFacePointRefBuffer_ForGroup.request(1);
				edge->point0->group->points = edge->point0;
			}
			if (edge->point1->group == nullptr)
			{
				edge->point1->group = m_importer->m_mqoFacePointGroupBuffer.request(1);
				edge->point1->group->vertexIndex = edge->point1->vertexIndex;
				//edge->point1->group->normal = edge->ownerFace->normal;
				edge->point1->group->pointsCount = 1;
				//edge->point1->group->points = m_mqoFacePointRefBuffer_ForGroup.request(1);
				edge->point1->group->points = edge->point1;
			}

			//edge->ownerFace->points[edge->index0]->group = m_mqoFacePointGroupBuffer.request(1);
			//edge->ownerFace->groups[edge->index1] = m_mqoFacePointGroupBuffer.request(1);
			//edge->ownerFace->groups[edge->index0]->normal = edge->ownerFace->normal;
			//edge->ownerFace->groups[edge->index1]->normal = edge->ownerFace->normal;
			//edge->ownerFace->vertexNormals[edge->index0] += edge->ownerFace->normal;
			//edge->ownerFace->vertexNormals[edge->index1] += edge->ownerFace->normal;
		}
	}

	for (int iGroup = 0; iGroup < m_importer->m_mqoFacePointGroupBuffer.getCount(); iGroup++)
	{
		MqoFacePointGroup& g = m_importer->m_mqoFacePointGroupBuffer.getAt(iGroup);
		if (g.points != nullptr)
		{
			int count = 0;
			Vector3 normal;
			MqoFacePoint* point = g.points;
			while (point != nullptr)
			{
				normal += point->face->normal;
				count++;
				point = point->next;
			}
			LN_ASSERT(count > 0);

			g.normal = normal / count;
			g.normal.normalize();


		}
		else
		{
			// 点を1つも持たないグループは、グループのマージによって点が全て他のグループに移されたもの。
		}
	}

	//for (int iFace = 0; iFace < m_mqoFaceList.GetCount(); iFace++)
	//{
	//	MqoFace* mqoFace = &m_mqoFaceList[iFace];
	//	mqoFace->vertexNormals[0].normalize();
	//	mqoFace->vertexNormals[1].normalize();
	//	mqoFace->vertexNormals[2].normalize();
	//}
}

//------------------------------------------------------------------------------
// edge と 対となる edge の両端の点をグループ化する
void MqoObject::makeMqoFacePointGroup(MqoFacePoint* p0, MqoFacePoint* p1/*MqoEdge* edge*/)
{
	{
		//MqoFacePoint* p0 = edge->point0;
		//MqoFacePoint* p1 = edge->adjacentPoint0;

		if (p0->group == nullptr && p1->group == nullptr)
		{
			// どちらもまだグループに属していない。2点をまとめる1つのグループを作る

			MqoFacePointGroup* g = m_importer->m_mqoFacePointGroupBuffer.request(1);
			g->vertexIndex = p0->vertexIndex;
			g->pointsCount = 2;

			//MqoFacePointRef2* ref0 = m_mqoFacePointRefBuffer_ForGroup.request(1);
			//MqoFacePointRef2* ref1 = m_mqoFacePointRefBuffer_ForGroup.request(1);
			//ref0->point = p0;
			//ref1->point = p1;

			g->points = p0;
			p0->group = g;
			p0->prev = nullptr;
			p0->next = p1;
			p1->group = g;
			p1->prev = p0;
			p1->next = nullptr;
		}
		else if (p0->group != nullptr && p1->group != nullptr)
		{
			if (p0->group != p1->group)
			{
				// 両方の点がグループに属しており、そのグループが違う場合はマージする

				MqoFacePointGroup* g = p0->group;
				MqoFacePointGroup* g1 = p1->group;



				// 挿入先 (p0 group の終端) を探す
				MqoFacePoint* last = g->points;
				while (last->next != nullptr)
				{
					last = last->next;
				}

				// p0 group へ移す
				MqoFacePoint* g1first = g1->points;
				last->next = g1first;
				g1first->prev = last;
				MqoFacePoint* g1pt = g1first;
				while (g1pt != nullptr)
				{
					g1pt->group = g;
					g1pt = g1pt->next;
				}

				g->pointsCount += g1->pointsCount;

				g1->points = nullptr;
				g1->pointsCount = 0;

				//// p1 をグループからはずす
				//if (p1->prev != nullptr) p1->prev->next = p1->next;
				//if (p1->next != nullptr) p1->next->prev = p1->prev;
				//if (p1->group->points == p1)
				//{
				//	p1->group->points = p1->next;
				//}
				//p1->group->pointsCount--;

				//

				//// p1 を p0 のグループへ追加 (p0 の後ろへ)
				//if (p0->next != nullptr) p0->next->prev = p1;
				//p1->next = p0->next;
				//p1->prev = p0;
				//p0->next = p1;
				//p1->group = p0->group;
				//p1->group->pointsCount++;
			}
			else
			{

			}
		}
		else
		{
			MqoFacePointGroup* g = (p0->group != nullptr) ? p0->group : p1->group;	// このグループへ
			MqoFacePoint* p = (p0->group == nullptr) ? p0 : p1;						// p を追加する

																					// リストの先頭へ追加 (末尾は検索が必要なので)
			MqoFacePoint* next = g->points;
			p->prev = nullptr;
			p->next = next;
			next->prev = p;
			g->points = p;
			g->pointsCount++;

			p1->group = g;
			p0->group = g;
		}


		//MqoFacePointGroup* g = edge->adjacentPoint0->group;			// まず片方のグループを見る
		//if (g == nullptr) g = edge->point0->group;	// まだ作られていなければもう片方を見る
		//if (g == nullptr)									// どちらも作られていなければ新しくグループを作る
		//{
		//	g = m_mqoFacePointGroupBuffer.request(1);
		//	g->vertexIndex = edge->point0->vertexIndex;
		//	g->pointsCount = 1;	// 初回の追加では両端の2つが関連付けられる。もうひとつは↓で++
		//}

		//g->normal = edge->ownerFace->normal;// Vector3::UnitY;////edge->ownerFace->normal + edge->adjacent->ownerFace->normal;
		//g->pointsCount++;

		//edge->point0->group = g;
		//edge->adjacentPoint0->group = g;
	}
	{
		//MqoFacePointGroup* g = edge->adjacentPoint1->group;			// まず片方のグループを見る
		//if (g == nullptr) g = edge->point1->group;	// まだ作られていなければもう片方を見る
		//if (g == nullptr)									// どちらも作られていなければ新しくグループを作る
		//{
		//	g = m_mqoFacePointGroupBuffer.request(1);
		//	g->vertexIndex = edge->point1->vertexIndex;
		//	g->pointsCount = 1;	// 初回の追加では両端の2つが関連付けられる。もうひとつは↓で++
		//}

		//g->normal = edge->ownerFace->normal;// Vector3::UnitY; //// +edge->adjacent->ownerFace->normal;
		//g->pointsCount++;

		//edge->point1->group = g;
		////edge->adjacentPoint1->group = g;
	}
}

//==============================================================================
// MqoImporter
//==============================================================================

//------------------------------------------------------------------------------
MqoImporter::MqoImporter()
	: /*m_meshIndexCount(0)
	, m_meshVertexCount(0)
	, */m_flipZCoord(true)
{
}

double AngleOf2Vector(Vector3 A, Vector3 B)
{
	//ベクトルAとBの長さを計算する
	double length_A = A.getLength();
	double length_B = B.getLength();

	//内積とベクトル長さを使ってcosθを求める
	double cos_sita = Vector3::dot(A, B) / (length_A * length_B);

	//cosθからθを求める
	double sita = acos(cos_sita);

	//ラジアンでなく0～180の角度でほしい場合はコメント外す
	sita = sita * 180.0 / Math::PI;

	return sita;
}

//------------------------------------------------------------------------------
Ref<StaticMeshModel> MqoImporter::import(ModelManager* manager, const PathName& parentDir, Stream* stream)
{
	if (LN_REQUIRE(manager != nullptr)) return nullptr;

	m_parentDir = parentDir;

	//Ref<Stream> stream(file, false); //manager->getFileManager()->createFileStream(filePath)

	m_model = Ref<StaticMeshModel>::makeRef();
	m_model->initialize(manager->getGraphicsManager());

	// Metasequoia4 で出力される .mqo ファイルの文字コードは Shift_JIS だった
	StreamReader reader(stream, Encoding::getEncoding(EncodingType::SJIS));

	String line;
	while (reader.readLine(&line))
	{
		// CommonMaterial
		int index = line.indexOf(_LT("Material"));
		if (index > -1)
		{
			//int count = StringTraits::ToInt32(line.c_str() + index + 9);
			loadMaterials(&reader);
		}

		// Object
		index = line.indexOf(_LT("Object"));
		if (index > -1)
		{
			//int count = StringTraits::ToInt32(line.c_str() + index + 7);

			int nameBegin = line.indexOf(_LT('"'), index + 6) + 1;
			int nameEnd = line.indexOf(_LT('"'), nameBegin);


			loadObject(&reader, line.substring(nameBegin, nameEnd - nameBegin));
		}
	}

	for (auto& obj : m_mqoObjectList)
	{
		obj->Smoothing();
		m_model->addMeshResource(obj->createMeshResource());
	}



	return m_model;
}

//------------------------------------------------------------------------------
void MqoImporter::loadMaterials(StreamReader* reader)
{
	String line;
	while (reader->readLine(&line))
	{
		if (line.indexOf(_LT("}")) > -1) break;

		// マテリアル1つの情報は、\s 区切りの１行になっている。
		// 最初のデータはマテリアル名。これは飛ばす。
		int dataHead = 0;
		{
			int first = line.indexOf('"');
			int second = line.indexOf('"', first + 1);
			dataHead = second + 2;
		}

		Color color = Color::White;
		float diffuse = 1.0f;
		float ambient = 0.0f;
		float emissive = 0.5f;
		float specular = 0.0f;
		float power = 1.0f;
		Ref<Texture> texture;

		while (dataHead < line.getLength())
		{
			int numHead = line.indexOf('(', dataHead) + 1;
			int numEnd = line.indexOf(')', dataHead);
			int dataEnd = numEnd + 1;
			if (line[numHead] == '"')	// tex("ファイル名") に備える
			{
				numHead++;
				numEnd = line.indexOf(_LT("\")"), dataHead);
			}

			//if (line.IndexOf(_LT("shader"), dataHead, CaseSensitivity::CaseInsensitive)
			//if (line.IndexOf(_LT("vcol"), dataHead, CaseSensitivity::CaseInsensitive)	頂点カラー
			//if (line.IndexOf(_LT("dbls"), dataHead, CaseSensitivity::CaseInsensitive)	両面表示
			if (line.indexOf(_LT("col"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	色（ＲＧＢ）、不透明度
			{
				readFloats(StringRef(line.c_str() + numHead, line.c_str() + numEnd), reinterpret_cast<float*>(&color), 4);
			}
			if (line.indexOf(_LT("dif"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	拡散光	0～1
			{
				diffuse = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			if (line.indexOf(_LT("amb"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	周囲光	0～1
			{
				ambient = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			if (line.indexOf(_LT("emi"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	自己照明	0～1
			{
				emissive = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			if (line.indexOf(_LT("spc"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	反射光	0～1
			{
				specular = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			if (line.indexOf(_LT("power"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	反射光の強さ	0～100
			{
				power = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			//if (line.IndexOf(_LT("reflect"), dataHead, CaseSensitivity::CaseInsensitive)	鏡面反射 （Ver4.0以降)	0～1
			//if (line.IndexOf(_LT("refract"), dataHead, CaseSensitivity::CaseInsensitive)	屈折率 （Ver4.0以降)	1～5
			if (line.indexOf(_LT("tex"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	模様マッピング名	相対パスまたは絶対パスで記述
			{
				texture = m_manager->createTexture(m_parentDir, StringRef(line.c_str() + numHead, line.c_str() + numEnd), ModelCreationFlag::IgnoreTextureNotFound);
			}
			//if (line.IndexOf(_LT("aplane"), dataHead, CaseSensitivity::CaseInsensitive)	透明マッピング名	相対パスまたは絶対パスで記述
			//if (line.IndexOf(_LT("bump"), dataHead, CaseSensitivity::CaseInsensitive)	凹凸マッピング名	相対パスまたは絶対パスで記述
			//if (line.IndexOf(_LT("proj_type"), dataHead, CaseSensitivity::CaseInsensitive)	マッピング方式
			//if (line.IndexOf(_LT("proj_pos"), dataHead, CaseSensitivity::CaseInsensitive)	投影位置（ＸＹＺ）
			//if (line.IndexOf(_LT("proj_scale"), dataHead, CaseSensitivity::CaseInsensitive)	投影拡大率（ＸＹＺ）
			//if (line.IndexOf(_LT("proj_angle"), dataHead, CaseSensitivity::CaseInsensitive)	投影角度（ＨＰＢ） - 180～180

			dataHead = dataEnd + 1;
		}
		
		Color c;
		c.a = color.a;

		auto material = newObject<DiffuseMaterial>();

		c.r = diffuse * color.r;
		c.g = diffuse * color.g;
		c.b = diffuse * color.b;
		material->setDiffuse(c);

		c.r = ambient * color.r;
		c.g = ambient * color.g;
		c.b = ambient * color.b;
		material->setAmbient(c);

		c.r = emissive * color.r;
		c.g = emissive * color.g;
		c.b = emissive * color.b;
		material->setEmissive(c);

		c.r = specular * color.r;
		c.g = specular * color.g;
		c.b = specular * color.b;
		material->setSpecular(c);

		material->setSpecularPower(power);

		m_model->addMaterial(material);
	}
}

//------------------------------------------------------------------------------
void MqoImporter::loadObject(StreamReader* reader, const String& name)
{
	auto mqoObject = Ref<MqoObject>::makeRef();
	mqoObject->m_importer = this;
	mqoObject->m_name = name;

	//int vertexIndexOffset = m_mqoVertexList.GetCount();	// この Object チャンクに含まれる Vertex は、全体の何番目から始まるか？
	int index;
	String line;
	while (reader->readLine(&line))
	{
		if (line.indexOf(_LT("}")) > -1) break;

		if ((index = line.indexOf(_LT("vertex "))) >= 0)
		{
			int count = StringTraits::toInt32(line.c_str() + index + 7);
			mqoObject->m_mqoVertexList.reserve(count);
			readVertexChunk(reader, mqoObject);
		}
		else if ((index = line.indexOf(_LT("BVertex "))) >= 0)
		{
			LN_NOTIMPLEMENTED();
		}
		else if ((index = line.indexOf(_LT("face "))) >= 0)
		{
			int count = StringTraits::toInt32(line.c_str() + index + 5);
			mqoObject->m_mqoFaceList.reserve(count);
			readFaceChunk(reader, mqoObject);
		}
	}

	m_mqoObjectList.add(mqoObject);
}

//------------------------------------------------------------------------------
void MqoImporter::readVertexChunk(StreamReader* reader, MqoObject* mqoObject)
{
	String line;
	while (reader->readLine(&line))
	{
		if (line.indexOf(_LT("}")) > -1) break;

		MqoVertex v;
		readFloats(StringRef(line, 2), reinterpret_cast<float*>(&v.position), 3);
		mqoObject->m_mqoVertexList.add(v);
	}
}

//------------------------------------------------------------------------------
void MqoImporter::readFaceChunk(StreamReader* reader, MqoObject* mqoObject)
{
	String line;
	while (reader->readLine(&line))
	{
		if (line.indexOf(_LT("}")) > -1) break;

		MqoFace face;
		initMqoFace(&face);

		int dataHead = 2;	// 行頭 tab
		face.vertexCount = StringTraits::toInt32(line.c_str() + dataHead);

		//if (face.vertexCount == 3) m_meshIndexCount += 3;
		//else if (face.vertexCount == 4) m_meshIndexCount += 6;
		//else LN_NOTIMPLEMENTED();

		dataHead += 2;		// 頂点数, space

		while (dataHead < line.getLength())
		{
			int numHead = line.indexOf('(', dataHead) + 1;
			int numEnd = line.indexOf(')', dataHead);
			int dataEnd = numEnd + 1;
			if (line[numHead] == '"')	// tex("ファイル名") に備える
			{
				numHead++;
				numEnd = line.indexOf(_LT("\")"), dataHead);
			}

			// V(%d ...)	頂点インデックス
			else if (StringTraits::compare(line.c_str() + dataHead, _LT("V"), 1, CaseSensitivity::CaseInsensitive) == 0)
			{
				readInts(StringRef(line, numHead, numEnd - numHead), face.vertexIndices, face.vertexCount);
				//for (int i = 0; i < face.vertexCount; i++) face.vertexIndices[i] += vertexIndexOffset;
			}
			// M(%d)	材質インデックス
			else if (StringTraits::compare(line.c_str() + dataHead, _LT("M"), 1, CaseSensitivity::CaseInsensitive) == 0)
			{
				face.materialIndex = StringTraits::toInt32(line.c_str() + numHead, numEnd - numHead);
			}
			// UV(%.5f %.5f ...)	ＵＶ値
			else if (StringTraits::compare(line.c_str() + dataHead, _LT("UV"), 2, CaseSensitivity::CaseInsensitive) == 0)
			{
				readFloats(StringRef(line, numHead, numEnd - numHead), reinterpret_cast<float*>(face.uv), face.vertexCount * 2);
			}
			// COL(%u)	頂点カラー
			else if (StringTraits::compare(line.c_str() + dataHead, _LT("COL"), 3, CaseSensitivity::CaseInsensitive) == 0)
			{
				readUInts(StringRef(line, numHead, numEnd - numHead), face.colors, face.vertexCount);
			}
			// CRS(%f ...)	Catmull-Clark/OpenSubdiv曲面用のエッジの折れ目
			//else if (line.Compare(_LT("CRS"), 3, CaseSensitivity::CaseInsensitive) == 0)
			//{
			//}

			dataHead = dataEnd + 1;
		}


		mqoObject->m_mqoFaceList.add(face);
	}
}

//------------------------------------------------------------------------------
void MqoImporter::readInts(const StringRef& str, int* values, int valuesCount)
{
	int* valuesEnd = values + valuesCount;
	StringTraits::SplitHelper(
		str.getBegin(), str.getEnd(), _LT(" "), 1, StringSplitOptions::RemoveEmptyEntries, CaseSensitivity::CaseSensitive,
		[&values, &valuesEnd](const Char* begin, const Char* end)
		{
			if (values < valuesEnd)
			{
				*values = StringTraits::toInt32(begin, end - begin);
				values++;
			}
		});
}

//------------------------------------------------------------------------------
void MqoImporter::readUInts(const StringRef& str, uint32_t* values, int valuesCount)
{
	uint32_t* valuesEnd = values + valuesCount;
	StringTraits::SplitHelper(
		str.getBegin(), str.getEnd(), _LT(" "), 1, StringSplitOptions::RemoveEmptyEntries, CaseSensitivity::CaseSensitive,
		[&values, &valuesEnd](const Char* begin, const Char* end)
	{
		if (values < valuesEnd)
		{
			*values = StringTraits::toUInt32(begin, end - begin);
			values++;
		}
	});
}

//------------------------------------------------------------------------------
void MqoImporter::readFloats(const StringRef& str, float* values, int valuesCount)
{
	float* valuesEnd = values + valuesCount;
	StringTraits::SplitHelper(
		str.getBegin(), str.getEnd(), _LT(" "), 1, StringSplitOptions::RemoveEmptyEntries, CaseSensitivity::CaseSensitive,
		[&values, &valuesEnd](const Char* begin, const Char* end)
		{
			if (values < valuesEnd)
			{
				*values = StringTraits::toFloat(begin, end - begin);
				values++;
			}
		});
}


//------------------------------------------------------------------------------
void MqoImporter::initMqoFace(MqoFace* face)
{
	memset(face, 0, sizeof(MqoFace));
	face->colors[0] = face->colors[1] = face->colors[2] = face->colors[3] = 0xFFFFFFFF;
}

//------------------------------------------------------------------------------
//int MqoImporter::AddFaceIndices(MeshResource* mesh, int startIndexBufferIndex, int faceIndex)
//{
//	MqoFace* face = &m_mqoFaceList[faceIndex];
//	if (face->vertexCount == 3)
//	{
//		int i0 = PutVertexSource(faceIndex, 0);
//		int i1 = PutVertexSource(faceIndex, 1);
//		int i2 = PutVertexSource(faceIndex, 2);
//		if (m_flipZCoord)
//		{
//			mesh->setIndex(startIndexBufferIndex + 0, i0);
//			mesh->setIndex(startIndexBufferIndex + 1, i2);
//			mesh->setIndex(startIndexBufferIndex + 2, i1);
//		}
//		else
//		{
//			mesh->setIndex(startIndexBufferIndex + 0, i0);
//			mesh->setIndex(startIndexBufferIndex + 1, i1);
//			mesh->setIndex(startIndexBufferIndex + 2, i2);
//		}
//		return 3;
//	}
//	//else if (face->vertexCount == 4)
//	//{
//	//	int i0 = PutVertexSource(faceIndex, 0);
//	//	int i1 = PutVertexSource(faceIndex, 1);
//	//	int i2 = PutVertexSource(faceIndex, 2);
//	//	int i3 = PutVertexSource(faceIndex, 3);
//	//	mesh->setIndex(startIndexBufferIndex + 0, i0);
//	//	mesh->setIndex(startIndexBufferIndex + 1, i1);
//	//	mesh->setIndex(startIndexBufferIndex + 2, i3);
//	//	mesh->setIndex(startIndexBufferIndex + 3, i3);
//	//	mesh->setIndex(startIndexBufferIndex + 4, i1);
//	//	mesh->setIndex(startIndexBufferIndex + 5, i2);
//	//	return 6;
//	//}
//	else
//	{
//		LN_NOTIMPLEMENTED();
//	}
//	return 0;
//}
//
////------------------------------------------------------------------------------
//// 指定した面上の1点と全く同じ点が既に登録されていればそのインデックスを返す。
//// そうでなければ新しく登録する。
//int MqoImporter::PutVertexSource(int faceIndex, int pointIndex)
//{
//	MqoFace* face = &m_mqoFaceList[faceIndex];
//	int vertexIndex = face->vertexIndices[pointIndex];
//	MqoVertex& mqoVertex = m_mqoVertexList[vertexIndex];
//	for (MqoFacePointRef& ref : mqoVertex.referenced)
//	{
//		if (EqualsFacePoint(face, pointIndex, &m_mqoFaceList[ref.faceIndex], ref.pointIndex))
//		{
//			return ref.meshVertexNumber;
//		}
//	}
//
//	// new
//	MqoFacePointRef ref;
//	ref.faceIndex = faceIndex;
//	ref.pointIndex = pointIndex;
//	ref.meshVertexNumber = m_meshVertexCount;
//	mqoVertex.referenced.Add(ref);
//	m_meshVertexCount++;
//
//	return ref.meshVertexNumber;
//}
//
////------------------------------------------------------------------------------
//bool MqoImporter::EqualsFacePoint(const MqoFace* face1, int pointIndex1, const MqoFace* face2, int pointIndex2)
//{
//	if (face1->vertexIndices[pointIndex1] != face2->vertexIndices[pointIndex2]) return false;
//	if (face1->uv[pointIndex1] != face2->uv[pointIndex2]) return false;
//	if (face1->colors[pointIndex1] != face2->colors[pointIndex2]) return false;
//	//if (face1->vertexNormals[pointIndex1] != face2->vertexNormals[pointIndex2]) return false;
//	return true;
//}









//==============================================================================
// MqoParser
//==============================================================================

MqoParser::MqoParser()
	: m_flipZCoord(true)
{
}

Ref<tr::SrMeshModel> MqoParser::import(ModelManager* manager, const PathName& filePath)
{
	if (LN_REQUIRE(manager != nullptr)) return nullptr;

	m_manager = manager;
	m_parentDir = filePath.getParent();
	m_model = newObject<tr::SrMeshModel>();

	// Metasequoia4 で出力される .mqo ファイルの文字コードは Shift_JIS だった
	StreamReader reader(filePath.c_str(), Encoding::getEncoding(EncodingType::SJIS));

	String line;
	while (reader.readLine(&line))
	{
		// CommonMaterial
		int index = line.indexOf(_LT("Material"));
		if (index > -1)
		{
			visitMaterialChunk();
			loadMaterials(&reader);
			continue;
		}

		// Object
		index = line.indexOf(_LT("Object"));
		if (index > -1)
		{
			int nameBegin = line.indexOf(_LT('"'), index + 6) + 1;
			int nameEnd = line.indexOf(_LT('"'), nameBegin);
			visitObjectChunk(line.substring(nameBegin, nameEnd - nameBegin));
			loadObject(&reader);
			continue;
		}
	}

	return m_model;
}

Ref<StaticMeshModel> MqoParser::import2(ModelManager* manager, const PathName& filePath)
{
	auto mesh = import(manager, filePath);
	mesh->calculateNormals();
	return mesh->generateStaticMeshModel();
}

void MqoParser::loadMaterials(StreamReader* reader)
{
	String line;
	while (reader->readLine(&line))
	{
		if (line.indexOf(_LT("}")) > -1) break;

		// マテリアル1つの情報は、\s 区切りの１行になっている。
		// 最初のデータはマテリアル名。これは飛ばす。
		int dataHead = 0;
		{
			int first = line.indexOf('"');
			int second = line.indexOf('"', first + 1);
			dataHead = second + 2;
		}

		Color color = Color::White;
		float diffuse = 1.0f;
		float ambient = 0.0f;
		float emissive = 0.5f;
		float specular = 0.0f;
		float power = 1.0f;
		Ref<Texture> texture;

		while (dataHead < line.getLength())
		{
			int numHead = line.indexOf('(', dataHead) + 1;
			int numEnd = line.indexOf(')', dataHead);
			int dataEnd = numEnd + 1;
			if (line[numHead] == '"')	// tex("ファイル名") に備える
			{
				numHead++;
				numEnd = line.indexOf(_LT("\")"), dataHead);
			}

			//if (line.IndexOf(_LT("shader"), dataHead, CaseSensitivity::CaseInsensitive)
			//if (line.IndexOf(_LT("vcol"), dataHead, CaseSensitivity::CaseInsensitive)	頂点カラー
			//if (line.IndexOf(_LT("dbls"), dataHead, CaseSensitivity::CaseInsensitive)	両面表示
			if (line.indexOf(_LT("col"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	色（ＲＧＢ）、不透明度
			{
				readFloats(StringRef(line.c_str() + numHead, line.c_str() + numEnd), reinterpret_cast<float*>(&color), 4);
			}
			if (line.indexOf(_LT("dif"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	拡散光	0～1
			{
				diffuse = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			if (line.indexOf(_LT("amb"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	周囲光	0～1
			{
				ambient = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			if (line.indexOf(_LT("emi"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	自己照明	0～1
			{
				emissive = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			if (line.indexOf(_LT("spc"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	反射光	0～1
			{
				specular = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			if (line.indexOf(_LT("power"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	反射光の強さ	0～100
			{
				power = StringTraits::toFloat(line.c_str() + numHead, dataEnd - numHead);
			}
			//if (line.IndexOf(_LT("reflect"), dataHead, CaseSensitivity::CaseInsensitive)	鏡面反射 （Ver4.0以降)	0～1
			//if (line.IndexOf(_LT("refract"), dataHead, CaseSensitivity::CaseInsensitive)	屈折率 （Ver4.0以降)	1～5
			if (line.indexOf(_LT("tex"), dataHead, CaseSensitivity::CaseInsensitive) == dataHead) //	模様マッピング名	相対パスまたは絶対パスで記述
			{
				texture = m_manager->createTexture(m_parentDir, StringRef(line.c_str() + numHead, line.c_str() + numEnd), ModelCreationFlag::IgnoreTextureNotFound);
			}
			//if (line.IndexOf(_LT("aplane"), dataHead, CaseSensitivity::CaseInsensitive)	透明マッピング名	相対パスまたは絶対パスで記述
			//if (line.IndexOf(_LT("bump"), dataHead, CaseSensitivity::CaseInsensitive)	凹凸マッピング名	相対パスまたは絶対パスで記述
			//if (line.IndexOf(_LT("proj_type"), dataHead, CaseSensitivity::CaseInsensitive)	マッピング方式
			//if (line.IndexOf(_LT("proj_pos"), dataHead, CaseSensitivity::CaseInsensitive)	投影位置（ＸＹＺ）
			//if (line.IndexOf(_LT("proj_scale"), dataHead, CaseSensitivity::CaseInsensitive)	投影拡大率（ＸＹＺ）
			//if (line.IndexOf(_LT("proj_angle"), dataHead, CaseSensitivity::CaseInsensitive)	投影角度（ＨＰＢ） - 180～180

			dataHead = dataEnd + 1;
		}

		Color c;
		c.a = color.a;

		auto material = newObject<DiffuseMaterial>();

		c.r = diffuse * color.r;
		c.g = diffuse * color.g;
		c.b = diffuse * color.b;
		material->setDiffuse(c);

		c.r = ambient * color.r;
		c.g = ambient * color.g;
		c.b = ambient * color.b;
		material->setAmbient(c);

		c.r = emissive * color.r;
		c.g = emissive * color.g;
		c.b = emissive * color.b;
		material->setEmissive(c);

		c.r = specular * color.r;
		c.g = specular * color.g;
		c.b = specular * color.b;
		material->setSpecular(c);

		material->setSpecularPower(power);
		material->setMaterialTexture(texture);

		visitMaterial(material);
	}
}

void MqoParser::loadObject(StreamReader* reader)
{
	int index;
	String line;
	while (reader->readLine(&line))
	{
		if (line.indexOf(_LT("}")) > -1) break;

		if ((index = line.indexOf(_LT("vertex "))) >= 0)
		{
			int count = StringTraits::toInt32(line.c_str() + index + 7);
			visitVertexChunk(count);
			readVertexChunk(reader);
		}
		else if ((index = line.indexOf(_LT("BVertex "))) >= 0)
		{
			LN_NOTIMPLEMENTED();
		}
		else if ((index = line.indexOf(_LT("face "))) >= 0)
		{
			int count = StringTraits::toInt32(line.c_str() + index + 5);
			visitFaceChunk(count);
			readFaceChunk(reader);
		}
	}
}

void MqoParser::readVertexChunk(StreamReader* reader)
{
	String line;
	int index = 0;
	while (reader->readLine(&line))
	{
		if (line.indexOf(_LT("}")) > -1) break;

		Vector3 v;
		readFloats(StringRef(line, 2), reinterpret_cast<float*>(&v), 3);
		visitVertex(v, index);
		index++;
	}
}

void MqoParser::readFaceChunk(StreamReader* reader)
{
	String line;
	while (reader->readLine(&line))
	{
		if (line.indexOf(_LT("}")) > -1) break;

		MqoFace face;
		memset(&face, 0, sizeof(MqoFace));
		face.colors[0] = face.colors[1] = face.colors[2] = face.colors[3] = 0xFFFFFFFF;

		int dataHead = 2;	// 行頭 tab
		face.vertexCount = StringTraits::toInt32(line.c_str() + dataHead);

		//if (face.vertexCount == 3) m_meshIndexCount += 3;
		//else if (face.vertexCount == 4) m_meshIndexCount += 6;
		//else LN_NOTIMPLEMENTED();

		dataHead += 2;		// 頂点数, space

		while (dataHead < line.getLength())
		{
			int numHead = line.indexOf('(', dataHead) + 1;
			int numEnd = line.indexOf(')', dataHead);
			int dataEnd = numEnd + 1;
			if (line[numHead] == '"')	// tex("ファイル名") に備える
			{
				numHead++;
				numEnd = line.indexOf(_LT("\")"), dataHead);
			}

			// V(%d ...)	頂点インデックス
			else if (StringTraits::compare(line.c_str() + dataHead, _LT("V"), 1, CaseSensitivity::CaseInsensitive) == 0)
			{
				readInts(StringRef(line, numHead, numEnd - numHead), face.vertexIndices, face.vertexCount);
				//for (int i = 0; i < face.vertexCount; i++) face.vertexIndices[i] += vertexIndexOffset;
			}
			// M(%d)	材質インデックス
			else if (StringTraits::compare(line.c_str() + dataHead, _LT("M"), 1, CaseSensitivity::CaseInsensitive) == 0)
			{
				face.materialIndex = StringTraits::toInt32(line.c_str() + numHead, numEnd - numHead);
			}
			// UV(%.5f %.5f ...)	ＵＶ値
			else if (StringTraits::compare(line.c_str() + dataHead, _LT("UV"), 2, CaseSensitivity::CaseInsensitive) == 0)
			{
				readFloats(StringRef(line, numHead, numEnd - numHead), reinterpret_cast<float*>(face.uv), face.vertexCount * 2);
			}
			// COL(%u)	頂点カラー
			else if (StringTraits::compare(line.c_str() + dataHead, _LT("COL"), 3, CaseSensitivity::CaseInsensitive) == 0)
			{
				readUInts(StringRef(line, numHead, numEnd - numHead), face.colors, face.vertexCount);
			}
			// CRS(%f ...)	Catmull-Clark/OpenSubdiv曲面用のエッジの折れ目
			//else if (line.Compare(_LT("CRS"), 3, CaseSensitivity::CaseInsensitive) == 0)
			//{
			//}

			dataHead = dataEnd + 1;
		}

		visitFace(face);
	}
}

void MqoParser::readInts(const StringRef& str, int* values, int valuesCount)
{
	int* valuesEnd = values + valuesCount;
	StringTraits::SplitHelper(
		str.getBegin(), str.getEnd(), _LT(" "), 1, StringSplitOptions::RemoveEmptyEntries, CaseSensitivity::CaseSensitive,
		[&values, &valuesEnd](const Char* begin, const Char* end)
	{
		if (values < valuesEnd)
		{
			*values = StringTraits::toInt32(begin, end - begin);
			values++;
		}
	});
}

void MqoParser::readUInts(const StringRef& str, uint32_t* values, int valuesCount)
{
	uint32_t* valuesEnd = values + valuesCount;
	StringTraits::SplitHelper(
		str.getBegin(), str.getEnd(), _LT(" "), 1, StringSplitOptions::RemoveEmptyEntries, CaseSensitivity::CaseSensitive,
		[&values, &valuesEnd](const Char* begin, const Char* end)
	{
		if (values < valuesEnd)
		{
			*values = StringTraits::toUInt32(begin, end - begin);
			values++;
		}
	});
}

void MqoParser::readFloats(const StringRef& str, float* values, int valuesCount)
{
	float* valuesEnd = values + valuesCount;
	StringTraits::SplitHelper(
		str.getBegin(), str.getEnd(), _LT(" "), 1, StringSplitOptions::RemoveEmptyEntries, CaseSensitivity::CaseSensitive,
		[&values, &valuesEnd](const Char* begin, const Char* end)
	{
		if (values < valuesEnd)
		{
			*values = StringTraits::toFloat(begin, end - begin);
			values++;
		}
	});
}

void MqoParser::visitMaterialChunk()
{
}

void MqoParser::visitMaterial(CommonMaterial* material)
{
	m_model->addMaterial(material);
}

void MqoParser::visitObjectChunk(const StringRef& name)
{
	m_mesh = m_model->addMesh();
}

void MqoParser::visitVertexChunk(int vertexCount)
{
	m_mesh->addVertices(vertexCount);
}

void MqoParser::visitVertex(const Vector3& vertex, int index)
{
	tr::SrMeshOperation::moveVertex(m_mesh->getVertex(index), vertex);
}

void MqoParser::visitFaceChunk(int faceCount)
{
}

void MqoParser::visitFace(const MqoFace& mqoFace)
{
	tr::SrFace* face = m_mesh->makeFace(mqoFace.vertexIndices, mqoFace.vertexCount);
	for (int i = 0; i < mqoFace.vertexCount; i++)
	{
		face->m_material = m_model->getMaterial(mqoFace.materialIndex);

		face->m_loops[i]->uv = mqoFace.uv[i];

		uint32_t c = mqoFace.colors[i];
		face->m_loops[i]->color.r = static_cast<float>(c & 0x000000FF) / 255.0f;
		face->m_loops[i]->color.g = static_cast<float>((c & 0x0000FF00) >> 8) / 255.0f;
		face->m_loops[i]->color.b = static_cast<float>((c & 0x00FF0000) >> 16) / 255.0f;
		face->m_loops[i]->color.a = static_cast<float>((c & 0xFF000000) >> 24) / 255.0f;
	}
}

} // namespace detail
LN_NAMESPACE_END
