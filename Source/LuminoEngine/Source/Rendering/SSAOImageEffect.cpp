﻿
#include "../Internal.h"
//#include <Lumino/Graphics/Texture.h>
//#include <Lumino/Graphics/Shader.h>
#include <Lumino/Graphics/Material.h>
#include <Lumino/Rendering/Rendering.h>
#include <Lumino/Rendering/SSAOImageEffect.h>
#include <Lumino/Scene/Camera.h>
#include "../Animation/AnimationManager.h"
#include "../Graphics/GraphicsManager.h"
#include "../Rendering/ClusteredShadingSceneRenderer.h"

LN_NAMESPACE_BEGIN

namespace detail
{

	extern RenderTargetTexture* g_m_normalRenderTarget;
}

//==============================================================================
// SSAOImageEffect
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(SSAOImageEffect, PostEffect);

SSAOImageEffect::SSAOImageEffect()
	: m_depthMap(nullptr)
{
	initializeProperties();
}

SSAOImageEffect::~SSAOImageEffect()
{
}

void SSAOImageEffect::initialize()
{
	PostEffect::initialize();

	m_material = Object::makeRef<CommonMaterial>();
	m_material->setShader(Object::makeRef<Shader>(detail::GraphicsManager::getInstance(), _T("D:/Proj/LN/HC1/External/Lumino/Source/LuminoEngine/Source/Rendering/Resource/SSAOImageEffect.fx")));
	
	m_material2 = Object::makeRef<CommonMaterial>();
	m_material2->setShader(Object::makeRef<Shader>(detail::GraphicsManager::getInstance(), _T("D:/Proj/LN/HC1/External/Lumino/Source/LuminoEngine/Source/Rendering/Resource/SSAOImageEffectBlur.fx")));

	m_ssaoRenderTarget = Ref<RenderTargetTexture>::makeRef();
	m_ssaoRenderTarget->createImpl(detail::GraphicsManager::getInstance(), SizeI(640, 480), 1, TextureFormat::R8G8B8A8);
}

void SSAOImageEffect::onAttached()
{
	m_depthMap = nullptr;

	// TODO: onAttach で保持
	auto* layer = dynamic_cast<WorldRenderView*>(GetOwnerLayer());
	if (layer && layer->getClusteredShadingSceneRenderer())
	{
		m_depthMap = layer->getClusteredShadingSceneRenderer()->getDepthPrepass()->m_depthMap;
	}

	PostEffect::onAttached();
}

void SSAOImageEffect::onRender(DrawList* context, RenderTargetTexture* source, RenderTargetTexture* destination)
{
	if (m_depthMap)
	{
		m_material->setTextureParameter(_T("normalDepth"), m_depthMap);
		//printf("ToneImageEffect::onRender %p > %p\n", source, destination);
		//m_material->setVectorParameter(_LT("_Tone"), m_tone);
		context->blit(source, m_ssaoRenderTarget, m_material);




		m_material2->setTextureParameter(_T("_SsaoTexture"), m_ssaoRenderTarget);
		context->blit(source, destination, m_material2);

		//m_shader.varTone->setVector(Tone.Get());
		//m_shader.varScreenTexture->setTexture(source);
		//context->blit(nullptr, destination, m_shader.shader);
	}
}

LN_NAMESPACE_END
