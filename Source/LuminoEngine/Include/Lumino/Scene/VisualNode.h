﻿
#pragma once
#include "Common.h"
#include "Detail.h"
#include <Lumino/Graphics/Color.h>
#include <Lumino/Graphics/Texture.h>
#include "SceneNode.h"
#include "WorldObject.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_SCENE_BEGIN
class MaterialList2;
class OffscreenWorldSubRenderView;

namespace detail {

struct OffscreenFilterInfo
{
	OffscreenWorldSubRenderView* ownerOffscreenView = nullptr;
	int					effectGroup = 0;
};

static const int MaxOffscreenId = 16;

} // namespace detail

/// VisualComponent
LN_CLASS()
class VisualComponent
	: public SceneNode
{
	LN_OBJECT;
public:



	//-------------------------------------------------------------------------
	/** @name Main material utilities. */
	/** @{ */

	//CommonMaterial* GetMainMaterial() const;

	//tr::ReflectionObjectList<CommonMaterial*>* GetMaterials() const;




	/** 不透明度を設定します。(default: 1.0) */
	void setOpacity(float value);

	/** 不透明度を取得します。 */
	float getOpacity() const;

	/** カラースケールを設定します。(default: Color(1, 1, 1, 1)) */
	void setColorScale(const Color& value);

	/** カラースケールを取得します。 */
	const Color& getColorScale() const;

	/** ブレンドカラーを設定します。(default: Color(0, 0, 0, 0)) */
	void setBlendColor(const Color& value);

	/** ブレンドカラーを取得します。 */
	const Color& getBlendColor() const;

	/** 色調を設定します。(default: Tone(0, 0, 0, 0)) */
	void setTone(const ToneF& value);

	/** 色調を取得します。 */
	const ToneF& getTone() const;

	/** シェーダを設定します。(default: nullptr) */
	void setShader(Shader* value);

	/** シェーダを取得します。 */
	Shader* getShader() const;





	///** メインマテリアルの不透明度を設定します。(default: 1.0)*/
	//void setOpacity(float value, int subsetIndex = -1);
	//float getOpacity() const;


	///** メインマテリアルのカラースケールを設定します。(default: )*/
	///// 乗算色の設定TODO: Color32
	//void setColorScale(const Color& value, int subsetIndex = -1);
	//void setColorScale(float r, float g, float b, float a = 1.0f, int subsetIndex = -1);
	//const Color& getColorScale() const;

	//void setColor(const Color32& value);
	//void setColor(int r, int g, int b, int a = 255);

	///// ブレンドカラーの設定 TODO: Color32
	//void setBlendColor(const Color& value, int subsetIndex = -1);

	///// 色調の設定 TODO: Tone32
	//void setTone(const ToneF& value, int subsetIndex = -1);


	/////// UV 変換行列の設定
	////void SetUVTransform(const Matrix& matrix, int subsetIndex = -1) { m_visualNodeParams.GetSubsetParams(subsetIndex).UVTransform = matrix; }

	/////// UV 変換行列の設定
	////const Matrix& GetUVTransform(int subsetIndex = -1)  const { return m_visualNodeParams.GetSubsetParams(subsetIndex).UVTransform; }

	///// シェーダの設定
	//void setShader(Shader* value, int subsetIndex = -1);

	/** @} */

	//-------------------------------------------------------------------------
	/** @name RenderState */
	/** @{ */

	/** このノードを描画する際の合成方法を設定します。デフォルトは BlendMode::Normal です。*/
	void setBlendMode(BlendMode mode);

	/** このノードを描画する際の合成方法を取得します。*/
	BlendMode getBlendMode() const;

	/** このノードを描画する際のカリング方法を設定します。デフォルトは CullingMode::Back です。*/
	void setCullingMode(CullingMode mode);

	/** このノードを描画する際のカリング方法を取得します。*/
	CullingMode getCullingMode() const;

	/** このノードを描画する際の深度テストの有無を設定します。デフォルトは true です。*/
	void setDepthTestEnabled(bool enabled);

	/** このノードを描画する際の深度テストの有無を取得します。*/
	bool isDepthTestEnabled() const;

	/** このノードを描画する際の深度書き込みの有無を設定します。デフォルトは true です。*/
	void setDepthWriteEnabled(bool enabled);

	/** このノードを描画する際の深度書き込みの有無を取得します。*/
	bool isDepthWriteEnabled() const;

	/** @} */

public:


	virtual SceneNodeType getSceneNodeType() const { return SceneNodeType_VisualNode; }
	virtual void updateFrameHierarchy(SceneNode* parent, float deltaTime) override;

	/// ノード単位の描画情報の更新。この後すぐ一連のサブセット描画が始まる
	//virtual void UpdateNodeRenderingParams(MMEShader* priorityShader) {}

	// obsolete
	virtual detail::Sphere getBoundingSphere();

	//void DrawSubsetInternal(SceneGraphRenderingContext* dc, int subsetIndex, MMEShader* shader, ShaderPass* pass);

	//virtual void onRender(SceneGraphRenderingContext* dc);
	//virtual void drawSubset(SceneGraphRenderingContext* dc, int subsetIndex) {}


protected:
	VisualComponent();
	virtual ~VisualComponent();
	void initialize();

LN_INTERNAL_ACCESS:
	//MaterialList2* GetMaterialList() { return m_materialList; }
	//Shader* GetPrimaryShader() const;
	detail::OffscreenFilterInfo* getOffscreenFilterInfo(int index) { return &m_offscreenFilterInfoList[index]; }

LN_PROTECTED_INTERNAL_ACCESS:
	virtual detail::SceneNodeDefaultShaderClass getShaderClass() { return detail::SceneNodeDefaultShaderClass_StaticMesh; }

//protected:
//	Ref<MaterialList2>	m_materialList;

private:
	void render(RenderingContext* context) override;

	//// TODO: Renderer への設定がまだ
	//BlendMode	m_blendMode;
	//CullingMode	m_cullingMode;
	//bool		m_depthTestEnabled;
	//bool		m_depthWriteEnabled;

	detail::OffscreenFilterInfo	m_offscreenFilterInfoList[detail::MaxOffscreenId];
};


