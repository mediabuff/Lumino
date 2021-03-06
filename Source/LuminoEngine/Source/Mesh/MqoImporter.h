﻿
#pragma once
#include <Lumino/Mesh/Mesh.h>
#include "SMesh.h"

LN_NAMESPACE_BEGIN
class Stream;
namespace detail {
class ModelManager;
class MqoImporter;
struct MqoFace;
struct MqoFacePointRef;
struct MqoFaceRef;
struct MqoEdge;
struct MqoFacePointGroup;

// 面上の点
struct MqoFacePoint
{
	MqoFace*			face;
	int					vertexIndex;			// オリジナルの頂点番号
	Vector3				normal;					// 頂点法線
	MqoFacePointGroup*	group = nullptr;		// 面上の点がどのグループに属しているか
	MqoFacePoint*		prev = nullptr;			// リスト構造の前のノード
	MqoFacePoint*		next = nullptr;			// リスト構造の次のノード
};

struct MqoFace
{
	// Mqo data
	int				vertexCount;		// 面を構成する頂点数 (3 or 4) 読み込まれた後、四角形は3角形に分割される
	int				vertexIndices[4];	// 面を構成する頂点のインデックス（頂点数分の数が存在）
	int				materialIndex;		// 材質インデックス
	Vector2			uv[4];				// UV値 (Vector2[4])
	uint32_t		colors[4];			// 頂点カラー

										// Work data
	Vector3			normal;				// 面法線
										//
	MqoEdge*		edges[3];			// 辺
										//Vector3			vertexNormals[3];			// 頂点法線 (これを使う時点では必ず三角形になっている)
										//MqoFacePointGroup*		groups[3] = {};			// 面上の点がどのグループに属しているか
	MqoFacePoint	points[3];
};

struct MqoVertex
{
	// Mqo data
	Vector3					position;
	List<MqoFacePointRef>	referenced;	// 頂点を参照している MqoFace の1点 (完全に同一要素を持つ点のうち1つ)

										// Work data
	MqoFaceRef*				faces = nullptr;	// この頂点を参照している面の LinkedList の先頭ノード
};

struct MqoFacePointRef
{
	//MqoFace*				face;
	int						faceIndex;
	int						pointIndex;
	int						meshVertexNumber;		// 頂点バッファに落とすときの頂点番号

													// MqoFacePointGroup で使われる情報
													//MqoFacePointRef*		next = nullptr;			// リスト構造の次のノード
};

// Edge 検索用の参考情報
struct MqoFaceRef
{
	MqoFace*				face = nullptr;			// 対応する MqoFace
	MqoFaceRef*				next = nullptr;			// リスト構造の次のノード
};

struct MqoEdge
{
	MqoFace*				ownerFace = nullptr;	// この Edge を持つ面
	MqoFacePoint*			point0 = nullptr;
	MqoFacePoint*			point1 = nullptr;
	//int						index0 = 0;				// MqoFace 上の始点の頂点インデックス (0-2)
	//int						index1 = 0;				// MqoFace 上の終点の頂点インデックス (0-2)
	MqoEdge*				adjacent = nullptr;			// 隣接辺 (単方向参照)
	MqoFacePoint*			adjacentPoint0 = nullptr;	// point0 に対する隣接辺上の point
	MqoFacePoint*			adjacentPoint1 = nullptr;	// point1 に対する隣接辺上の point
};

//struct MqoFacePointRef2
//{
//	MqoFacePoint*			point = nullptr;
//	MqoFacePointRef2*		next = nullptr;			// リスト構造の次のノード
//};

// スムージングの判定で、法線を共有する Face 上の点をまとめる (関係する頂点が1つしかなくてもグループを作る)
struct MqoFacePointGroup
{
	MqoFacePoint*			points = nullptr;		// 点のリスト
	int						vertexIndex;	// 頂点番号
	Vector3					normal;					// スムージングされた法線
	int						pointsCount = 0;
	int						outputVertexIndex = -1;	// 最終的な頂点バッファに出力した頂点番号 (0 以上になることで、既に出力されていることも表す)
};


class MqoObject : public RefObject
{
public:
	MqoImporter*			m_importer;
	String					m_name;
	List<MqoVertex>			m_mqoVertexList;
	List<MqoFace>			m_mqoFaceList;

