﻿
#include "Internal.h"
#include <Lumino/Physics/PhysicsWorld.h>
#include <Lumino/Rendering/RenderingContext.h>
#include <Lumino/Mesh/Mesh.h>
#include <Lumino/Scene/SceneGraph.h>
#include <Lumino/Scene/Camera.h>
#include <Lumino/Scene/Light.h>
#include <Lumino/Scene/Fog.h>
#include <Lumino/World.h>
#include <Lumino/Scene/WorldObject.h>
#include <Lumino/Scene/OffscreenWorldView.h>
#include "Graphics/GraphicsManager.h"
#include "EngineManager.h"

LN_NAMESPACE_BEGIN

//==============================================================================
// World
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(World, Object);

//------------------------------------------------------------------------------
World::World()
{
}

//------------------------------------------------------------------------------
World::~World()
{
}

//------------------------------------------------------------------------------
void World::initialize()
{
	m_renderer = newObject<RenderingContext>();
	//m_insideWorldRenderer = newObject<DrawList>(detail::EngineDomain::getGraphicsManager());
	m_debugRenderer = newObject<DrawList>(detail::EngineDomain::getGraphicsManager());

	m_debugRendererDefaultMaterial = newObject<CommonMaterial>();

	for (int i = 0; i < detail::MaxOffscreenId; i++)
	{
		m_offscreenIdStorage.add(i);
	}

	beginUpdateFrame();
}

WorldObject* World::add(WorldObject* obj)
{
	addWorldObject(obj, false);
	return obj;
}

void World::remove(WorldObject* obj)
{
	removeWorldObject(obj);
}

WorldObject* World::addEmptyObject()
{
	return add(newObject<WorldObject>());
}

void World::removeAllObjects()
{
	for (int i = m_rootWorldObjectList.getCount() - 1; i >= 0; i--)
	{
		if (!m_rootWorldObjectList[i]->isSpecialObject())
		{
			m_rootWorldObjectList[i]->m_parent = nullptr;
			m_rootWorldObjectList.removeAt(i);
		}
	}
}

//------------------------------------------------------------------------------
DrawList* World::getRenderer() const
{
	return m_renderer;
}

//------------------------------------------------------------------------------
DrawList* World::GetDebugRenderer() const
{
	return m_debugRenderer;
}

//------------------------------------------------------------------------------
void World::addWorldObject(WorldObject* obj, bool autoRelease)
{
	m_rootWorldObjectList.add(obj);
	obj->m_world = this;
	obj->m_isAutoRelease = autoRelease;
	obj->onAttachedWorld(this);
}

//------------------------------------------------------------------------------
void World::removeWorldObject(WorldObject* obj)
{
	if (LN_REQUIRE(obj != nullptr)) return;
	if (LN_REQUIRE(obj->m_world == this)) return;
	m_rootWorldObjectList.remove(obj);
	obj->m_parent = nullptr;
	obj->onDetachedWorld(this);
}

//------------------------------------------------------------------------------
void World::addOffscreenWorldView(OffscreenWorldSubRenderView* view)
{
	if (LN_REQUIRE(!m_offscreenIdStorage.isEmpty())) return;
	m_offscreenWorldViewList.add(view);

	// assign Id
	view->setId(m_offscreenIdStorage.getLast());
	m_offscreenIdStorage.removeLast();
}

//------------------------------------------------------------------------------
void World::removeOffscreenWorldView(OffscreenWorldSubRenderView* view)
{
	m_offscreenWorldViewList.remove(view);

	// repay Id.
	m_offscreenIdStorage.add(view->getId());
	view->setId(0);
}

//------------------------------------------------------------------------------
void World::beginUpdateFrame()
{
	// onRender の外のデバッグ描画などでも使用するため、描画リストのクリアは SceneGraph の更新前でなければならない。
	// また、出来上がった描画リストを、複数のビューやカメラが描画することを想定する。
	if (m_renderer != nullptr)
	{
		m_renderer->beginMakeElements();
	}
	if (m_debugRenderer != nullptr)
	{
		m_debugRenderer->beginMakeElements();
		m_debugRendererDefaultMaterial->setShader(detail::EngineDomain::getGraphicsManager()->getBuiltinShader(BuiltinShader::Sprite));
		//TODO:
		//m_debugRenderer->setDefaultMaterial(m_debugRendererDefaultMaterial);
	}
}

