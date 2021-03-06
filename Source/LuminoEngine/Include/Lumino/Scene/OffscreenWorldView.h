﻿
#pragma once
#include "VisualNode.h"

LN_NAMESPACE_BEGIN
//class CameraComponent;
class VisualComponent;

/**
	@brief		
*/
class OffscreenWorldSubRenderView
	: public WorldRenderViewBase
{
public:
	RenderTargetTexture* getRenderTarget() const;

	void hideVisual(VisualComponent* renderObject);

protected:
	virtual void filterWorldMatrix(Matrix* outMatrix) override;
	virtual Matrix calculateViewMatrix(RenderView* mainRenderView);
	virtual Matrix calculateProjectionMatrix(RenderView* mainRenderView);

LN_CONSTRUCT_ACCESS:
	OffscreenWorldSubRenderView();
	virtual ~OffscreenWorldSubRenderView();
	void initialize();

LN_INTERNAL_ACCESS:
	void setId(int id) { m_id = id; }
	int getId() const { return m_id; }
	void renderWorld(World* world, RenderView* mainRenderView);
	bool filterRenderObject(VisualComponent* renderObject);
	detail::OffscreenFilterInfo* updateRenderObjectFilterInfo(VisualComponent* renderObject);

private:
	Ref<RenderTargetTexture>	m_renderTarget;
	Ref<RenderingContext>	m_renderer;
	int							m_id;
};



class SkyComponent
	: public VisualComponent
{
public:

protected:
	virtual void onRender2(RenderingContext* renderer) override;

LN_INTERNAL_ACCESS:
	SkyComponent();
	virtual ~SkyComponent();
	void initialize();

private:

	Ref<CommonMaterial>			m_skyMaterial;
};

class MirrorComponent
	: public VisualComponent
{
public:

protected:
	virtual void onRender2(RenderingContext* renderer) override;

LN_INTERNAL_ACCESS:
	MirrorComponent();
	virtual ~MirrorComponent();
	void initialize();

private:
	Ref<OffscreenWorldSubRenderView>	m_offscreen;
	Ref<CommonMaterial>			m_material;

};

LN_NAMESPACE_END
