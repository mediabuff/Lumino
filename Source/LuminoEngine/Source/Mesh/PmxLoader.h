﻿
#pragma once
#include "Common.h"

LN_NAMESPACE_BEGIN
class BinaryReader;
class Texture2D;
class PmxSkinnedMeshResource;
namespace detail { class ModelManager; }

class PmxLoader
{
public:

#pragma pack(push, 1)

	enum PMX_Encode
	{
		PMX_Encode_UTF16	= 0,
		PMX_Encode_UTF8		= 1,
	};

	struct PMX_Header
	{
		char	Magic[4];		// "PMX " (PMX1.0は"Pmx ")
		float	Version;		// PMDバージョン番号
		byte_t	DataSize;		// 後続するデータ列のバイトサイズ  PMX2.0は 8 で固定
		byte_t	Data[8];		// [0] - エンコード方式  | 0:UTF16 1:UTF8
								// [1] - 追加UV数 	| 0～4 詳細は頂点参照
								// [2] - 頂点Indexサイズ | 1,2,4 のいずれか
								// [3] - テクスチャIndexサイズ | 1,2,4 のいずれか
								// [4] - 材質Indexサイズ | 1,2,4 のいずれか
								// [5] - ボーンIndexサイズ | 1,2,4 のいずれか
								// [6] - モーフIndexサイズ | 1,2,4 のいずれか
								// [7] - 剛体Indexサイズ | 1,2,4 のいずれか
	};

#pragma pack( pop )

public:
	PmxLoader();
	~PmxLoader();

	Ref<PmxSkinnedMeshResource> load(detail::ModelManager* manager, Stream* stream, const PathName& baseDir, bool isDynamic, ModelCreationFlag flags);

private:
	PMX_Encode getEncode() { return (PMX_Encode)m_pmxHeader.Data[0]; }
	int getAdditionalUVCount() { return m_pmxHeader.Data[1]; }
	int getVertexIndexSize() { return m_pmxHeader.Data[2]; }
	int getTextureIndexSize() { return m_pmxHeader.Data[3]; }
	int getMaterialIndexSize() { return m_pmxHeader.Data[4]; }
	int getBoneIndexSize() { return m_pmxHeader.Data[5]; }
	int getMorphIndexSize() { return m_pmxHeader.Data[6]; }
	int getRigidBodyIndexSize() { return m_pmxHeader.Data[7]; }

private:
	void loadModelInfo(BinaryReader* reader);
	void loadVertices(BinaryReader* reader);
	void loadIndices(BinaryReader* reader);
	void loadTextureTable(BinaryReader* reader, const PathName& baseDir);
	void loadMaterials(BinaryReader* reader);
	void loadBones(BinaryReader* reader);
	void loadMorphs(BinaryReader* reader);
	void loadDisplayFrame(BinaryReader* reader);
	void loadRigidBodys(BinaryReader* reader);
	void loadJoints(BinaryReader* reader);

	String readString(BinaryReader* reader);
	void calcSDEFCorrection();

private:
	detail::ModelManager*			m_manager;
	bool							m_isDynamic;
	ModelCreationFlag				m_flags;
	Ref<PmxSkinnedMeshResource>	m_modelCore;		///< 最終出力
	PMX_Header						m_pmxHeader;
	List<Ref<Texture>>			m_textureTable;
	ByteBuffer						m_tmpBuffer;
	bool							m_hasSDEF;
};

LN_NAMESPACE_END