void World::updateFrame(float deltaSceonds)
{
	onPreUpdate(deltaSceonds);
	onInternalPhysicsUpdate(deltaSceonds);
	onUpdate(deltaSceonds);
	onInternalAnimationUpdate(deltaSceonds);
	onPostUpdate(deltaSceonds);
}

void World::onPreUpdate(float deltaSceonds)
{
	for (auto& obj : m_rootWorldObjectList)
	{
		obj->onPreUpdate();
	}
}

void World::onInternalPhysicsUpdate(float deltaSceonds)
{
}

void World::onUpdate(float deltaSceonds)
{
	for (auto& obj : m_rootWorldObjectList)
	{
		obj->updateFrame(deltaSceonds);
	}
}

void World::onInternalAnimationUpdate(float deltaSceonds)
{
}

void World::onPostUpdate(float deltaSceonds)
{
}

//------------------------------------------------------------------------------
void World::renderRoot(WorldRenderViewBase* renderView, WorldDebugDrawFlags debugDrawFlags)
{
	// pre render
	for (auto& view : m_offscreenWorldViewList)
	{
		view->renderWorld(this, renderView);
	}

	// main render
	render(m_renderer, renderView, debugDrawFlags, renderView->getLayerCullingMask());
}

//------------------------------------------------------------------------------
void World::render(RenderingContext* context, WorldRenderViewBase* renderView, WorldDebugDrawFlags debugDrawFlags, uint32_t layerMask, OffscreenWorldSubRenderView* offscreen)
{
	LN_ASSERT(context->getRenderView() == nullptr);	// render 下はネスト禁止
	context->setRenderView(renderView);
	
	for (auto& obj : m_rootWorldObjectList)
	{
		obj->render(context);

		for (auto& c : obj->m_components)
		{
			c->onPreRender(context);

			VisualComponent* visual = nullptr;
			if (c->getSpecialComponentType() == SpecialComponentType::Visual)
			{
				visual = static_cast<VisualComponent*>(c.get());

				bool visible = LayerMask::filterComponent(c.get(), layerMask);
				if (visible && offscreen != nullptr) visible = offscreen->filterRenderObject(visual);

				if (visible)
				{
					c->render(context);
				}
			}
		}
	}

	// reset status
	//context->setBuiltinEffectData(detail::BuiltinEffectData::DefaultData);
	context->setVisualNodeParameters(nullptr);
	context->setRenderView(nullptr);
}

//------------------------------------------------------------------------------
void World::executeDrawListRendering(RenderTargetTexture* renderTarget, DepthBuffer* depthBuffer)
{
}

//------------------------------------------------------------------------------
void World::onUIEvent(UIEventArgs* e)
{
	for (auto& obj : m_rootWorldObjectList)
	{
		obj->onUIEvent(e);
		if (e->handled) return;
	}

	m_onEvent.raise(e);
}

//------------------------------------------------------------------------------
EventConnection World::connectOnUIEvent(UIEventHandler handler)
{
	return m_onEvent.connect(handler);
}

void World::updateObjectsWorldMatrix()
{
	for (auto& obj : m_rootWorldObjectList)
	{
		obj->updateWorldMatrixHierarchical();
	}
}

//==============================================================================
// World2D
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(World2D, World);

//------------------------------------------------------------------------------
World2D::World2D()
{
}

//------------------------------------------------------------------------------
World2D::~World2D()
{
}

//------------------------------------------------------------------------------
void World2D::initialize()
{
	World::initialize();

	m_mainCamera = newObject<Camera>(CameraProjection_2D, true);
	m_mainCamera->setSpecialObject(true);
	add(m_mainCamera);
}

//------------------------------------------------------------------------------
Camera* World2D::getMainCamera() const
{
	return m_mainCamera;
}

//------------------------------------------------------------------------------
//detail::SceneGraphRenderingProfilerInterface& World2D::GetRenderingProfiler()
//{
//	return m_sceneGraph->GetRenderingProfiler();
//}

//------------------------------------------------------------------------------
void World2D::beginUpdateFrame()
{
	World::beginUpdateFrame();

	//if (m_sceneGraph != nullptr) {
	//	m_sceneGraph->beginUpdateFrame();
	//}
}

//------------------------------------------------------------------------------
void World2D::onUpdate(float deltaSceonds)
{
	World::onUpdate(deltaSceonds);
}

