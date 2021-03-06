﻿
#include "../Internal.h"
#include <Lumino/Graphics/GraphicsContext.h>	// TODO: いらない
#include <Lumino/Rendering/RenderingContext.h>
#include "SceneGraphManager.h"
#include <Lumino/Scene/SceneGraph.h>
#include <Lumino/Scene/Sprite.h>
#include <Lumino/World.h>
#include <Lumino/Assets.h>

LN_NAMESPACE_BEGIN
LN_NAMESPACE_SCENE_BEGIN

//==============================================================================
// SpriteComponent
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(SpriteComponent, VisualComponent);

//------------------------------------------------------------------------------
SpriteComponent::SpriteComponent()
	: VisualComponent()
	, m_size()
	, m_srcRect()
	, m_flipMode(FlipMode_None)
	, m_texture(nullptr)
{
}

//------------------------------------------------------------------------------
SpriteComponent::~SpriteComponent()
{
}

//------------------------------------------------------------------------------
void SpriteComponent::initialize()
{
	VisualComponent::initialize();
	m_srcRect.set(0, 0, -1, -1);
	setSize(Size(-1, -1));

	setBlendMode(BlendMode::Alpha);

	m_material = newObject<CommonMaterial>();

	//owner->getRootNode()->addChild(this);
	setAutoRemove(true);
}

//------------------------------------------------------------------------------
void SpriteComponent::setTexture(Texture* texture)
{
	//if (LN_REQUIRE(m_materialList != nullptr)) return;
	//m_materialList->GetAt(0)->setMaterialTexture(texture);
	m_texture = texture;
	updateVertexData();
}

//------------------------------------------------------------------------------
void SpriteComponent::setSize(const Size& size)
{
	m_size = size;
}

//------------------------------------------------------------------------------
Texture* SpriteComponent::getTexture() const
{
	//if (LN_REQUIRE(m_materialList != nullptr)) return nullptr;
	//return m_materialList->GetAt(0)->getMaterialTexture(nullptr);
	return m_texture;
}

//------------------------------------------------------------------------------
void SpriteComponent::setSourceRect(const Rect& rect)
{
	m_srcRect = rect;
	updateVertexData();
}

//------------------------------------------------------------------------------
void SpriteComponent::setSourceRect(float x, float y, float width, float height)
{
	setSourceRect(Rect(x, y, width, height));
}

//------------------------------------------------------------------------------
void SpriteComponent::setAnchorPoint(const Vector2& ratio)
{
	m_anchor = ratio;
}

//------------------------------------------------------------------------------
void SpriteComponent::setAnchorPoint(float ratioX, float ratioY)
{
	m_anchor.set(ratioX, ratioY);
}

//------------------------------------------------------------------------------
void SpriteComponent::renderSprite(RenderingContext* renderer, SpriteBaseDirection dir)
{
	//CommonMaterial* mat = GetMainMaterial();
	Color colorScale = getColorScale();
	colorScale.a *= getOpacity();
	renderer->drawSprite(
		Vector3::Zero,
		m_renderSize,
		m_anchor,
		getTexture(),
		m_renderSourceRect,
		colorScale,
		dir,
		getBillboardType(),
		m_material);
}

//------------------------------------------------------------------------------
void SpriteComponent::updateVertexData()
{
	// 転送元矩形が負値ならテクスチャ全体を転送する
	Texture* tex = getTexture();
	const SizeI& texSize = (tex != nullptr) ? tex->getSize() : SizeI::Zero;
	m_renderSourceRect = m_srcRect;
	if (m_renderSourceRect.width < 0 && m_renderSourceRect.height < 0)
	{
		m_renderSourceRect.width = texSize.width;
		m_renderSourceRect.height = texSize.height;
	}
	m_renderSize = m_size;
	if (m_renderSize.width < 0 && m_renderSize.height < 0)
	{
		m_renderSize.width = m_renderSourceRect.width;
		m_renderSize.height = m_renderSourceRect.height;
	}
}


//==============================================================================
// Sprite2DComponent
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(Sprite2DComponent, SpriteComponent);

//------------------------------------------------------------------------------
Sprite2DComponentPtr Sprite2DComponent::create()
{
	auto obj = Sprite2DComponentPtr::makeRef();
	obj->initialize();
	return obj;
}

//------------------------------------------------------------------------------
Sprite2DComponentPtr Sprite2DComponent::create(const StringRef& filePath)
{
	auto tex = ln::Assets::loadTexture(filePath);
	return create(tex);
}

//------------------------------------------------------------------------------
Sprite2DComponentPtr Sprite2DComponent::create(Texture* texture)
{
	auto obj = create();
	obj->setTexture(texture);
	return obj;
}

//------------------------------------------------------------------------------
Sprite2DComponent::Sprite2DComponent()
{
}

//------------------------------------------------------------------------------
Sprite2DComponent::~Sprite2DComponent()
{
}

//------------------------------------------------------------------------------
void Sprite2DComponent::initialize()
{
	SpriteComponent::initialize();
}

//------------------------------------------------------------------------------
void Sprite2DComponent::initialize(Texture* texture)
{
	initialize();
	setTexture(texture);
}

//------------------------------------------------------------------------------
void Sprite2DComponent::onRender2(RenderingContext* renderer)
{
	renderSprite(renderer, SpriteBaseDirection::Basic2D);
}


//==============================================================================
// Sprite3DComponent
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(Sprite3DComponent, SpriteComponent);

//------------------------------------------------------------------------------
Sprite3DComponentPtr Sprite3DComponent::create()
{
	auto obj = Sprite3DComponentPtr::makeRef();
	obj->initialize();
	return obj;
}

