﻿
#pragma once
#include <array>
#include "SceneGraph.h"
#include "../../src/Scene/MME/MMETypes.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_SCENE_BEGIN

/**
	@brief
*/
class MMDSceneGraph
	: public SceneGraph
{
public:
	//SceneNode* GetRootNode() { return m_defaultRoot; }
	//SceneNode* Get2DRootNode() { return m_default2DRoot; }
	//SceneNode* Get3DRootNode() { return m_default3DRoot; }
	Camera* GetDefault3DCamera() { return m_default3DCamera; }
	virtual void UpdateFrame(float elapsedTime) override;
	virtual SceneNode* GetRootNode() override { return m_defaultRoot; }
	virtual Camera* GetMainCamera() override { return m_default3DCamera; }
	virtual List<RenderingPass*>* GetRenderingPasses() override { return &m_mmdRenderingPasses; }

	//virtual bool InjectMouseWheel(int delta) override;
	virtual detail::MaterialInstance* CreateMaterialInstance() override;

LN_INTERNAL_ACCESS:
	MMDSceneGraph();
	virtual ~MMDSceneGraph();
	void CreateCore(SceneGraphManager* manager);

private:
	SceneNode*		m_defaultRoot;
	//SceneNode*		m_default3DRoot;
	//SceneNode*		m_default2DRoot;

	Camera*			m_default3DCamera;
	Light*			m_defaultLight;


	//Camera*			m_default2DCamera;
	EffectBatchRendererNode*	m_effectBatchRendererNode;



	List<RenderingPass*>	m_mmdRenderingPasses;

};

LN_NAMESPACE_SCENE_END
LN_NAMESPACE_END