//------------------------------------------------------------------------------
void World2D::render(RenderingContext* context, WorldRenderViewBase* renderView, WorldDebugDrawFlags debugDrawFlags, uint32_t layerMask, OffscreenWorldSubRenderView* offscreen)
{
	World::render(context, renderView, debugDrawFlags, layerMask, offscreen);
}

//==============================================================================
// World3D
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(World3D, World);

//------------------------------------------------------------------------------
World3D::World3D()
	: m_visibleGridPlane(false)
{
}

//------------------------------------------------------------------------------
World3D::~World3D()
{
}

//------------------------------------------------------------------------------
void World3D::initialize()
{
	World::initialize();

	m_physicsWorld = newObject<PhysicsWorld>();

	m_mainCamera = newObject<Camera>(CameraProjection_3D, true);
	m_mainCamera->setSpecialObject(true);
	add(m_mainCamera);

	//m_mainLight = newObject<Light>();
	//m_mainLight->setSpecialObject(true);
	//addWorldObject(m_mainLight, true);

	setFogColor(Color(1, 1, 1, 1));
	setFogDensity(0.0);

	createGridPlane();
}

//------------------------------------------------------------------------------
PhysicsWorld* World3D::getPhysicsWorld3D() const
{
	return m_physicsWorld;
}

//------------------------------------------------------------------------------
Camera* World3D::getMainCamera() const
{
	return m_mainCamera;
}

//------------------------------------------------------------------------------
//detail::SceneGraphRenderingProfilerInterface& World3D::GetRenderingProfiler()
//{
//	return m_sceneGraph->GetRenderingProfiler();
//}

Fog* World3D::getFog() const
{
	return m_fog;
}

void World3D::setFog(Fog* fog)
{
	m_fog = fog;
}

//------------------------------------------------------------------------------
void World3D::beginUpdateFrame()
{
	World::beginUpdateFrame();

	//if (m_sceneGraph != nullptr)
	//{
	//	m_sceneGraph->beginUpdateFrame();
	//}
}

//------------------------------------------------------------------------------
void World3D::onInternalPhysicsUpdate(float deltaSceonds)
{
	// Physics モジュールの Component が、WorldObject の WorldMatrix を元にシミュレーション前準備を行うことがあるので
	// ここで WorldMatrix を更新しておく。
	updateObjectsWorldMatrix();

	if (m_physicsWorld != nullptr)
	{
		m_physicsWorld->stepSimulation(deltaSceonds);
	}
}

//------------------------------------------------------------------------------
void World3D::render(RenderingContext* context, WorldRenderViewBase* renderView, WorldDebugDrawFlags debugDrawFlags, uint32_t layerMask, OffscreenWorldSubRenderView* offscreen)
{

	World::render(context, renderView, debugDrawFlags, layerMask, offscreen);

	renderGridPlane(context, renderView);

	if (debugDrawFlags.TestFlag(WorldDebugDrawFlags::PhysicsInfo))
	{
		if (m_physicsWorld != nullptr)
		{
			class DebugRenderer : public PhysicsWorld::IDebugRenderer
			{
			public:
				DrawList* renderer;

				virtual void drawLine(const Vector3& from, const Vector3& to, const Vector3& fromColor, const Vector3& toColor)
				{
					renderer->drawLinePrimitive(from, Color(fromColor, 1.0f), to, Color(toColor, 1.0f));
				}
			};
			DebugRenderer r;
			r.renderer = GetDebugRenderer();

			m_physicsWorld->drawDebugShapes(&r);
		}
	}
}


