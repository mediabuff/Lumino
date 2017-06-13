﻿
#include "../Internal.h"
#include <Lumino/Graphics/GraphicsContext.h>
#include <Lumino/Scene/Camera.h>
#include <Lumino/Scene/Light.h>
#include <Lumino/Scene/VisualNode.h>
#include <Lumino/Scene/SceneGraph.h>
#include "../EngineDiagCore.h"
#include "SceneGraphManager.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_SCENE_BEGIN

//==============================================================================
// MaterialList2
//==============================================================================

//------------------------------------------------------------------------------
MaterialList2::MaterialList2()
{
}

//------------------------------------------------------------------------------
MaterialList2::~MaterialList2()
{
}

//------------------------------------------------------------------------------
void MaterialList2::initialize(int subMaterialCount, bool createMainMaterial)
{
	resize(subMaterialCount);
	for (int i = 0; i < subMaterialCount; ++i)
	{
		auto m = RefPtr<Material>::makeRef();	// TODO
		getAt(i, m);
	}

	if (createMainMaterial)
	{
		m_mainMaterial = RefPtr<Material>::makeRef();	// TODO
	}
}

//------------------------------------------------------------------------------
void MaterialList2::copyShared(MaterialList* srcList, bool createMainMaterial)
{
	if (LN_CHECK_ARG(srcList != nullptr)) return;

	resize(srcList->getCount());
	for (int i = 0; i < srcList->getCount(); ++i)
	{
		getAt(i, srcList->getAt(i));
	}


	if (createMainMaterial)
	{
		m_mainMaterial = RefPtr<Material>::makeRef();
	}
	else if (getCount() > 0)
	{
		m_mainMaterial = getAt(0);
	}
}

//------------------------------------------------------------------------------
Material* MaterialList2::GetMainMaterial() const
{
	return m_mainMaterial;
}


//==============================================================================
// VisualComponent
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(VisualComponent, SceneNode);

//------------------------------------------------------------------------------
VisualComponent::VisualComponent()
	: m_blendMode(BlendMode::Normal)
	, m_cullingMode(CullingMode::Back)
	, m_depthTestEnabled(true)
	, m_depthWriteEnabled(true)
{
	setSpecialComponentType(SpecialComponentType::Visual);
}

//------------------------------------------------------------------------------
VisualComponent::~VisualComponent()
{
}

//------------------------------------------------------------------------------
void VisualComponent::initialize()
{
	SceneNode::initialize();
}

////------------------------------------------------------------------------------
//Material* VisualComponent::GetMainMaterial() const
//{
//	return m_materialList->GetMainMaterial();
//}
//
////------------------------------------------------------------------------------
//tr::ReflectionObjectList<Material*>* VisualComponent::GetMaterials() const
//{
//	return m_materialList;
//}

//------------------------------------------------------------------------------
void VisualComponent::setOpacity(float value)
{
	m_builtinEffectData.setOpacity(value);
}

//------------------------------------------------------------------------------
float VisualComponent::getOpacity() const
{
	return m_builtinEffectData.getOpacity();
}

//------------------------------------------------------------------------------
void VisualComponent::setColorScale(const Color& value)
{
	m_builtinEffectData.setColorScale(value);
}

//------------------------------------------------------------------------------
const Color& VisualComponent::getColorScale() const
{
	return m_builtinEffectData.getColorScale();
}

//------------------------------------------------------------------------------
void VisualComponent::setBlendColor(const Color& value)
{
	m_builtinEffectData.setBlendColor(value);
}

//------------------------------------------------------------------------------
const Color& VisualComponent::getBlendColor() const
{
	return m_builtinEffectData.getBlendColor();
}

//------------------------------------------------------------------------------
void VisualComponent::setTone(const ToneF& value)
{
	m_builtinEffectData.setTone(value);
}

//------------------------------------------------------------------------------
const ToneF& VisualComponent::getTone() const
{
	return m_builtinEffectData.getTone();
}

//------------------------------------------------------------------------------
void VisualComponent::setShader(Shader* value)
{
	m_builtinEffectData.setShader(value);
}

