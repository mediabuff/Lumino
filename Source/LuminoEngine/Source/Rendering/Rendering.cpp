﻿
#include "../Internal.h"
#include <float.h>
#include <Lumino/Graphics/Color.h>
#include <Lumino/Rendering/Rendering.h>
#include <Lumino/Graphics/ContextInterface.h>
#include <Lumino/Mesh/Mesh.h>
#include "../Graphics/Device/GraphicsDriverInterface.h"
#include "../Graphics/GraphicsManager.h"
#include "../Graphics/RendererImpl.h"
#include "PrimitiveRenderer.h"
#include "MeshRendererProxy.h"
#include "SpriteRenderer.h"
#include "../Graphics/Text/TextRenderer.h"
#include "../Graphics/Text/FontManager.h"
#include "../Mesh/MeshFactory.h"
#include "ShapesRenderer.h"
#include "NanoVGRenderer.h"
#include "FrameRectRenderFeature.h"

LN_NAMESPACE_BEGIN

const DrawElementMetadata DrawElementMetadata::Default;

namespace detail {

//==============================================================================
// DynamicLightInfo
//==============================================================================

//------------------------------------------------------------------------------
DynamicLightInfo::DynamicLightInfo()
{
}

//==============================================================================
// CommandDataCache
//==============================================================================

//------------------------------------------------------------------------------
CommandDataCache::CommandDataCache()
	: m_dataList()
	, m_dataBuffer()
	, m_dataBufferUsed(0)
{
}

//------------------------------------------------------------------------------
CommandDataCache::~CommandDataCache()
{
}

//------------------------------------------------------------------------------
void CommandDataCache::Reserve(size_t dataCount, size_t byteCount)
{
	Clear();
	m_dataList.Reserve(dataCount);
	m_dataBuffer.Resize(byteCount, false);
}

//------------------------------------------------------------------------------
void CommandDataCache::Clear()
{
	m_dataList.Clear();
	m_dataBufferUsed = 0;
}

//------------------------------------------------------------------------------
CommandDataCache::DataHandle CommandDataCache::AllocData(size_t byteCount, const void* data)
{
	// バッファが足りなければ拡張する
	if (m_dataBufferUsed + byteCount > m_dataBuffer.GetSize())
	{
		size_t newSize = m_dataBuffer.GetSize() + std::max(m_dataBuffer.GetSize(), byteCount);	// 最低でも byteCount 分を拡張する
		m_dataBuffer.Resize(newSize, false);
	}

	if (data != nullptr)
	{
		byte_t* ptr = &(m_dataBuffer.GetData()[m_dataBufferUsed]);
		size_t size = m_dataBuffer.GetSize() - m_dataBufferUsed;
		memcpy_s(ptr, size, data, byteCount);
	}

	size_t dataIdx = m_dataBufferUsed;
	m_dataList.Add(dataIdx);
	m_dataBufferUsed += byteCount;
	return dataIdx;
}

//------------------------------------------------------------------------------
byte_t* CommandDataCache::GetData(DataHandle handle)
{
	return &(m_dataBuffer.GetData()[handle]);
}


//==============================================================================
// InternalContext
//==============================================================================

//------------------------------------------------------------------------------
InternalContext::InternalContext()
	: m_baseRenderer(nullptr)
	, m_primitiveRenderer(nullptr)
	, m_meshRenderer(nullptr)
	, m_spriteRenderer(nullptr)
	, m_nanoVGRenderer(nullptr)
	, m_current(nullptr)
	, m_currentStatePtr(nullptr)
{
}

//------------------------------------------------------------------------------
void InternalContext::Initialize(detail::GraphicsManager* manager)
{
	m_baseRenderer = manager->GetRenderer();

	m_primitiveRenderer = RefPtr<PrimitiveRenderer>::MakeRef();
	m_primitiveRenderer->Initialize(manager);

	m_blitRenderer = RefPtr<BlitRenderer>::MakeRef();
	m_blitRenderer->Initialize(manager);

	m_meshRenderer = RefPtr<MeshRendererProxy>::MakeRef();
	m_meshRenderer->Initialize(manager);

	m_spriteRenderer = RefPtr<SpriteRenderer>::MakeRef(manager, 2048);	// TODO

	m_textRenderer = RefPtr<TextRenderer>::MakeRef();
	m_textRenderer->Initialize(manager);

	m_vectorTextRenderer = RefPtr<VectorTextRenderer>::MakeRef();
	m_vectorTextRenderer->Initialize(manager);

	m_shapesRenderer = RefPtr<ShapesRenderer>::MakeRef();
	m_shapesRenderer->Initialize(manager);

	m_nanoVGRenderer = RefPtr<NanoVGRenderer>::MakeRef();
	m_nanoVGRenderer->Initialize(manager);

	m_frameRectRenderer = RefPtr<FrameRectRenderFeature>::MakeRef();
	m_frameRectRenderer->Initialize(manager);
}

//------------------------------------------------------------------------------
Details::Renderer* InternalContext::GetRenderStateManager()
{
	return m_baseRenderer;
}

//------------------------------------------------------------------------------
Details::Renderer* InternalContext::BeginBaseRenderer()
{
	SwitchActiveRenderer(m_baseRenderer);
	return m_baseRenderer;
}

//------------------------------------------------------------------------------
PrimitiveRenderer* InternalContext::BeginPrimitiveRenderer()
{
	SwitchActiveRenderer(m_primitiveRenderer);
	return m_primitiveRenderer;
}

//------------------------------------------------------------------------------
BlitRenderer* InternalContext::BeginBlitRenderer()
{
	SwitchActiveRenderer(m_blitRenderer);
	return m_blitRenderer;
}

//------------------------------------------------------------------------------
MeshRendererProxy* InternalContext::BeginMeshRenderer()
{
	SwitchActiveRenderer(m_meshRenderer);
	return m_meshRenderer;
}

//------------------------------------------------------------------------------
SpriteRenderer* InternalContext::BeginSpriteRenderer()
{
	SwitchActiveRenderer(m_spriteRenderer);
	return m_spriteRenderer;
}

//------------------------------------------------------------------------------
TextRenderer* InternalContext::BeginTextRenderer()
{
	SwitchActiveRenderer(m_textRenderer);
	return m_textRenderer;
}

//------------------------------------------------------------------------------
VectorTextRenderer* InternalContext::BeginVectorTextRenderer()
{
	SwitchActiveRenderer(m_vectorTextRenderer);
	return m_vectorTextRenderer;
}

//------------------------------------------------------------------------------
ShapesRenderer* InternalContext::BeginShapesRenderer()
{
	SwitchActiveRenderer(m_shapesRenderer);
	return m_shapesRenderer;
}

//------------------------------------------------------------------------------
NanoVGRenderer* InternalContext::BeginNanoVGRenderer()
{
	SwitchActiveRenderer(m_nanoVGRenderer);
	return m_nanoVGRenderer;
}

//------------------------------------------------------------------------------
FrameRectRenderFeature* InternalContext::BeginFrameRectRenderer()
{
	SwitchActiveRenderer(m_frameRectRenderer);
	return m_frameRectRenderer;
}

//------------------------------------------------------------------------------
void InternalContext::SetViewInfo(const Size& viewPixelSize, const Matrix& viewMatrix, const Matrix& projMatrix)
{
	m_spriteRenderer->SetViewInfo(viewPixelSize, viewMatrix, projMatrix);
	m_textRenderer->SetViewInfo(viewMatrix * projMatrix, SizeI(viewPixelSize.width, viewPixelSize.height));
	m_frameRectRenderer->SetViewInfo(viewMatrix * projMatrix);
}

//------------------------------------------------------------------------------
SpriteRenderer* InternalContext::GetSpriteRenderer()
{
	return m_spriteRenderer;
}

//------------------------------------------------------------------------------
void InternalContext::ApplyStatus(DrawElementBatch* state, const DefaultStatus& defaultStatus)
{
	m_currentStatePtr = state;
	m_currentStatePtr->ApplyStatus(this, defaultStatus);

	if (m_current != nullptr)
	{
		m_current->OnSetState(m_currentStatePtr);
	}
}

//------------------------------------------------------------------------------
void InternalContext::Flush()
{
	if (m_current != nullptr) m_current->Flush();
}

//------------------------------------------------------------------------------
void InternalContext::SwitchActiveRenderer(detail::IRendererPloxy* renderer)
{
	if (m_current != renderer)
	{
		if (m_current != nullptr)
		{
			m_current->OnDeactivated();
		}

		m_current = renderer;

		if (m_current != nullptr)
		{
			m_current->OnActivated();
			m_current->OnSetState(m_currentStatePtr);
		}
	}
}


//==============================================================================
// DrawElementBatch
//==============================================================================

//------------------------------------------------------------------------------
BatchState::BatchState()
{
	Reset();
}

//------------------------------------------------------------------------------
//void BatchState::SetBlendMode(BlendMode mode)
//{
//	if (m_blendMode != mode)
//	{
//		m_blendMode = mode;
//		m_hashDirty = true;
//	}
//}

//------------------------------------------------------------------------------
void BatchState::SetRenderTarget(int index, RenderTargetTexture* renderTarget)
{
	if (m_renderTargets[index] != renderTarget)
	{
		m_renderTargets[index] = renderTarget;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void BatchState::SetDepthBuffer(DepthBuffer* depthBuffer)
{
	if (m_depthBuffer != depthBuffer)
	{
		m_depthBuffer = depthBuffer;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void BatchState::SetScissorRect(const RectI& scissorRect)
{
	if (m_scissorRect != scissorRect)
	{
		m_scissorRect = scissorRect;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void BatchState::SetBlendMode(BlendMode mode)
{
	if (m_blendMode != mode)
	{
		m_blendMode = mode;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void BatchState::SetCullingMode(CullingMode mode)
{
	if (m_cullingMode != mode)
	{
		m_cullingMode = mode;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void BatchState::SetDepthTestEnabled(bool enabled)
{
	if (m_depthTestEnabled != enabled)
	{
		m_depthTestEnabled = enabled;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void BatchState::SetDepthWriteEnabled(bool enabled)
{
	if (m_depthWriteEnabled != enabled)
	{
		m_depthWriteEnabled = enabled;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void BatchState::SetBrush(Brush* brush)
{
	if (m_brush != brush)
	{
		m_brush = brush;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
Brush* BatchState::GetBrush() const
{
	return m_brush;
}

//------------------------------------------------------------------------------
void BatchState::SetFont(Font* font)
{
	if (m_font != font)
	{
		m_font = font;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
Font* BatchState::GetFont() const
{
	return m_font;
}

//------------------------------------------------------------------------------
void BatchState::Reset()
{
	for (int i = 0; i < Graphics::MaxMultiRenderTargets; ++i)
		m_renderTargets[i] = nullptr;
	m_depthBuffer = nullptr;

	m_blendMode = BlendMode::Normal;
	m_cullingMode = CullingMode::Back;
	m_depthTestEnabled = true;
	m_depthWriteEnabled = true;

	m_brush = nullptr;
	m_font = nullptr;

	m_hashCode = 0;
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void BatchState::ApplyStatus(InternalContext* context, CombinedMaterial* combinedMaterial, const DefaultStatus& defaultStatus)
{
	auto* stateManager = context->GetRenderStateManager();

	// RenderState
	{
		BlendMode blendMode = (combinedMaterial->m_blendMode.IsSet()) ? combinedMaterial->m_blendMode.Get() : m_blendMode;
		CullingMode cullingMode = (combinedMaterial->m_cullingMode.IsSet()) ? combinedMaterial->m_cullingMode.Get() : m_cullingMode;



		// TODO: Base
		RenderState state;
		ContextInterface::MakeBlendMode(blendMode, &state);
		state.Culling = cullingMode;
		//state.AlphaTest = combinedMaterial->m_alphaTest;
		stateManager->SetRenderState(state);

		// スプライトバッチ化のため (TODO: いらないかも。SpriteRenderer では State でそーとしなくなった)
		context->GetSpriteRenderer()->SetState(state);
	}
	// DepthStencilState
	{
		DepthStencilState state;
		state.DepthTestEnabled = (combinedMaterial->m_depthTestEnabled.IsSet()) ? combinedMaterial->m_depthTestEnabled.Get() : m_depthTestEnabled;
		state.DepthWriteEnabled = (combinedMaterial->m_depthWriteEnabled.IsSet()) ? combinedMaterial->m_depthWriteEnabled.Get() : m_depthWriteEnabled;
		stateManager->SetDepthStencilState(state);
	}
	// FrameBuffer
	{
		//if (defaultRenderTarget == nullptr && defaultDepthBuffer == nullptr)
		//{
		//	// TODO: 子DrawListが実行された後のステート復帰。
		//	// もしかしたらオプションにはできないかもしれない。
		//	return;
		//}

		for (int i = 0; i < Graphics::MaxMultiRenderTargets; ++i)
		{
			if (i == 0 && m_renderTargets[i] == nullptr)
				stateManager->SetRenderTarget(i, defaultStatus.defaultRenderTarget);
			else
				stateManager->SetRenderTarget(i, m_renderTargets[i]);
		}
		if (m_depthBuffer == nullptr)
			stateManager->SetDepthBuffer(defaultStatus.defaultDepthBuffer);
		else
			stateManager->SetDepthBuffer(m_depthBuffer);
		// TODO: m_scissorRect
	}
}

//------------------------------------------------------------------------------
uint32_t BatchState::GetHashCode() const
{
	if (m_hashDirty)
	{
		m_hashCode = 0;
		m_hashDirty = false;
		m_hashCode = Hash::CalcHash(reinterpret_cast<const char*>(this), sizeof(BatchState));
	}
	return m_hashCode;
}


//==============================================================================
// DrawElementBatch
//==============================================================================

//------------------------------------------------------------------------------
DrawElementBatch::DrawElementBatch()
{
	Reset();
}

//------------------------------------------------------------------------------
void DrawElementBatch::SetTransfrom(const Matrix& value)
{
	if (m_transfrom != value)
	{
		m_transfrom = value;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void DrawElementBatch::SetCombinedMaterial(CombinedMaterial* value)
{
	if (m_combinedMaterial != value)
	{
		m_combinedMaterial = value;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void DrawElementBatch::SetBuiltinEffect(const BuiltinEffectData& data)
{
	if (!m_builtinEffectData.Equals(data))
	{
		m_builtinEffectData = data;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void DrawElementBatch::SetStandaloneShaderRenderer(bool enabled)
{
	if (m_standaloneShaderRenderer != enabled)
	{
		m_standaloneShaderRenderer = enabled;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
bool DrawElementBatch::IsStandaloneShaderRenderer() const
{
	return m_standaloneShaderRenderer;
}

//------------------------------------------------------------------------------
bool DrawElementBatch::Equal(const BatchState& state_, Material* material, const Matrix& transfrom, const BuiltinEffectData& effectData) const
{
	assert(m_combinedMaterial != nullptr);
	return
		state.GetHashCode() == state_.GetHashCode() &&
		m_combinedMaterial->GetSourceHashCode() == material->GetHashCode() &&
		m_transfrom == transfrom &&
		m_builtinEffectData.GetHashCode() == effectData.GetHashCode();
//#if 1
//	return GetHashCode() == obj.GetHashCode();
//#else
//	if (m_rendererId != obj.m_rendererId) return false;
//
//	if (m_alphaBlendEnabled != obj.m_alphaBlendEnabled) return false;
//	if (m_blendMode != obj.m_blendMode) return false;
//	if (m_cullingMode != obj.m_cullingMode) return false;
//	if (m_alphaTestEnabled != obj.m_alphaTestEnabled) return false;
//
//	if (m_depthTestEnabled != obj.m_depthTestEnabled) return false;
//	if (m_depthWriteEnabled != obj.m_depthWriteEnabled) return false;
//
//	if (m_shaderPass != obj.m_shaderPass) return false;
//	if (m_shaderValueList.GetCount() != obj.m_shaderValueList.GetCount()) return false;
//	for (int i = 0; i < m_shaderValueList.GetCount(); ++i)
//	{
//		if (m_shaderValueList[i].variable != obj.m_shaderValueList[i].variable) return false;
//		if (!m_shaderValueList[i].value.Equals(obj.m_shaderValueList[i].value)) return false;
//	}
//
//	for (int i = 0; i < Graphics::MaxMultiRenderTargets; ++i)
//	{
//		if (m_renderTargets[i] != obj.m_renderTargets[i]) return false;
//	}
//	if (m_depthBuffer != obj.m_depthBuffer) return false;
//	if (m_scissorRect != obj.m_scissorRect) return false;
//
//	return true;
//#endif
}

//------------------------------------------------------------------------------
void DrawElementBatch::Reset()
{
	state.Reset();

	m_transfrom = Matrix::Identity;
	m_combinedMaterial = nullptr;
	m_standaloneShaderRenderer = false;

	m_hashCode = 0;
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void DrawElementBatch::ApplyStatus(InternalContext* context, const DefaultStatus& defaultStatus)
{
	state.ApplyStatus(context, m_combinedMaterial, defaultStatus);
}

//------------------------------------------------------------------------------
size_t DrawElementBatch::GetHashCode() const
{
	if (m_hashDirty || state.IsHashDirty())
	{
		size_t hs = state.GetHashCode();
		m_hashCode = 0;
		m_hashDirty = false;
		m_hashCode = Hash::CalcHash(reinterpret_cast<const char*>(this), sizeof(DrawElementBatch));
		m_hashCode += hs;
	}
	return m_hashCode;
}


//==============================================================================
// DrawElement
//==============================================================================

//------------------------------------------------------------------------------
DrawElement::DrawElement()
	: /*m_transform()
	, */batchIndex(-1)
	, drawingSectionId(DrawingSectionId::None)
	, subsetIndex(0)
	//, zSortDistanceBase(ZSortDistanceBase::CameraDistance)
	, zDistance(0.0f)
	, m_stateFence(0)
{
	boundingSphere.center = Vector3::Zero;
	boundingSphere.radius = -1.0f;
}

//------------------------------------------------------------------------------
DrawElement::~DrawElement()
{
}

//------------------------------------------------------------------------------
const Matrix& DrawElement::GetTransform(DrawElementList* oenerList) const
{
	return oenerList->GetBatch(batchIndex)->GetTransfrom();
}

//------------------------------------------------------------------------------
void DrawElement::MakeElementInfo(DrawElementList* oenerList, const CameraInfo& cameraInfo, ElementInfo* outInfo)
{
	outInfo->viewProjMatrix = &cameraInfo.viewProjMatrix;
	outInfo->WorldMatrix = GetTransform(oenerList);
	outInfo->WorldViewProjectionMatrix = outInfo->WorldMatrix * cameraInfo.viewMatrix * cameraInfo.projMatrix;	// TODO: viewProj はまとめたい
	outInfo->affectedLights = GetAffectedDynamicLightInfos();
}

//------------------------------------------------------------------------------
void DrawElement::MakeSubsetInfo(DrawElementList* oenerList, CombinedMaterial* material, SubsetInfo* outInfo)
{
	outInfo->combinedMaterial = material;
	outInfo->materialTexture = (material != nullptr) ? material->m_mainTexture : nullptr;
}

//------------------------------------------------------------------------------
void DrawElement::MakeBoundingSphere(const Vector3& minPos, const Vector3& maxPos)
{
	Vector3 center = minPos + ((maxPos - minPos) / 2);
	boundingSphere.center = center;
	boundingSphere.radius = std::max(Vector3::Distance(minPos, center), Vector3::Distance(maxPos, center));
}

//------------------------------------------------------------------------------
DynamicLightInfo** DrawElement::GetAffectedDynamicLightInfos()
{
	return nullptr;
}

//------------------------------------------------------------------------------
//void DrawElement::OnJoindDrawList(const Matrix& transform)
//{
//	m_transform = transform;
//}


//==============================================================================
// LightingDrawElement
//==============================================================================

//------------------------------------------------------------------------------
LightingDrawElement::LightingDrawElement()
{
	memset(m_affectedDynamicLightInfos, 0, sizeof(m_affectedDynamicLightInfos));
}


//==============================================================================
// DrawElementList
//==============================================================================

//------------------------------------------------------------------------------
DrawElementList::DrawElementList()
{
	m_commandDataCache.Reserve(512, 4096);	// 適当に
	m_extDataCache.Reserve(512, 4096);
}

//------------------------------------------------------------------------------
void DrawElementList::ClearCommands()
{
	for (int i = 0; i < GetElementCount(); ++i)
	{
		DrawElement* cmd = GetElement(i);
		cmd->~DrawElement();
	}

	m_commandDataCache.Clear();
	m_extDataCache.Clear();
	m_batchList.Clear();

	m_combinedMaterialCache.ReleaseAll();
	m_dynamicLightList.Clear();
}

//------------------------------------------------------------------------------
void DrawElementList::PostAddCommandInternal(const BatchState& state, Material* availableMaterial, const Matrix& transform, const BuiltinEffectData& effectData, DrawElement* element)
{
	if (m_batchList.IsEmpty() || !m_batchList.GetLast().Equal(state, availableMaterial, transform, effectData))
	{
		// CombinedMaterial を作る
		CombinedMaterial* cm = m_combinedMaterialCache.QueryCommandList();
		cm->Combine(nullptr, availableMaterial, effectData);	// TODO

		// 新しく DrawElementBatch を作る
		m_batchList.Add(DrawElementBatch());
		m_batchList.GetLast().state = state;
		m_batchList.GetLast().SetCombinedMaterial(cm);
		m_batchList.GetLast().SetTransfrom(transform);
		m_batchList.GetLast().SetBuiltinEffect(effectData);
	}
	element->batchIndex = m_batchList.GetCount() - 1;
}

//------------------------------------------------------------------------------
//void DrawElementList::ResolveCombinedMaterials()
//{
//	for (DrawElementBatch& state : m_batchList)
//	{
//		if (state.m_material != nullptr)
//		{
//			state.m_material->ResolveCombinedMaterial();
//		}
//	}
//}

//------------------------------------------------------------------------------
void DrawElementList::AddDynamicLightInfo(DynamicLightInfo* lightInfo)
{
	assert(lightInfo != nullptr);
	m_dynamicLightList.Add(lightInfo);
}

//==============================================================================
// SceneRenderer
//==============================================================================

//------------------------------------------------------------------------------
SceneRenderer::SceneRenderer()
{
}

//------------------------------------------------------------------------------
SceneRenderer::~SceneRenderer()
{
}

//------------------------------------------------------------------------------
void SceneRenderer::Initialize(GraphicsManager* manager)
{
	m_manager = manager;
}

//------------------------------------------------------------------------------
void SceneRenderer::OnPreRender(DrawElementList* elementList)
{
}

//------------------------------------------------------------------------------
void SceneRenderer::AddPass(RenderingPass2* pass)
{
	m_renderingPassList.Add(pass);
}

//------------------------------------------------------------------------------
void SceneRenderer::Render(
	RenderView* drawElementListSet,
	DrawElementList* elementList,
	const detail::CameraInfo& cameraInfo,
	RenderTargetTexture* defaultRenderTarget,
	DepthBuffer* defaultDepthBuffer,
	RenderDiag* diag)
{
	if (diag != nullptr) diag->BeginRenderView();
	if (diag != nullptr) diag->BeginDrawList();

	elementList->SetDefaultRenderTarget(defaultRenderTarget);
	elementList->SetDefaultDepthBuffer(defaultDepthBuffer);

	OnPreRender(elementList);

	InternalContext* context = m_manager->GetInternalContext();
	drawElementListSet->m_renderingElementList.Clear();

	// 視点に関する情報の設定
	context->SetViewInfo(cameraInfo.viewPixelSize, cameraInfo.viewMatrix, cameraInfo.projMatrix);

	// ライブラリ外部への書き込み対応
	//context->BeginBaseRenderer()->Clear(ClearFlags::Depth/* | ClearFlags::Stencil*/, Color());

	// 視錘台カリング
	for (int i = 0; i < elementList->GetElementCount(); ++i)
	{
		DrawElement* element = elementList->GetElement(i);
		Sphere boundingSphere = element->GetBoundingSphere();

		if (boundingSphere.radius < 0 ||	// マイナス値なら視錐台と衝突判定しない
			cameraInfo.viewFrustum.Intersects(boundingSphere.center, boundingSphere.radius))
		{
			// このノードは描画できる
			drawElementListSet->m_renderingElementList.Add(element);

			// calculate distance for ZSort
			const Matrix& transform = element->GetTransform(elementList);
			switch (cameraInfo.zSortDistanceBase)
			{
				case ZSortDistanceBase::NodeZ:
					element->zDistance = transform.GetPosition().z;
					break;
				case ZSortDistanceBase::CameraDistance:
					element->zDistance = (transform.GetPosition() - cameraInfo.viewPosition).GetLengthSquared();
					break;
				case ZSortDistanceBase::CameraScreenDistance:
					element->zDistance = Vector3::Dot(
						transform.GetPosition() - cameraInfo.viewPosition,
						transform.GetFront());		// 平面と点の距離
						// TODO: ↑第2引数違くない？要確認
					break;
				default:
					element->zDistance = 0.0f;
					break;
			}
		}
	}

	/*
	if (left->m_priority == right->m_priority)
	{
		// 距離は降順。遠いほうを先に描画する。
		return left->m_zDistance > right->m_zDistance;
	}
	// 優先度は降順。高いほうを先に描画する。
	return left->m_priority > right->m_priority;
	*/
	// 距離は降順。遠いほうを先に描画する
	// 優先度は昇順。高いほうを手前に描画する (UE4 ESceneDepthPriorityGroup)
	// フェンスID は昇順。高いほうを後に描画する
	std::stable_sort(
		drawElementListSet->m_renderingElementList.begin(), drawElementListSet->m_renderingElementList.end(),
		[](const DrawElement* lhs, const DrawElement* rhs)
		{
			if (lhs->m_stateFence == rhs->m_stateFence)
			{
				if (lhs->metadata.priority == rhs->metadata.priority)
					return lhs->zDistance > rhs->zDistance;
				return lhs->metadata.priority < rhs->metadata.priority;
			}
			else
			{
				return lhs->m_stateFence < rhs->m_stateFence;
			}
		}
	);

	DrawElement::DrawArgs drawArgs;
	drawArgs.oenerList = elementList;
	drawArgs.context = context;
	drawArgs.renderer = this;
	drawArgs.defaultRenderTarget = defaultRenderTarget;
	drawArgs.defaultDepthBuffer = defaultDepthBuffer;
	drawArgs.diag = diag;

	for (RenderingPass2* pass : m_renderingPassList)
	{
		// DrawElement 描画
		int currentBatchIndex = -1;
		DrawElementBatch* currentState = nullptr;
		//Shader* currentShader = nullptr;
		for (DrawElement* element : drawElementListSet->m_renderingElementList)
		{
			bool visible = true;

			// ステートの変わり目チェック
			if (element->batchIndex != currentBatchIndex)
			{
				context->Flush();
				currentBatchIndex = element->batchIndex;
				currentState = elementList->GetBatch(currentBatchIndex);
				context->ApplyStatus(currentState, { defaultRenderTarget, defaultDepthBuffer });
				if (diag != nullptr) diag->ChangeRenderStage();
			}

			// 固定の内部シェーダを使わない場合はいろいろ設定する
			if (!currentState->IsStandaloneShaderRenderer())
			{
				CombinedMaterial* material = currentState->GetCombinedMaterial();
				ElementRenderingPolicy policy;
				pass->SelectElementRenderingPolicy(element, material, &policy);
				visible = policy.visible;

				if (visible)
				{
					Shader* shader = policy.shader;

					ElementInfo elementInfo;
					element->MakeElementInfo(elementList, cameraInfo, &elementInfo);

					SubsetInfo subsetInfo;
					element->MakeSubsetInfo(elementList, material, &subsetInfo);

					shader->GetSemanticsManager()->UpdateCameraVariables(cameraInfo);
					shader->GetSemanticsManager()->UpdateElementVariables(elementInfo);
					shader->GetSemanticsManager()->UpdateSubsetVariables(subsetInfo);

					material->ApplyUserShaderValeues(shader);

					auto* stateManager = context->GetRenderStateManager();
					ShaderPass* pass = shader->GetTechniques().GetAt(0)->GetPasses().GetAt(0);	// TODO: DrawList の実行者によって決定する
					stateManager->SetShaderPass(pass);
				}
			}

			// 描画実行
			if (visible)
			{
				if (diag != nullptr) element->ReportDiag(diag);
				element->DrawSubset(drawArgs);
			}
		}

		context->Flush();
	}

	if (diag != nullptr) diag->EndDrawList();
	if (diag != nullptr) diag->EndRenderView();
}




//==============================================================================
// NonShadingRenderer
//==============================================================================

//------------------------------------------------------------------------------
NonShadingRenderer::NonShadingRenderer()
{
}

//------------------------------------------------------------------------------
NonShadingRenderer::~NonShadingRenderer()
{
}

//------------------------------------------------------------------------------
void NonShadingRenderer::Initialize(GraphicsManager* manager)
{
	SceneRenderer::Initialize(manager);

	auto pass = RefPtr<detail::NonShadingRenderingPass>::MakeRef();
	pass->Initialize(manager);
	AddPass(pass);
}


//==============================================================================
// NonShadingRenderingPass
//==============================================================================
//------------------------------------------------------------------------------
NonShadingRenderingPass::NonShadingRenderingPass()
{
}

//------------------------------------------------------------------------------
NonShadingRenderingPass::~NonShadingRenderingPass()
{
}

//------------------------------------------------------------------------------
void NonShadingRenderingPass::Initialize(GraphicsManager* manager)
{
	m_defaultShader = manager->GetBuiltinShader(BuiltinShader::Sprite);
}

//------------------------------------------------------------------------------
Shader* NonShadingRenderingPass::GetDefaultShader() const
{
	return m_defaultShader;
}



//==============================================================================
// ForwardShadingRenderer
//==============================================================================

//------------------------------------------------------------------------------
ForwardShadingRenderer::ForwardShadingRenderer()
{
}

//------------------------------------------------------------------------------
ForwardShadingRenderer::~ForwardShadingRenderer()
{
}

//------------------------------------------------------------------------------
void ForwardShadingRenderer::Initialize(GraphicsManager* manager)
{
	SceneRenderer::Initialize(manager);

	auto pass = RefPtr<detail::ForwardShadingRenderingPass>::MakeRef();
	pass->Initialize(manager);
	AddPass(pass);
}

//------------------------------------------------------------------------------
void ForwardShadingRenderer::OnPreRender(DrawElementList* elementList)
{
	auto& lights = elementList->GetDynamicLightList();
	m_selectingLights.Clear();
	for (DynamicLightInfo* light : lights)
	{
		m_selectingLights.Add(light);
	}

	for (int i = 0; i < elementList->GetElementCount(); i++)
	{
		DrawElement* element = elementList->GetElement(i);
		DynamicLightInfo** lightInfos = element->GetAffectedDynamicLightInfos();
		if (lightInfos != nullptr)
		{
			UpdateAffectLights(element, elementList);
		}
	}
}

//------------------------------------------------------------------------------
void ForwardShadingRenderer::UpdateAffectLights(DrawElement* element, DrawElementList* elementList)
{
	/*
		まず全てのライトに、このノードとの距離をセットする。
		その後近い順にソートして、ソート結果の先頭から必要な数だけ取りだしている。
		ライトの数が少なければどんなアルゴリズムにしても大差はないと思うが、
		ノード単位でソートが実行されるので速度的に少し心配。
		先頭数個が確定したときにソートを終了する等、最適化の余地はあるが…。
	*/

	if (m_selectingLights.GetCount() > DynamicLightInfo::MaxLights)
	{
		// ソート基準値の計算
		for (DynamicLightInfo* light : m_selectingLights)
		{
			light->tempDistance = Vector3::DistanceSquared(element->GetTransform(elementList).GetPosition(), light->transform.GetPosition());
		}

		// ソート (昇順)
		std::stable_sort(m_selectingLights.begin(), m_selectingLights.end(), [](DynamicLightInfo* lhs, DynamicLightInfo* rhs) { return lhs->tempDistance < rhs->tempDistance; });
	}

	// 出力 (足りない分は nullptr で埋める)
	DynamicLightInfo** affectLightList = element->GetAffectedDynamicLightInfos();
	int count = std::min(m_selectingLights.GetCount(), DynamicLightInfo::MaxLights);
	int i = 0;
	for (; i < count; ++i)
	{
		affectLightList[i] = m_selectingLights[i];
	}
	for (; i < DynamicLightInfo::MaxLights; ++i)
	{
		affectLightList[i] = nullptr;
	}
}

//==============================================================================
// ForwardShadingRenderingPass
//==============================================================================
//------------------------------------------------------------------------------
ForwardShadingRenderingPass::ForwardShadingRenderingPass()
{
}

//------------------------------------------------------------------------------
ForwardShadingRenderingPass::~ForwardShadingRenderingPass()
{
}

//------------------------------------------------------------------------------
void ForwardShadingRenderingPass::Initialize(GraphicsManager* manager)
{
	m_defaultShader = manager->GetBuiltinShader(BuiltinShader::LegacyDiffuse);
}

//------------------------------------------------------------------------------
Shader* ForwardShadingRenderingPass::GetDefaultShader() const
{
	return m_defaultShader;
}


//==============================================================================
// InfomationRenderingPass
//==============================================================================

//------------------------------------------------------------------------------
InfomationRenderingPass::InfomationRenderingPass()
{
}

//------------------------------------------------------------------------------
InfomationRenderingPass::~InfomationRenderingPass()
{
}

//------------------------------------------------------------------------------
void InfomationRenderingPass::Initialize(GraphicsManager* manager)
{
	NonShadingRenderingPass::Initialize(manager);
}


//==============================================================================
// CombinedMaterialCache
//==============================================================================

//------------------------------------------------------------------------------
RefPtr<CombinedMaterial> CombinedMaterialCache::CreateObject()
{
	auto m = RefPtr<CombinedMaterial>::MakeRef();
	return m;
}


//==============================================================================
// ScopedStateBlock2
//==============================================================================

//------------------------------------------------------------------------------
ScopedStateBlock2::ScopedStateBlock2(DrawList* renderer)
	: m_renderer(renderer)
	, m_state(renderer->GetState())
{}

//------------------------------------------------------------------------------
ScopedStateBlock2::~ScopedStateBlock2()
{
	Apply();
}

//------------------------------------------------------------------------------
void ScopedStateBlock2::Apply()
{
	m_renderer->SetState(m_state);
}






//==============================================================================
// RenderingPass
//==============================================================================
//------------------------------------------------------------------------------
RenderingPass2::RenderingPass2()
{

}

//------------------------------------------------------------------------------
RenderingPass2::~RenderingPass2()
{
}

//------------------------------------------------------------------------------
void RenderingPass2::SelectElementRenderingPolicy(DrawElement* element, CombinedMaterial* material, ElementRenderingPolicy* outPolicy)
{
	outPolicy->shader = nullptr;
	if (material != nullptr && material->m_shader != nullptr)
	{
		outPolicy->shader = material->m_shader;
	}
	else
	{
		outPolicy->shader = GetDefaultShader();
	}

	// とありあえず全部可
	outPolicy->visible = true;
}

////------------------------------------------------------------------------------
//void RenderingPass2::Initialize(GraphicsManager* manager)
//{
//	m_defaultShader = manager->GetDefaultShader(DefaultShader::NoLightingRendering);
//}
//
////------------------------------------------------------------------------------
//Shader* RenderingPass2::GetDefaultShader() const
//{
//	return m_defaultShader;
//}

////------------------------------------------------------------------------------
//void RenderingPass2::RenderElement(DrawList* renderer, DrawElement* element)
//{
//}
//
////------------------------------------------------------------------------------
//void RenderingPass2::RenderElementSubset(DrawList* renderer, DrawElement* element, int subsetIndex)
//{
//}





//==============================================================================
class ClearElement : public DrawElement
{
public:
	ClearFlags flags;
	Color color;
	float z;
	uint8_t stencil;

	virtual void DrawSubset(const DrawArgs& e) override
	{
		e.context->BeginBaseRenderer()->Clear(flags, color, z, stencil);
	}

	virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("Clear"); }
};

} // namespace detail 



//==============================================================================
// RenderDiag
//==============================================================================

//------------------------------------------------------------------------------
RenderDiagItem::RenderDiagItem()
	: m_name(nullptr)
	, m_subType(SubType::Command)
{
}

//------------------------------------------------------------------------------
String RenderDiagItem::ToString() const
{
	return String(m_name);
}

//==============================================================================
// RenderDiag
//==============================================================================

//------------------------------------------------------------------------------
void RenderDiag::Clear()
{
	for (auto& pair : m_cacheMap)
	{
		pair.second->Clear();
	}
	m_items.clear();
}

//------------------------------------------------------------------------------
void RenderDiag::BeginRenderView()
{
	Instantiate<detail::RenderDiagItem_BeginRenderView>("BeginRenderView")->m_subType = RenderDiagItem::SubType::ScopeBegin;
}

//------------------------------------------------------------------------------
void RenderDiag::EndRenderView()
{
	Instantiate<detail::RenderDiagItem_EndRenderView>("EndRenderView")->m_subType = RenderDiagItem::SubType::ScopeEnd;
}

//------------------------------------------------------------------------------
void RenderDiag::BeginDrawList()
{
	Instantiate<detail::RenderDiagItem_BeginDrawList>("BeginDrawList")->m_subType = RenderDiagItem::SubType::ScopeBegin;
}

//------------------------------------------------------------------------------
void RenderDiag::EndDrawList()
{
	Instantiate<detail::RenderDiagItem_EndDrawList>("EndDrawList")->m_subType = RenderDiagItem::SubType::ScopeEnd;
}

//------------------------------------------------------------------------------
void RenderDiag::ChangeRenderStage()
{
	Instantiate<detail::RenderDiagItem_EndDrawList>("ChangeRenderStage");
}

//------------------------------------------------------------------------------
void RenderDiag::CallCommonElement(const TCHAR* typeName)
{
	Instantiate<detail::RenderDiagItem_Common>(typeName);
}

//------------------------------------------------------------------------------
void RenderDiag::Print()
{
	int level = 0;
	for (RenderDiagItem* item : m_items)
	{
		if (item->m_subType == RenderDiagItem::SubType::ScopeEnd) level--;
		for (int i = 0; i < level; i++) printf("  ");
		if (item->m_subType == RenderDiagItem::SubType::ScopeBegin) level++;

		Console::WriteLine(item->ToString());
	}
}

//==============================================================================
// DrawList
//==============================================================================



//------------------------------------------------------------------------------
DrawList::DrawList()
	: m_currentSectionTopElement(nullptr)
	, m_metadata(nullptr)
	, m_currentStateFence(0)
{
}

//------------------------------------------------------------------------------
DrawList::~DrawList()
{
	m_drawElementList.ClearCommands();
}

//------------------------------------------------------------------------------
void DrawList::Initialize(detail::GraphicsManager* manager)
{
	if (LN_CHECK_ARG(manager != nullptr)) return;
	m_manager = manager;
	m_state.Reset();

	m_defaultMaterial = RefPtr<Material>::MakeRef();
	m_defaultMaterial->Initialize();
}

//------------------------------------------------------------------------------
void DrawList::SetRenderTarget(int index, RenderTargetTexture* renderTarget)
{
	m_state.state.state.SetRenderTarget(index, renderTarget);
	m_currentStateFence++;
}

//------------------------------------------------------------------------------
RenderTargetTexture* DrawList::GetRenderTarget(int index) const
{
	return m_state.state.state.GetRenderTarget(index);
}

//------------------------------------------------------------------------------
void DrawList::SetDepthBuffer(DepthBuffer* depthBuffer)
{
	m_state.state.state.SetDepthBuffer(depthBuffer);
	m_currentStateFence++;
}

//------------------------------------------------------------------------------
DepthBuffer* DrawList::GetDepthBuffer() const
{
	return m_state.state.state.GetDepthBuffer();
}

//------------------------------------------------------------------------------
void DrawList::SetBrush(Brush* brush)
{
	brush = (brush != nullptr) ? brush : Brush::Black;
	m_state.state.state.SetBrush(brush);
}

//------------------------------------------------------------------------------
Brush* DrawList::GetBrush() const
{
	return m_state.state.state.GetBrush();
}

//------------------------------------------------------------------------------
void DrawList::SetFont(Font* font)
{
	font = (font != nullptr) ? font : m_manager->GetFontManager()->GetDefaultFont();

	m_state.state.state.SetFont(font);
}

//------------------------------------------------------------------------------
void DrawList::SetShader(Shader* shader)
{
	m_defaultMaterial->SetShader(shader);
}

//------------------------------------------------------------------------------
Shader* DrawList::GetShader() const
{
	return m_defaultMaterial->GetShader();
}

//------------------------------------------------------------------------------
void DrawList::SetBlendMode(BlendMode mode)
{
	m_state.state.state.SetBlendMode(mode);
}

//------------------------------------------------------------------------------
void DrawList::SetCullingMode(CullingMode mode)
{
	m_state.state.state.SetCullingMode(mode);
}

//------------------------------------------------------------------------------
//void DrawList::SetOpacity(float opacity)
//{
//	m_state.state.
//}

//------------------------------------------------------------------------------
void DrawList::SetDepthTestEnabled(bool enabled)
{
	m_state.state.state.SetDepthTestEnabled(enabled);
}

//------------------------------------------------------------------------------
void DrawList::SetDepthWriteEnabled(bool enabled)
{
	m_state.state.state.SetDepthWriteEnabled(enabled);
}

//------------------------------------------------------------------------------
void DrawList::SetDefaultMaterial(Material* material)
{
	if (LN_CHECK_ARG(material != nullptr)) return;
	m_defaultMaterial = material;
}

//------------------------------------------------------------------------------
void DrawList::SetBuiltinEffectData(const detail::BuiltinEffectData& data)
{
	m_builtinEffectData = data;
}

//------------------------------------------------------------------------------
void DrawList::BeginMakeElements()
{
	m_drawElementList.ClearCommands();
	m_state.Reset();
	SetBrush(nullptr);
	SetFont(nullptr);
	//m_state.state.state.SetFont(m_manager->GetFontManager()->GetDefaultFont());
	m_defaultMaterial->Reset();
	m_builtinEffectData.Reset();
	//m_defaultMaterial->cullingMode = CullingMode::None;
	m_currentSectionTopElement = nullptr;
	m_currentStateFence = 0;
}

//------------------------------------------------------------------------------
void DrawList::EndFrame()
{
	m_manager->GetInternalContext()->Flush();
}

////------------------------------------------------------------------------------
//void DrawList::BeginFrame(RenderTargetTexture* defaultRenderTarget, DepthBuffer* defaultDepthBuffer)
//{
//	m_state.state.m_renderTargets[0] = defaultRenderTarget;
//	m_state.state.m_depthBuffer = defaultDepthBuffer;
//}

//------------------------------------------------------------------------------
void DrawList::SetTransform(const Matrix& transform)
{
	m_state.state.SetTransfrom(transform);
}

//------------------------------------------------------------------------------
void DrawList::Clear(ClearFlags flags, const Color& color, float z, uint8_t stencil)
{
	auto* ptr = m_drawElementList.AddCommand<detail::ClearElement>(m_state.state.state, m_defaultMaterial, Matrix::Identity, m_builtinEffectData);
	ptr->flags = flags;
	ptr->color = color;
	ptr->z = z;
	ptr->stencil = stencil;
	m_currentStateFence++;
}

//------------------------------------------------------------------------------
void DrawList::DrawLinePrimitive(
	const Vector3& position1, const Color& color1,
	const Vector3& position2, const Color& color2)
{
	// TODO: キャッシュと LineStrip
	class DrawElement_DrawLine : public detail::DrawElement
	{
	public:
		Vector3 position1; Color color1;
		Vector3 position2; Color color2;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			e.context->BeginPrimitiveRenderer()->DrawLine(
				position1, color1, position2, color2);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawLine"); }
	};
	auto* ptr = ResolveDrawElement<DrawElement_DrawLine>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer, nullptr);
	ptr->position1 = position1; ptr->color1 = color1;
	ptr->position2 = position2; ptr->color2 = color2;
	ptr->MakeBoundingSphere(Vector3::Min(position1, position2), Vector3::Max(position1, position2));
}

//------------------------------------------------------------------------------
void DrawList::DrawSquarePrimitive(
	const Vector3& position1, const Vector2& uv1, const Color& color1,
	const Vector3& position2, const Vector2& uv2, const Color& color2,
	const Vector3& position3, const Vector2& uv3, const Color& color3,
	const Vector3& position4, const Vector2& uv4, const Color& color4/*,
	ShaderPass* shaderPass*/)
{
	class DrawSquarePrimitiveElement : public detail::LightingDrawElement	// TODO: LightingDrawElement は忘れやすい。デフォルトありでいいと思う
	{
	public:
		Vector3 position[4];
		Vector2 uv[4];
		Color color[4];

		virtual void DrawSubset(const DrawArgs& e) override
		{
			e.context->BeginPrimitiveRenderer()->DrawSquare(
				position[0], uv[0], color[0],
				position[1], uv[1], color[1],
				position[2], uv[2], color[2],
				position[3], uv[3], color[3]);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawSquarePrimitive"); }
	};
	auto* e = ResolveDrawElement<DrawSquarePrimitiveElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer, nullptr);
	e->position[0] = position1; e->uv[0] = uv1; e->color[0] = color1;
	e->position[1] = position2; e->uv[1] = uv2; e->color[1] = color2;
	e->position[2] = position3; e->uv[2] = uv3; e->color[2] = color3;
	e->position[3] = position4; e->uv[3] = uv4; e->color[3] = color4;
	e->MakeBoundingSphere(Vector3::Min(e->position, 4), Vector3::Max(e->position, 4));
}

//------------------------------------------------------------------------------
void DrawList::DrawSquare(float sizeX, float sizeZ, int slicesX, int slicesZ, const Color& color, const Matrix& localTransform, Material* material)
{
	class DrawCylinderElement : public detail::LightingDrawElement	// TODO: LightingDrawElement は忘れやすい。デフォルトありでいいと思う
	{
	public:
		detail::PlaneMeshFactory3 factory;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			auto* r = e.context->BeginPrimitiveRenderer();
			r->DrawMeshFromFactory(factory, detail::PrimitiveRendererMode::TriangleList);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawCylinderElement"); }
	};
	auto* e = ResolveDrawElement<DrawCylinderElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer, material);
	e->factory.Initialize(Vector2(sizeX, sizeZ), slicesX, slicesZ, color, localTransform);
	e->boundingSphere.center = Vector3::Zero;
	e->boundingSphere.radius = Vector3(sizeX, sizeZ, 0).GetLength();
}

//------------------------------------------------------------------------------
void DrawList::DrawArc(float startAngle, float endAngle, float innerRadius, float outerRadius, int slices, const Color& color, const Matrix& localTransform, Material* material)
{
	class DrawArcElement : public detail::LightingDrawElement	// TODO: LightingDrawElement は忘れやすい。デフォルトありでいいと思う
	{
	public:
		detail::ArcMeshFactory factory;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			auto* r = e.context->BeginPrimitiveRenderer();
			r->DrawMeshFromFactory(factory, detail::PrimitiveRendererMode::TriangleList);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawArcElement"); }
	};
	auto* e = ResolveDrawElement<DrawArcElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer, material);
	e->factory.Initialize(startAngle, endAngle, innerRadius, outerRadius, slices, color, localTransform);
	e->boundingSphere.center = Vector3::Zero;
	e->boundingSphere.radius = outerRadius;
}

//------------------------------------------------------------------------------
void DrawList::DrawBox(const Box& box, const Color& color, const Matrix& localTransform, Material* material)
{
	if (box.center != Vector3::Zero) LN_NOTIMPLEMENTED();

	class DrawBoxElement : public detail::LightingDrawElement	// TODO: LightingDrawElement は忘れやすい。デフォルトありでいいと思う
	{
	public:
		detail::RegularBoxMeshFactory factory;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			auto* r = e.context->BeginPrimitiveRenderer();
			r->DrawMeshFromFactory(factory, detail::PrimitiveRendererMode::TriangleList);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawBoxElement"); }
	};
	auto* e = ResolveDrawElement<DrawBoxElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer, material);
	e->factory.Initialize(Vector3(box.width, box.height, box.depth), color, localTransform);

	Vector3 min, max;
	box.GetMinMax(&min, &max);
	e->MakeBoundingSphere(min, max);
}

//------------------------------------------------------------------------------
void DrawList::DrawSphere(float radius, int slices, int stacks, const Color& color, const Matrix& localTransform)
{
	class DrawSphereElement : public detail::LightingDrawElement	// TODO: LightingDrawElement は忘れやすい。デフォルトありでいいと思う
	{
	public:
		detail::RegularSphereMeshFactory factory;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			auto* r = e.context->BeginPrimitiveRenderer();
			r->DrawMeshFromFactory(factory, detail::PrimitiveRendererMode::TriangleList);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawSphereElement"); }
	};
	auto* e = ResolveDrawElement<DrawSphereElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer, nullptr);
	e->factory.Initialize(radius, slices, stacks, color, localTransform);
	e->boundingSphere.center = Vector3::Zero;
	e->boundingSphere.radius = radius;
}

//------------------------------------------------------------------------------
void DrawList::DrawCylinder(float radius, float	height, int slices, int stacks, const Color& color, const Matrix& localTransform)
{
	class DrawCylinderElement : public detail::LightingDrawElement	// TODO: LightingDrawElement は忘れやすい。デフォルトありでいいと思う
	{
	public:
		detail::RegularCylinderMeshFactory factory;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			auto* r = e.context->BeginPrimitiveRenderer();
			r->DrawMeshFromFactory(factory, detail::PrimitiveRendererMode::TriangleList);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawCylinder"); }
	};
	auto* e = ResolveDrawElement<DrawCylinderElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer, nullptr);
	e->factory.Initialize(radius, height, slices, stacks, color, localTransform);
	e->boundingSphere.center = Vector3::Zero;
	e->boundingSphere.radius = Vector3(radius, height, 0).GetLength();
}

//------------------------------------------------------------------------------
void DrawList::DrawCone(float radius, float height, int slices, const Color& color, const Matrix& localTransform)
{
	class DrawConeElement : public detail::LightingDrawElement	// TODO: LightingDrawElement は忘れやすい。デフォルトありでいいと思う
	{
	public:
		detail::RegularConeMeshFactory factory;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			auto* r = e.context->BeginPrimitiveRenderer();
			r->DrawMeshFromFactory(factory, detail::PrimitiveRendererMode::TriangleList);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawCone"); }
	};
	auto* e = ResolveDrawElement<DrawConeElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer, nullptr);
	e->factory.Initialize(radius, height, slices, color, localTransform);
	e->boundingSphere.center = Vector3::Zero;
	e->boundingSphere.radius = Vector3(radius, height, 0).GetLength();
}

//------------------------------------------------------------------------------
void DrawList::DrawMesh(MeshResource* mesh, int subsetIndex, Material* material)
{
	DrawMeshResourceInternal(mesh, subsetIndex, material);
}

//------------------------------------------------------------------------------
//void DrawList::DrawMesh(StaticMeshModel* mesh, int subsetIndex, Material* material)
//{
//	DrawMeshSubsetInternal(mesh, subsetIndex, material);
//}

//------------------------------------------------------------------------------
void DrawList::Blit(Texture* source)
{
	BlitInternal(source, nullptr, Matrix::Identity, nullptr);
}

//------------------------------------------------------------------------------
void DrawList::Blit(Texture* source, const Matrix& transform)
{
	BlitInternal(source, nullptr, transform, nullptr);
}

//------------------------------------------------------------------------------
void DrawList::Blit(Texture* source, RenderTargetTexture* dest, const Matrix& transform)
{
	BlitInternal(source, dest, transform, nullptr);
}

//------------------------------------------------------------------------------
void DrawList::Blit(Texture* source, RenderTargetTexture* dest, Material* material)
{
	BlitInternal(source, dest, Matrix::Identity, material);
}

//------------------------------------------------------------------------------
void DrawList::DrawGlyphRun(const PointF& position, GlyphRun* glyphRun)
{
	class DrawElement_DrawGlyphRun : public detail::DrawElement
	{
	public:
		RefPtr<GlyphRun>	glyphRun;
		PointF position;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			e.context->BeginTextRenderer()->DrawGlyphRun(GetTransform(e.oenerList), position, glyphRun);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawGlyphRun"); }
	};

	auto* e = ResolveDrawElement<DrawElement_DrawGlyphRun>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_textRenderer, nullptr);
	e->glyphRun = glyphRun;
	e->position = position;
	//e->boundingSphere = ;	// TODO
}

//------------------------------------------------------------------------------
void DrawList::DrawText_(const StringRef& text, const PointF& position)
{
	DrawText_(text, Rect(position, FLT_MAX, FLT_MAX), StringFormatFlags::LeftAlignment);
}

//------------------------------------------------------------------------------
void DrawList::DrawText_(const StringRef& text, const Rect& rect, StringFormatFlags flags)
{
	class DrawElement_DrawText : public detail::DrawElement
	{
	public:
		String text;	// TODO: BlukData
		Rect rect;
		StringFormatFlags flags;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			e.context->BeginTextRenderer()->DrawString(GetTransform(e.oenerList), text.c_str(), text.GetLength(), rect, flags);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawText"); }
	};

	auto* e = ResolveDrawElement<DrawElement_DrawText>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_textRenderer, nullptr);
	e->text = text;
	e->rect = rect;
	e->flags = flags;
	//e->boundingSphere = ;	// TODO
}

//------------------------------------------------------------------------------
void DrawList::DrawChar(TCHAR ch, const PointF& position)
{
	class DrawElement_DrawChar : public detail::DrawElement
	{
	public:
		TCHAR ch;
		PointF position;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			e.context->BeginVectorTextRenderer()->DrawChar(GetTransform(e.oenerList), ch, Rect(position, 0, 0), TextLayoutOptions::None);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawChar"); }
	};

	// TODO: UTF32 変換

	auto* e = ResolveDrawElement<DrawElement_DrawChar>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_vectorTextRenderer, nullptr);
	e->ch = ch;
	e->position = position;
	//e->boundingSphere = ;	// TODO
}

//------------------------------------------------------------------------------
void DrawList::DrawText2(const StringRef& text, const Rect& rect)
{
	class DrawElement_DrawString : public detail::DrawElement
	{
	public:
		detail::CommandDataCache::DataHandle utf32DataHandle;
		int length;
		Rect rect;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			e.context->BeginVectorTextRenderer()->DrawString(
				GetTransform(e.oenerList), 
				(const UTF32*)e.oenerList->GetExtData(utf32DataHandle),
				length,
				rect,
				TextLayoutOptions::None);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawString"); }
	};

	const ByteBuffer& utf32Data = m_manager->GetFontManager()->GetTCharToUTF32Converter()->Convert(text.GetBegin(), text.GetLength() * sizeof(TCHAR));

	auto* e = ResolveDrawElement<DrawElement_DrawString>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_vectorTextRenderer, nullptr);
	e->utf32DataHandle = m_drawElementList.AllocExtData(utf32Data.GetSize());
	e->length = utf32Data.GetSize() / sizeof(UTF32);
	e->rect = rect;
	//e->boundingSphere = ;	// TODO

	memcpy(m_drawElementList.GetExtData(e->utf32DataHandle), utf32Data.GetConstData(), utf32Data.GetSize());
}

//------------------------------------------------------------------------------
void DrawList::DrawSprite(
	const Vector3& position,
	const Size& size,
	const Vector2& anchor,
	Texture* texture,
	const Rect& srcRect,
	const Color& color,
	SpriteBaseDirection baseDirection,
	BillboardType billboardType,
	Material* material)
{
	class DrawElement_DrawSprite : public detail::DrawElement
	{
	public:
		Vector3 position;
		Vector2 size;
		Vector2 anchorRatio;
		RefPtr<Texture> texture;
		Rect srcRect;
		Color color;
		SpriteBaseDirection baseDirection;
		BillboardType billboardType;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			auto* r = e.context->BeginSpriteRenderer();
			r->SetTransform(GetTransform(e.oenerList));
			r->DrawRequest(position, size, anchorRatio, texture, srcRect, color, baseDirection, billboardType);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawSprite"); }
	};

	auto* ptr = ResolveDrawElement<DrawElement_DrawSprite>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_spriteRenderer, material);
	ptr->position = position;
	ptr->size.Set(size.width, size.height);
	ptr->anchorRatio = anchor;
	ptr->texture = texture;
	ptr->srcRect = srcRect;
	ptr->color = color;
	ptr->baseDirection = baseDirection;
	ptr->billboardType = billboardType;
	detail::SpriteRenderer::MakeBoundingSphere(ptr->size, baseDirection, &ptr->boundingSphere);
	ptr->boundingSphere.center += m_state.state.GetTransfrom().GetPosition();	// TODO: 他と共通化
}


//------------------------------------------------------------------------------
class DrawElement_DrawNanoVGCommands : public detail::DrawElement
{
public:
	detail::NanoVGCommandList*	m_commandList = nullptr;

	detail::NanoVGCommandList* GetGCommandList(DrawList* owner)
	{
		if (m_commandList == nullptr)
		{
			m_commandList = owner->GetManager()->GetNanoVGCommandListCache()->QueryCommandList();
		}
		return m_commandList;
	}

	virtual void DrawSubset(const DrawArgs& e) override
	{
		auto* r = e.context->BeginNanoVGRenderer();
		//auto cl = r->TakeCommandList();
		//detail::NanoVGCommandHelper::nvgRect(cl, rect.x, rect.y, rect.width, rect.height);
		//detail::NanoVGCommandHelper::nvgFill(cl);
		r->ExecuteCommand(m_commandList);
		m_commandList = nullptr;
	}
	virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawNanoVGCommands"); }
};

void DrawList::DrawRectangle(const Rect& rect)
{
	if (m_state.state.state.GetBrush() != nullptr &&
		(m_state.state.state.GetBrush()->GetImageDrawMode() == BrushImageDrawMode::BoxFrame || m_state.state.state.GetBrush()->GetImageDrawMode() == BrushImageDrawMode::BorderFrame))
	{
		DrawFrameRectangle(rect);
		return;
	}

	auto* ptr = ResolveDrawElement<DrawElement_DrawNanoVGCommands>(detail::DrawingSectionId::NanoVG, m_manager->GetInternalContext()->m_nanoVGRenderer, nullptr);
	auto* list = ptr->GetGCommandList(this);
	detail::NanoVGCommandHelper::nvgBeginPath(list);
	detail::NanoVGCommandHelper::nvgRect(list, rect.x, rect.y, rect.width, rect.height);
	detail::NanoVGCommandHelper::nvgFill(list);
	//ptr->rect = rect;
	// TODO: カリング
	
}

void DrawList::DrawScreenRectangle()
{
	Blit(nullptr);
}

//------------------------------------------------------------------------------
void DrawList::AddDynamicLightInfo(detail::DynamicLightInfo* lightInfo)
{
	m_drawElementList.AddDynamicLightInfo(lightInfo);
}

//------------------------------------------------------------------------------
void DrawList::PushMetadata(const DrawElementMetadata* metadata)
{
	if (LN_CHECK_STATE(m_metadata == nullptr)) return;
	m_metadata = metadata;
	// TODO: stack
}

//------------------------------------------------------------------------------
const DrawElementMetadata* DrawList::GetMetadata()
{
	return m_metadata;
}

//------------------------------------------------------------------------------
void DrawList::PopMetadata()
{
	m_metadata = nullptr;
}
//
////------------------------------------------------------------------------------
//template<typename TElement>
//TElement* DrawList::ResolveDrawElement(detail::DrawingSectionId sectionId, detail::IRendererPloxy* renderer, Material* userMaterial)
//{
//	Material* availableMaterial = (userMaterial != nullptr) ? userMaterial : m_defaultMaterial.Get();
//
//	// これを決定してから比較を行う
//	m_state.state.SetStandaloneShaderRenderer(renderer->IsStandaloneShader());
//
//	m_state.state.m_rendererId = reinterpret_cast<intptr_t>(renderer);
//
//	const DrawElementMetadata* userMetadata = GetMetadata();
//	const DrawElementMetadata* metadata = (userMetadata != nullptr) ? userMetadata : &DrawElementMetadata::Default;
//
//	// 何か前回追加された DrawElement があり、それと DrawingSectionId、State が一致するならそれに対して追記できる
//	if (sectionId != detail::DrawingSectionId::None &&
//		m_currentSectionTopElement != nullptr &&
//		m_currentSectionTopElement->drawingSectionId == sectionId &&
//		m_currentSectionTopElement->metadata.Equals(*metadata) &&
//		m_drawElementList.GetBatch(m_currentSectionTopElement->batchIndex)->Equal(m_state.state.state, availableMaterial, m_state.state.GetTransfrom()))
//	{
//		return static_cast<TElement*>(m_currentSectionTopElement);
//	}
//
//	// DrawElement を新しく作る
//	TElement* element = m_drawElementList.AddCommand<TElement>(m_state.state.state, availableMaterial, m_state.state.GetTransfrom());
//	//element->OnJoindDrawList(m_state.transfrom);
//	element->drawingSectionId = sectionId;
//	element->metadata = *metadata;
//	m_currentSectionTopElement = element;
//	return element;
//}

//------------------------------------------------------------------------------
void DrawList::DrawMeshResourceInternal(MeshResource* mesh, int subsetIndex, Material* material)
{
	class DrawElement_DrawMeshResourceInternal : public detail::LightingDrawElement
	{
	public:
		RefPtr<MeshResource>	mesh;
		int startIndex;
		int primitiveCount;
		PrimitiveType primitiveType;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			e.context->BeginMeshRenderer()->DrawMesh(mesh, startIndex, primitiveCount, primitiveType);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawMeshResourceInternal"); }
	};

	MeshAttribute attr;
	mesh->GetMeshAttribute(subsetIndex, &attr);
	if (attr.PrimitiveNum == 0) return;		// not need draw

	auto* e = ResolveDrawElement<DrawElement_DrawMeshResourceInternal>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_meshRenderer, material);
	e->subsetIndex = subsetIndex;
	e->mesh = mesh;
	e->startIndex = attr.StartIndex;
	e->primitiveCount = attr.PrimitiveNum;
	e->primitiveType = attr.primitiveType;
	//e->boundingSphere = ;	// TODO
}

//------------------------------------------------------------------------------
//void DrawList::DrawMeshSubsetInternal(StaticMeshModel* mesh, int subsetIndex, Material* material)
//{
//	/* 
//	 * この時点では MeshResource ではなく StaticMeshModel が必要。
//	 * LOD リソースがある場合、実際に書くときの視点情報を元に、描画する LOD リソースを選択する必要がある。
//	 */
//
//	class DrawElement_DrawMeshInternal : public detail::LightingDrawElement
//	{
//	public:
//		RefPtr<StaticMeshModel>	mesh;
//		int startIndex;
//		int primitiveCount;
//		PrimitiveType primitiveType;
//
//		virtual void DrawSubset(detail::DrawElementList* oenerList, detail::InternalContext* context) override
//		{
//			context->BeginMeshRenderer()->DrawMesh(mesh->GetMeshResource(), startIndex, primitiveCount, primitiveType);
//		}
//	};
//	MeshAttribute attr;
//	mesh->GetMeshResource()->GetMeshAttribute(subsetIndex, &attr);
//	if (attr.PrimitiveNum == 0) return;		// not need draw
//
//	auto* e = ResolveDrawElement<DrawElement_DrawMeshInternal>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_meshRenderer, material);
//	e->subsetIndex = subsetIndex;
//	e->mesh = mesh;
//	e->startIndex = attr.StartIndex;
//	e->primitiveCount = attr.PrimitiveNum;
//	e->primitiveType = attr.primitiveType;
//	//e->boundingSphere = ;	// TODO
//}

//------------------------------------------------------------------------------
void DrawList::BlitInternal(Texture* source, RenderTargetTexture* dest, const Matrix& transform, Material* material)
{

	class DrawElement_BlitInternal : public detail::DrawElement
	{
	public:
		Matrix			overrideTransform;
		RefPtr<Texture>	source;

		virtual void MakeElementInfo(detail::DrawElementList* oenerList, const detail::CameraInfo& cameraInfo, detail::ElementInfo* outInfo) override
		{
			DrawElement::MakeElementInfo(oenerList, cameraInfo, outInfo);
			outInfo->WorldViewProjectionMatrix = overrideTransform;
		}
		virtual void MakeSubsetInfo(detail::DrawElementList* oenerList, detail::CombinedMaterial* material, detail::SubsetInfo* outInfo) override
		{
			DrawElement::MakeSubsetInfo(oenerList, material, outInfo);

			// MaterialTexture を上書きする
			outInfo->materialTexture = source;
		}

		virtual void DrawSubset(const DrawArgs& e) override
		{
			e.context->BeginBlitRenderer()->Blit();
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("BlitInternal"); }
	};

	if (dest != nullptr)
	{
		SetRenderTarget(0, dest);
	}

	auto* e = ResolveDrawElement<DrawElement_BlitInternal>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_blitRenderer, material);
	e->overrideTransform = transform;
	e->source = source;
}

//------------------------------------------------------------------------------
void DrawList::DrawFrameRectangle(const Rect& rect)
{
	class DrawElement_DrawFrameRectangle : public detail::DrawElement
	{
	public:
		Rect rect;

		virtual void MakeSubsetInfo(detail::DrawElementList* oenerList, detail::CombinedMaterial* material, detail::SubsetInfo* outInfo) override
		{
			DrawElement::MakeSubsetInfo(oenerList, material, outInfo);

			// MaterialTexture を上書きする
			outInfo->materialTexture = oenerList->GetBatch(batchIndex)->state.GetBrush()->GetTexture();
		}
		virtual void DrawSubset(const DrawArgs& e) override
		{
			auto* r = e.context->BeginFrameRectRenderer();
			r->Draw(GetTransform(e.oenerList), rect);
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("DrawFrameRectangle"); }
	};
	auto* ptr = ResolveDrawElement<DrawElement_DrawFrameRectangle>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_frameRectRenderer, nullptr);
	ptr->rect = rect;
	// TODO: カリング
}

//------------------------------------------------------------------------------
void DrawList::RenderSubView(RenderView* listSet, detail::SceneRenderer* renderer, RenderTargetTexture* defaultRenderTarget, DepthBuffer* defaultDepthBuffer)
{
	class DrawElement_RenderSubView : public detail::DrawElement
	{
	public:
		RenderView* listSet;
		detail::SceneRenderer* renderer;
		RefPtr<RenderTargetTexture>	defaultRenderTarget;
		RefPtr<DepthBuffer> defaultDepthBuffer;

		virtual void DrawSubset(const DrawArgs& e) override
		{
			// それぞれ、省略されていれば親のを使う
			detail::SceneRenderer* primRenderer = (renderer != nullptr) ? renderer : e.renderer;
			RenderTargetTexture* primRenderTarget = (defaultRenderTarget != nullptr) ? defaultRenderTarget : e.defaultRenderTarget;
			DepthBuffer* primDepthBuffer = (defaultDepthBuffer != nullptr) ? defaultDepthBuffer : e.defaultDepthBuffer;

			// TODO: scoped change block
			auto* status = e.context->GetCurrentStatus();

			// TODO: Zソートは結合してやりたいので、外側で for するのはだめ。
			// また、バッチはindexで管理しているので、Element は親 list を参照する必要がある。
			for (auto& list : listSet->m_lists)
			{
				primRenderer->Render(listSet, list, listSet->m_cameraInfo, primRenderTarget, primDepthBuffer, e.diag);
			}

			// ステート復帰
			e.context->ApplyStatus(status, { e.defaultRenderTarget, e.defaultDepthBuffer });
		}
		virtual void ReportDiag(RenderDiag* diag) override { diag->CallCommonElement("RenderSubView"); }
	};

	// TODO: m_frameRectRenderer は違う気がする・・・
	auto* e = ResolveDrawElement<DrawElement_RenderSubView>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_frameRectRenderer, nullptr);
	//e->elementList = listSet->m_lists[0];
	//e->cameraInfo = listSet->m_cameraInfo;
	e->listSet = listSet;
	e->renderer = renderer;
	e->defaultRenderTarget = defaultRenderTarget;
	e->defaultDepthBuffer = defaultDepthBuffer;
}

LN_NAMESPACE_END