//------------------------------------------------------------------------------
void World3D::createGridPlane()
{
	detail::GraphicsManager* gm = detail::EngineDomain::getGraphicsManager();

	// 適当な四角形メッシュ
	m_gridPlane = Ref<StaticMeshModel>::makeRef();
	m_gridPlane->initializeScreenPlane(MeshCreationFlags::DynamicBuffers);
	MeshResource* mesh = m_gridPlane->getMeshResource(0);
	mesh->addSections(1);
	mesh->getSection(0)->MaterialIndex = 0;
	mesh->getSection(0)->StartIndex = 0;
	mesh->getSection(0)->PrimitiveNum = 2;
	mesh->getSection(0)->primitiveType = PrimitiveType_TriangleList;

	static const byte_t shaderCode[] =
	{
#include "Scene/Resource/InfinitePlaneGrid.lfx.h"
	};
	static const size_t shaderCodeLen = LN_ARRAY_SIZE_OF(shaderCode);
	auto shader = Ref<Shader>::makeRef();
	shader->initialize(gm, shaderCode, shaderCodeLen);
	m_gridPlane->getMaterial(0)->setShader(shader);

	// 四方の辺に黒線を引いたテクスチャを作り、マテリアルにセットしておく
	SizeI gridTexSize(512, 512);
	auto gridTex = newObject<Texture2D>(gridTexSize, TextureFormat::R8G8B8A8, true, ResourceUsage::Static);
	for (int x = 0; x < gridTexSize.width; ++x)
	{
		gridTex->setPixel(x, 0, Color(0, 0, 0, 0.5));
		gridTex->setPixel(x, gridTexSize.width - 1, Color(0, 0, 0, 0.5));
	}
	for (int y = 0; y < gridTexSize.height; ++y)
	{
		gridTex->setPixel(0, y, Color(0, 0, 0, 0.5));
		gridTex->setPixel(gridTexSize.height - 1, y, Color(0, 0, 0, 0.5));
	}
	m_gridPlane->getMaterial(0)->setMaterialTexture(gridTex);

	m_gridPlane->getMaterial(0)->setBlendMode(BlendMode::Alpha);
	m_gridPlane->getMaterial(0)->setDepthWriteEnabled(false);
}

//------------------------------------------------------------------------------
void World3D::renderGridPlane(DrawList* renderer, RenderView* renderView)
{
	if (m_visibleGridPlane)
	{
		adjustGridPlane(renderView);
		renderer->setTransform(Matrix::Identity);

		DrawElementMetadata metadata;
		metadata.priority = (int)DepthPriority::Foreground;
		renderer->pushMetadata(&metadata);
		renderer->drawMesh(m_gridPlane->getMeshResource(0), 0, m_gridPlane->getMaterial(0));
		renderer->popMetadata();
	}
}

//------------------------------------------------------------------------------
void World3D::adjustGridPlane(RenderView* renderView)
{
	/*
	カメラの視錐台と、グリッドを描画したい平面との衝突点から、四角形メッシュを作る。
	これで視界に映る平面全体をカバーするメッシュができる。
	あとはシェーダで、頂点の位置を利用してグリッドテクスチャをサンプリングする。
	*/

	struct Line
	{
		Vector3	from;
		Vector3	to;
	};

	auto& vf = renderView->m_cameraInfo.viewFrustum;
	Vector3 points[8];
	vf.getCornerPoints(points);

	Line lines[12] =
	{
		// near
		{ points[0], points[1] },
		{ points[1], points[2] },
		{ points[2], points[3] },
		{ points[3], points[0] },
		// near - far
		{ points[0], points[4] },
		{ points[1], points[5] },
		{ points[2], points[6] },
		{ points[3], points[7] },
		// far
		{ points[4], points[5] },
		{ points[5], points[6] },
		{ points[6], points[7] },
		{ points[7], points[4] },
	};

	Plane plane(0, 1, 0, 0);
	List<Vector3> hits;
	for (Line& li : lines)
	{
		Vector3 pt;
		if (plane.intersects(li.from, li.to, &pt))
		{
			hits.add(pt);
		}
	}

	Vector3 minPos, maxPos;
	for (Vector3& p : hits)
	{
		minPos = Vector3::min(p, minPos);
		maxPos = Vector3::max(p, maxPos);
	}

	MeshResource* mesh = m_gridPlane->getMeshResource(0);
	mesh->setPosition(0, Vector3(minPos.x, 0, maxPos.z));
	mesh->setPosition(1, Vector3(minPos.x, 0, minPos.z));
	mesh->setPosition(2, Vector3(maxPos.x, 0, maxPos.z));
	mesh->setPosition(3, Vector3(maxPos.x, 0, minPos.z));
	mesh->setUV(0, Vector2(-1.0f, 1.0f));
	mesh->setUV(1, Vector2(-1.0f, -1.0f));
	mesh->setUV(2, Vector2(1.0f, 1.0f));
	mesh->setUV(3, Vector2(1.0f, -1.0f));
	mesh->m_attributes[0].PrimitiveNum = 2;
}

LN_NAMESPACE_END