//------------------------------------------------------------------------------
Shader* VisualComponent::getShader() const
{
	return m_builtinEffectData.getShader();
}

////------------------------------------------------------------------------------
//void VisualComponent::setOpacity(float value, int subsetIndex)
//{
//	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
//	// TODO: サブマテリアルの設定
//	//m_materialList->GetMainMaterial()->setOpacity(opacity);
//	m_builtinEffectData.setOpacity(value);
//}
//float VisualComponent::getOpacity() const
//{
//	return m_builtinEffectData.getOpacity();
//}
//
//void VisualComponent::setColorScale(const Color& value, int subsetIndex)
//{
//	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
//	// TODO: サブマテリアルの設定
//	//m_materialList->GetMainMaterial()->setColorScale(color);
//	m_builtinEffectData.setColorScale(value);
//}
//void VisualComponent::setColorScale(float r, float g, float b, float a, int subsetIndex)
//{
//	setColorScale(Color(r, g, b, a));
//}
//void VisualComponent::setColor(const Color32& color)
//{
//	setColorScale(Color(color));
//}
//void VisualComponent::setColor(int r, int g, int b, int a)
//{
//	setColor(Color32(r, g, b, a));
//}
//const Color& VisualComponent::getColorScale() const
//{
//	return m_builtinEffectData.getColorScale();
//}
//void VisualComponent::setBlendColor(const Color& color, int subsetIndex)
//{
//	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
//	// TODO: サブマテリアルの設定
//	//m_materialList->GetMainMaterial()->setBlendColor(color);
//	m_builtinEffectData.setBlendColor(color);
//}
//void VisualComponent::setTone(const ToneF& tone, int subsetIndex)
//{
//	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
//	// TODO: サブマテリアルの設定
//	//m_materialList->GetMainMaterial()->setTone(tone);
//	m_builtinEffectData.setTone(tone);
//}
//void VisualComponent::setShader(Shader* value, int subsetIndex)
//{
//	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
//	// TODO: サブマテリアルの設定
//	//m_materialList->GetMainMaterial()->setShader(shader);
//	m_builtinEffectData.setShader(value);
//}

//------------------------------------------------------------------------------
void VisualComponent::setBlendMode(BlendMode mode) { m_blendMode = mode; }

//------------------------------------------------------------------------------
//BlendMode VisualComponent::getBlendMode() const { return GetMainMaterial()->getBlendMode(); }

//------------------------------------------------------------------------------
void VisualComponent::setCullingMode(CullingMode mode) { m_cullingMode = mode; }

//------------------------------------------------------------------------------
//CullingMode VisualComponent::getCullingMode() const { return GetMainMaterial()->getCullingMode(); }

//------------------------------------------------------------------------------
void VisualComponent::setDepthTestEnabled(bool enabled) { m_depthTestEnabled = enabled; }

//------------------------------------------------------------------------------
//bool VisualComponent::isDepthTestEnabled() const { return GetMainMaterial()->isDepthTestEnabled(); }

//------------------------------------------------------------------------------
void VisualComponent::setDepthWriteEnabled(bool enabled) { m_depthWriteEnabled = enabled; }

//------------------------------------------------------------------------------
//bool VisualComponent::isDepthWriteEnabled() const { return GetMainMaterial()->isDepthWriteEnabled(); }

//------------------------------------------------------------------------------
void VisualComponent::UpdateFrameHierarchy(SceneNode* parent, float deltaTime)
{
	// TODO: 描画関係のデータは updateFrame でやるべきではないような気もする。
	//m_materialList->UpdateMaterialInstances(m_ownerSceneGraph);

	SceneNode::UpdateFrameHierarchy(parent, deltaTime);
}

//------------------------------------------------------------------------------
detail::Sphere VisualComponent::GetBoundingSphere()
{
	const detail::Sphere s{ Vector3::Zero, -1 };
	return s;
}

