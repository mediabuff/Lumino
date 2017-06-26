﻿
#include "../Internal.h"
#include <Lumino/Graphics/GraphicsContext.h>
#include <Lumino/Rendering/RenderingContext.h>
#include "SceneGraphManager.h"
#include <Lumino/World.h>
#include <Lumino/Scene/SceneGraph.h>
#include <Lumino/Scene/TextBlock.h>
#include "../Documents/DocumentElements.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_SCENE_BEGIN

//==============================================================================
// TextBlock2DComponent
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(TextBlock2DComponent, VisualComponent);

//------------------------------------------------------------------------------
TextBlock2DComponentPtr TextBlock2DComponent::create()
{
	auto ptr = TextBlock2DComponentPtr::makeRef();
	ptr->initialize();
	return ptr;
}

//------------------------------------------------------------------------------
TextBlock2DComponentPtr TextBlock2DComponent::create(const StringRef& text)
{
	auto ptr = TextBlock2DComponentPtr::makeRef();
	ptr->initialize();
	ptr->setText(text);
	return ptr;
}

//------------------------------------------------------------------------------
TextBlock2DComponent::TextBlock2DComponent()
	: VisualComponent()
	, m_paragraph(nullptr)
{
}

//------------------------------------------------------------------------------
TextBlock2DComponent::~TextBlock2DComponent()
{
}

//------------------------------------------------------------------------------
void TextBlock2DComponent::initialize()
{
	VisualComponent::initialize();

	//owner->getRootNode()->addChild(this);
	setAutoRemove(true);

	m_paragraph = RefPtr<detail::Paragraph>::makeRef();
	m_paragraph->initialize();

	//setBlendMode(BlendMode::Alpha);
}

//------------------------------------------------------------------------------
void TextBlock2DComponent::setText(const StringRef& text)
{
	m_paragraph->clearInlines();
	auto run = RefPtr<detail::run>::makeRef();
	run->initialize();
	run->setText(text);
	m_paragraph->addInline(run);
}

//------------------------------------------------------------------------------
void TextBlock2DComponent::setAnchorPoint(const Vector2& ratio)
{
	m_anchor = ratio;
}

//------------------------------------------------------------------------------
void TextBlock2DComponent::setAnchorPoint(float ratioX, float ratioY)
{
	m_anchor.set(ratioX, ratioY);
}

//------------------------------------------------------------------------------
void TextBlock2DComponent::updateFrameHierarchy(SceneNode* parent, float deltaTime)
{
	VisualComponent::updateFrameHierarchy(parent, deltaTime);
	m_paragraph->updateLayout(Size::MaxValue);
	//m_paragraph->measureLayout(Size::MaxValue);
	//m_paragraph->arrangeLayout(RectF(0, 0, Size::MaxValue));
}

//------------------------------------------------------------------------------
detail::Sphere TextBlock2DComponent::getBoundingSphere()
{
	return VisualComponent::getBoundingSphere();
}

//------------------------------------------------------------------------------
void TextBlock2DComponent::onRender2(RenderingContext* renderer)
{
	struct LocalRenderer : detail::IDocumentsRenderer
	{
		DrawList* renderer;

		virtual void onDrawGlyphRun(const Matrix& transform, Brush* forground, GlyphRun* glyphRun, const PointF& point) override
		{
			// TODO: ここで強制設定よりは VisualComponent::setBlendMode がいいか？
			renderer->setBlendMode(BlendMode::Alpha);
			renderer->setTransform(transform);
			renderer->setBrush(forground);
			renderer->drawGlyphRun(point, glyphRun);
		}
	} r;
	r.renderer = renderer;


	const Size& size = m_paragraph->getRenderSize();
	m_paragraph->render(Matrix::makeTranslation(-size.width * m_anchor.x, -size.height * m_anchor.y, 0) * getOwnerObject()->transform.getWorldMatrix(), &r);
}

//------------------------------------------------------------------------------
//void TextBlock2DComponent::onRender(SceneGraphRenderingContext* dc)
//{
//	const Size& size = m_paragraph->getRenderSize();
//	m_paragraph->render(Matrix::MakeTranslation(-size.width * m_anchor.x, -size.height * m_anchor.y, 0) * m_combinedGlobalMatrix, dc);
//}


//==============================================================================
// TextBlock2D
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(TextBlock2D, VisualObject);

//------------------------------------------------------------------------------
RefPtr<TextBlock2D> TextBlock2D::create()
{
	return newObject<TextBlock2D>();
}

//------------------------------------------------------------------------------
RefPtr<TextBlock2D> TextBlock2D::create(const StringRef& text)
{
	return newObject<TextBlock2D>(text);
}

//------------------------------------------------------------------------------
TextBlock2D::TextBlock2D()
	: VisualObject()
{
}

//------------------------------------------------------------------------------
TextBlock2D::~TextBlock2D()
{
}

//------------------------------------------------------------------------------
void TextBlock2D::initialize()
{
	VisualObject::initialize();
	m_component = TextBlock2DComponent::create();
	addComponent(m_component);
	detail::EngineDomain::getDefaultWorld2D()->addWorldObject(this, true);
}

//------------------------------------------------------------------------------
void TextBlock2D::initialize(const StringRef& text)
{
	initialize();
	setText(text);
}

//------------------------------------------------------------------------------
void TextBlock2D::setText(const StringRef& text)
{
	m_component->setText(text);
}

//------------------------------------------------------------------------------
VisualComponent* TextBlock2D::getMainVisualComponent() const
{
	return m_component;
}

LN_NAMESPACE_SCENE_END
LN_NAMESPACE_END