//------------------------------------------------------------------------------
Sprite3DComponentPtr Sprite3DComponent::create(float width, float height)
{
	auto obj = Sprite3DComponentPtr::makeRef();
	obj->initialize();
	obj->setSize(Size(width, height));
	return obj;
}

//------------------------------------------------------------------------------
Sprite3DComponentPtr Sprite3DComponent::create(float width, float height, Texture* texture)
{
	auto obj = Sprite3DComponentPtr::makeRef();
	obj->initialize();
	obj->setSize(Size(width, height));
	obj->setTexture(texture);
	return obj;
}

//------------------------------------------------------------------------------
Sprite3DComponent::Sprite3DComponent()
{
}

//------------------------------------------------------------------------------
Sprite3DComponent::~Sprite3DComponent()
{
}

//------------------------------------------------------------------------------
void Sprite3DComponent::initialize()
{
	SpriteComponent::initialize();
}

//------------------------------------------------------------------------------
void Sprite3DComponent::onRender2(RenderingContext* renderer)
{
	renderSprite(renderer, SpriteBaseDirection::ZMinus);
}


//==============================================================================
// Sprite2D
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(SpriteBase, VisualObject);

//------------------------------------------------------------------------------
SpriteBase::SpriteBase()
	: VisualObject()
{
}

//------------------------------------------------------------------------------
SpriteBase::~SpriteBase()
{
}

//------------------------------------------------------------------------------
void SpriteBase::initialize()
{
	VisualObject::initialize();
}

//------------------------------------------------------------------------------
void SpriteBase::setTexture(Texture* texture)
{
	getSpriteComponent()->setTexture(texture);
}

//------------------------------------------------------------------------------
Texture* SpriteBase::getTexture() const
{
	return getSpriteComponent()->getTexture();
}

//------------------------------------------------------------------------------
void SpriteBase::setSourceRect(const Rect& rect)
{
	getSpriteComponent()->setSourceRect(rect);
}

//------------------------------------------------------------------------------
void SpriteBase::setSourceRect(float x, float y, float width, float height)
{
	getSpriteComponent()->setSourceRect(x, y, width, height);
}

//------------------------------------------------------------------------------
const Rect& SpriteBase::getSourceRect() const
{
	return getSpriteComponent()->getSourceRect();
}

//------------------------------------------------------------------------------
void SpriteBase::setAnchorPoint(const Vector2& ratio)
{
	getSpriteComponent()->setAnchorPoint(ratio);
}

//------------------------------------------------------------------------------
void SpriteBase::setAnchorPoint(float ratioX, float ratioY)
{
	getSpriteComponent()->setAnchorPoint(ratioX, ratioY);
}

//==============================================================================
// Sprite2D
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(Sprite2D, SpriteBase);

//------------------------------------------------------------------------------
Ref<Sprite2D> Sprite2D::create()
{
	return newObject<Sprite2D>();
}

//------------------------------------------------------------------------------
Ref<Sprite2D> Sprite2D::create(const StringRef& filePath)
{
	return newObject<Sprite2D>(filePath);
}

//------------------------------------------------------------------------------
Ref<Sprite2D> Sprite2D::create(Texture* texture)
{
	return newObject<Sprite2D>(texture);
}

//------------------------------------------------------------------------------
Sprite2D::Sprite2D()
	: SpriteBase()
	, m_component(nullptr)
{
}

//------------------------------------------------------------------------------
Sprite2D::~Sprite2D()
{
}

//------------------------------------------------------------------------------
void Sprite2D::initialize()
{
	SpriteBase::initialize();
	m_component = Sprite2DComponent::create();
	m_component->setLayer(LayerMask::GetLayer(BuiltinLayers::Default2D));
	addComponent(m_component);
}

//------------------------------------------------------------------------------
void Sprite2D::initialize(const StringRef& filePath)
{
	SpriteBase::initialize();
	m_component = Sprite2DComponent::create(filePath);
	addComponent(m_component);
}

//------------------------------------------------------------------------------
void Sprite2D::initialize(Texture* texture)
{
	SpriteBase::initialize();
	m_component = Sprite2DComponent::create(texture);
	addComponent(m_component);
}

//------------------------------------------------------------------------------
VisualComponent* Sprite2D::getMainVisualComponent() const
{
	return m_component;
}

//------------------------------------------------------------------------------
SpriteComponent* Sprite2D::getSpriteComponent() const
{
	return m_component;
}


//==============================================================================
// Sprite3D
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(Sprite3D, SpriteBase);

//------------------------------------------------------------------------------
Ref<Sprite3D> Sprite3D::create()
{
	return newObject<Sprite3D>();
}

//------------------------------------------------------------------------------
Ref<Sprite3D> Sprite3D::create(float width, float height, Texture* texture)
{
	return newObject<Sprite3D>(width, height, texture);
}

//------------------------------------------------------------------------------
Sprite3D::Sprite3D()
	: SpriteBase()
	, m_component(nullptr)
{
}

//------------------------------------------------------------------------------
Sprite3D::~Sprite3D()
{
}

//------------------------------------------------------------------------------
void Sprite3D::initialize()
{
	SpriteBase::initialize();
	m_component = Sprite3DComponent::create();
	m_component->setLayer(LayerMask::GetLayer(BuiltinLayers::Default3D));
	addComponent(m_component);
}

//------------------------------------------------------------------------------
void Sprite3D::initialize(float width, float height, Texture* texture)
{
	SpriteBase::initialize();
	m_component = Sprite3DComponent::create(width, height, texture);
	addComponent(m_component);
}

//------------------------------------------------------------------------------
VisualComponent* Sprite3D::getMainVisualComponent() const
{
	return m_component;
}

//------------------------------------------------------------------------------
SpriteComponent* Sprite3D::getSpriteComponent() const
{
	return m_component;
}


LN_NAMESPACE_SCENE_END
LN_NAMESPACE_END