////------------------------------------------------------------------------------
//Shader* VisualComponent::GetPrimaryShader() const
//{
//	// TODO: main が無ければ [0] のをつかう
//	return m_materialList->GetMainMaterial()->getShader();
//}

//------------------------------------------------------------------------------
void VisualComponent::render(DrawList* context)
{
	if (m_isVisible)
	{
		context->setBlendMode(m_blendMode);
		context->setCullingMode(m_cullingMode);
		context->setDepthTestEnabled(m_depthTestEnabled);
		context->setDepthWriteEnabled(m_depthWriteEnabled);
		context->SetBuiltinEffectData(m_builtinEffectData);
		onRender(context);
		OnRender2(context);
	}
}



//==============================================================================
// VisualObject
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(VisualObject, WorldObject);

//------------------------------------------------------------------------------
VisualObject::VisualObject()
{
}

//------------------------------------------------------------------------------
VisualObject::~VisualObject()
{
}

//------------------------------------------------------------------------------
void VisualObject::initialize()
{
	WorldObject::initialize();
}

//------------------------------------------------------------------------------
void VisualObject::setOpacity(float value)
{
	GetMainVisualComponent()->setOpacity(value);
}

//------------------------------------------------------------------------------
float VisualObject::getOpacity() const
{
	return GetMainVisualComponent()->getOpacity();
}

//------------------------------------------------------------------------------
void VisualObject::setColorScale(const Color& value)
{
	GetMainVisualComponent()->setColorScale(value);
}

//------------------------------------------------------------------------------
void VisualObject::setColorScale(float r, float g, float b, float a)
{
	setColorScale(Color(r, g, b, a));
}

//------------------------------------------------------------------------------
const Color& VisualObject::getColorScale() const
{
	return GetMainVisualComponent()->getColorScale();
}

//------------------------------------------------------------------------------
void VisualObject::setBlendColor(const Color& value)
{
	GetMainVisualComponent()->setBlendColor(value);
}

//------------------------------------------------------------------------------
const Color& VisualObject::getBlendColor() const
{
	return GetMainVisualComponent()->getBlendColor();
}

//------------------------------------------------------------------------------
void VisualObject::setTone(const ToneF& value)
{
	GetMainVisualComponent()->setTone(value);
}

//------------------------------------------------------------------------------
const ToneF& VisualObject::getTone() const
{
	return GetMainVisualComponent()->getTone();
}

//------------------------------------------------------------------------------
void VisualObject::setShader(Shader* value)
{
	GetMainVisualComponent()->setShader(value);
}

//------------------------------------------------------------------------------
Shader* VisualObject::getShader() const
{
	return GetMainVisualComponent()->getShader();
}

//------------------------------------------------------------------------------
void VisualObject::setBlendMode(BlendMode mode)
{
	GetMainVisualComponent()->setBlendMode(mode);
}

//------------------------------------------------------------------------------
BlendMode VisualObject::getBlendMode() const
{
	return GetMainVisualComponent()->getBlendMode();
}

//------------------------------------------------------------------------------
void VisualObject::setCullingMode(CullingMode mode)
{
	GetMainVisualComponent()->setCullingMode(mode);
}

//------------------------------------------------------------------------------
CullingMode VisualObject::getCullingMode() const
{
	return GetMainVisualComponent()->getCullingMode();
}

//------------------------------------------------------------------------------
void VisualObject::setDepthTestEnabled(bool enabled)
{
	GetMainVisualComponent()->setDepthTestEnabled(enabled);
}

//------------------------------------------------------------------------------
bool VisualObject::isDepthTestEnabled() const
{
	return GetMainVisualComponent()->isDepthTestEnabled();
}

//------------------------------------------------------------------------------
void VisualObject::setDepthWriteEnabled(bool enabled)
{
	GetMainVisualComponent()->setDepthWriteEnabled(enabled);
}

//------------------------------------------------------------------------------
bool VisualObject::isDepthWriteEnabled() const
{
	return GetMainVisualComponent()->isDepthWriteEnabled();
}

LN_NAMESPACE_SCENE_END
LN_NAMESPACE_END