	void Smoothing();
	Ref<MeshResource> createMeshResource();

private:
	void makeMqoFaceRefsAndEdge();
	void makeMqoFacePointNormals();
	void makeMqoFacePointGroup(MqoFacePoint* p0, MqoFacePoint* p1/*MqoEdge* edge*/);
};

class MqoImporter
{
public:
	MqoImporter();
	virtual ~MqoImporter() = default;

	Ref<StaticMeshModel> import(ModelManager* manager, const PathName& parentDir, Stream* stream);

private:

	void loadMaterials(StreamReader* reader);
	void loadObject(StreamReader* reader, const String& name);
	void readVertexChunk(StreamReader* reader, MqoObject* mqoObject);
	void readFaceChunk(StreamReader* reader, MqoObject* mqoObject);
	static void readInts(const StringRef& str, int* values, int valuesCount);
	static void readUInts(const StringRef& str, uint32_t* values, int valuesCount);
	static void readFloats(const StringRef& str, float* values, int valuesCount);


	void initMqoFace(MqoFace* face);
	//int AddFaceIndices(MeshResource* mesh, int startIndexBufferIndex, int faceIndex);
	//int PutVertexSource(int faceIndex, int pointIndex);
	//bool EqualsFacePoint(const MqoFace* face1, int pointIndex1, const MqoFace* face2, int pointIndex2);

	ModelManager*			m_manager;
	PathName				m_filePath;
	PathName				m_parentDir;
	List<Ref<MqoObject>>	m_mqoObjectList;
	//List<MqoVertex>			m_mqoVertexList;
	//List<MqoFace>			m_mqoFaceList;
	Ref<StaticMeshModel>	m_model;
	//int						m_meshIndexCount;
	//int						m_meshVertexCount;
	bool					m_flipZCoord;

	List<MqoFaceRef>				m_mqoFaceRefBuffer;
	List<MqoEdge>					m_mqoEdgeBuffer;
	//CacheBuffer<MqoFacePointRef2>	m_mqoFacePointRefBuffer_ForGroup;
	CacheBuffer<MqoFacePointGroup>	m_mqoFacePointGroupBuffer;

	friend class MqoObject;
};






class MqoParser
{
public:
	MqoParser();

	Ref<tr::SrMeshModel> import(ModelManager* manager, const PathName& filePath);
	Ref<StaticMeshModel> import2(ModelManager* manager, const PathName& filePath);

private:
	struct MqoFace
	{
		int				vertexCount;		// 面を構成する頂点数 (3 or 4) 読み込まれた後、四角形は3角形に分割される
		int				vertexIndices[4];	// 面を構成する頂点のインデックス（頂点数分の数が存在）
		int				materialIndex;		// 材質インデックス
		Vector2			uv[4];				// UV値
		uint32_t		colors[4];			// 頂点カラー
	};

	void loadMaterials(StreamReader* reader);
	void loadObject(StreamReader* reader);
	void readVertexChunk(StreamReader* reader);
	void readFaceChunk(StreamReader* reader);
	static void readInts(const StringRef& str, int* values, int valuesCount);
	static void readUInts(const StringRef& str, uint32_t* values, int valuesCount);
	static void readFloats(const StringRef& str, float* values, int valuesCount);

	void visitMaterialChunk();
	void visitMaterial(CommonMaterial* material);
	void visitObjectChunk(const StringRef& name);
	void visitVertexChunk(int vertexCount);
	void visitVertex(const Vector3& vertex, int index);
	void visitFaceChunk(int faceCount);
	void visitFace(const MqoFace& mqoFace);

	ModelManager*			m_manager;
	PathName				m_filePath;
	PathName				m_parentDir;
	Ref<tr::SrMeshModel>	m_model;
	tr::SrMesh*				m_mesh;
	bool					m_flipZCoord;
};


} // namespace detail
LN_NAMESPACE_END