/**
	@brief
*/
class MaterialList2
	: public tr::ReflectionObjectList<CommonMaterial*>	// SubMaterials (0 の場合もある)
{
public:
	CommonMaterial* GetMainMaterial() const;
	
LN_INTERNAL_ACCESS:
	MaterialList2();
	virtual ~MaterialList2();
	void initialize(int subMaterialCount, bool createMainMaterial);
	void copyShared(MaterialList* srcList, bool createMainMaterial);
	
LN_INTERNAL_ACCESS:
	Ref<CommonMaterial>				m_mainMaterial;
};




/**
	@brief		
*/
class VisualObject
	: public WorldObject
{
	LN_OBJECT;
public:

	/** 不透明度を設定します。(default: 1.0) */
	void setOpacity(float value);

	/** 不透明度を取得します。 */
	float getOpacity() const;

	/** カラースケールを設定します。(default: Color(1, 1, 1, 1)) */
	void setColorScale(const Color& value);

	/** カラースケールを設定します。(default: Color(1, 1, 1, 1)) */
	void setColorScale(float r, float g, float b, float a = 1.0f);

	/** カラースケールを取得します。 */
	const Color& getColorScale() const;

	/** ブレンドカラーを設定します。(default: Color(0, 0, 0, 0)) */
	void setBlendColor(const Color& value);

	/** ブレンドカラーを取得します。 */
	const Color& getBlendColor() const;

	/** 色調を設定します。(default: Tone(0, 0, 0, 0)) */
	void setTone(const ToneF& value);

	/** 色調を取得します。 */
	const ToneF& getTone() const;

	/** シェーダを設定します。(default: nullptr) */
	void setShader(Shader* value);

	/** シェーダを取得します。 */
	Shader* getShader() const;

	/** このノードを描画する際の合成方法を設定します。デフォルトは BlendMode::Normal です。*/
	void setBlendMode(BlendMode mode);

	/** このノードを描画する際の合成方法を取得します。*/
	BlendMode getBlendMode() const;

	/** このノードを描画する際のカリング方法を設定します。デフォルトは CullingMode::Back です。*/
	void setCullingMode(CullingMode mode);

	/** このノードを描画する際のカリング方法を取得します。*/
	CullingMode getCullingMode() const;

	/** このノードを描画する際の深度テストの有無を設定します。デフォルトは true です。*/
	void setDepthTestEnabled(bool enabled);

	/** このノードを描画する際の深度テストの有無を取得します。*/
	bool isDepthTestEnabled() const;

	/** このノードを描画する際の深度書き込みの有無を設定します。デフォルトは true です。*/
	void setDepthWriteEnabled(bool enabled);

	/** このノードを描画する際の深度書き込みの有無を取得します。*/
	bool isDepthWriteEnabled() const;


protected:
	virtual VisualComponent* getMainVisualComponent() const = 0;

LN_CONSTRUCT_ACCESS:
	VisualObject();
	virtual ~VisualObject();
	void initialize();
};

LN_NAMESPACE_SCENE_END
LN_NAMESPACE_END
