﻿
#include "Internal.h"
#include <Lumino/Rendering/RenderingContext.h>
#include <Lumino/Rendering/SceneRenderer.h>
#include <Lumino/World.h>
#include <Lumino/Scene/Camera.h>
#include <Lumino/Scene/OffscreenWorldView.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// OffscreenWorldSubRenderView
/*
■[MME メモ] DefaultEffect アノテーションのためのグループ分け
	ひとつの OffscreenScene 内でのエフェクトファイルの割り当て方法や
	描画そのものの有無を決めるため、ワイルドカード指定可能なオブジェクト名の
	マッチング判定が行われる。
	ただ、この処理を毎フレーム行うのはかなり処理時間的にキツイ。

	そこで、一度マッチングを行った後はどのエフェクトを割り当てるかを示す
	整数値を持たせて、以降これを使って割り当てを決める。
	例えば以下のようになる。

	string DefaultEffect =
	"self = hide;"				… グループ 0
	"Mirror*.x = hide;"			… グループ 0
	"Player1=MirrorMain.fx;";	… グループ 1
	"Player2=MirrorMain.fx;";	… グループ 1
	"*=MirrorObject.fx;";		… グループ 2

	もしいずれにもマッチしなければグループ -1。
	グループ -1 は非表示を表す。
*/
//==============================================================================
bool g_ofs = false;
//------------------------------------------------------------------------------
OffscreenWorldSubRenderView::OffscreenWorldSubRenderView()
	: m_id(0)
{
}

//------------------------------------------------------------------------------
OffscreenWorldSubRenderView::~OffscreenWorldSubRenderView()
{
}

//------------------------------------------------------------------------------
void OffscreenWorldSubRenderView::initialize()
{
	WorldRenderViewBase::initialize();
	m_renderer = newObject<RenderingContext>();

	//m_renderView = Ref<RenderView>::MakeRef();
	//m_renderView->
	m_lists.add(m_renderer->getDrawElementList());

	setLayerCullingMask(0xFFFFFFFF);	// TODO
}

//------------------------------------------------------------------------------
RenderTargetTexture* OffscreenWorldSubRenderView::getRenderTarget() const
{
	return m_renderTarget;
}

//------------------------------------------------------------------------------
void OffscreenWorldSubRenderView::hideVisual(VisualComponent* renderObject)
{
	auto* filterInfo = updateRenderObjectFilterInfo(renderObject);
	filterInfo->effectGroup = -1;
}

//------------------------------------------------------------------------------
void OffscreenWorldSubRenderView::filterWorldMatrix(Matrix* outMatrix)
{
	//(*outMatrix) = (*outMatrix) * Matrix::makeReflection(Plane(Vector3::UnitY));
}

//------------------------------------------------------------------------------
Matrix OffscreenWorldSubRenderView::calculateViewMatrix(RenderView* mainRenderView)
{
#if 0
	return Matrix::makeReflection(Vector3::UnitY) * mainRenderView->m_cameraInfo.viewMatrix;
#else
	Vector3 pos = mainRenderView->m_cameraInfo.viewPosition;
	Vector3 dir = mainRenderView->m_cameraInfo.viewDirection;
	//pos.y *= -1;
	//dir.y *= -1;
	// TODO:
	return Matrix::makeReflection(Vector3::UnitY) * Matrix::makeLookAtRH(pos, pos + dir, /*-*/Vector3::UnitY);
#endif
	//const Matrix& worldMatrix = mainViewCamera->getOwnerObject()->transform.getWorldMatrix();
	//Matrix viewMatrix = Matrix::makeLookAtLH(worldMatrix.getPosition(), Vector3(0, 0, 0), -Vector3::UnitY);

	//Matrix::makeTranspose


	//return mainRenderView->m_cameraInfo.viewMatrix;//mainViewCamera->getViewMatrix(); //viewMatrix;// 
}

//------------------------------------------------------------------------------
Matrix OffscreenWorldSubRenderView::calculateProjectionMatrix(RenderView* mainRenderView)
{
	// TODO:
	return Matrix::makePerspectiveFovRH(Math::PI / 3.0f, 640 / 480, 0.3, 1000);
	//return mainRenderView->m_cameraInfo.projMatrix; //mainViewCamera->getProjectionMatrix();
}

//------------------------------------------------------------------------------
void OffscreenWorldSubRenderView::renderWorld(World* world, RenderView* mainRenderView)
{
	m_cameraInfo.dataSourceId = reinterpret_cast<intptr_t>(mainRenderView) + 1;
	m_cameraInfo.viewPixelSize = mainRenderView->getViewSize();
	m_cameraInfo.viewPosition = mainRenderView->m_cameraInfo.viewPosition;//mainViewCamera->getTransform()->getWorldMatrix().getPosition();
	m_cameraInfo.viewDirection = mainRenderView->m_cameraInfo.viewDirection;	// TODO: Mirror 考慮
	m_cameraInfo.viewMatrix = calculateViewMatrix(mainRenderView);
	m_cameraInfo.projMatrix = calculateProjectionMatrix(mainRenderView);
	m_cameraInfo.viewProjMatrix = m_cameraInfo.viewMatrix * m_cameraInfo.projMatrix;
	m_cameraInfo.viewFrustum = mainRenderView->m_cameraInfo.viewFrustum;//mainViewCamera->getViewFrustum();	// TODO: この View 独自処理にしたい
	m_cameraInfo.zSortDistanceBase = mainRenderView->m_cameraInfo.zSortDistanceBase;
	m_cameraInfo.nearClip = mainRenderView->m_cameraInfo.nearClip;
	m_cameraInfo.farClip = mainRenderView->m_cameraInfo.farClip;



	// TODO: ↑のm_cameraInfoを渡す
	//m_renderer->setCurrentCamera(mainViewCamera);



	m_renderer->beginMakeElements();



	RenderTargetTexture* backbuffer = m_renderer->getRenderTarget(0);

	if (m_renderTarget == nullptr)
	{
		m_renderTarget = newObject<RenderTargetTexture>(SizeI::fromFloatSize(mainRenderView->getViewSize()), 1, TextureFormat::R8G8B8X8);
	}
	//else if (m_renderTarget->GetSize() != backbuffer->GetSize())
	//{
	//	LN_NOTIMPLEMENTED();
	//}

	// TODO: 深度バッファやクリア方法の指定
	m_renderer->setRenderTarget(0, m_renderTarget);
	//g->setDepthBuffer(m_depthBuffer);
	m_renderer->clear(ClearFlags::All, Color::White, 1.0f, 0);
	
	g_ofs = true;
	world->render(m_renderer, this, WorldDebugDrawFlags::None, getLayerCullingMask(), this);	// TODO: LayerMask, debugdraw の指定

	g_ofs = false;

	// 戻す
	m_renderer->setRenderTarget(0, backbuffer);



	mainRenderView->getSceneRenderer()->render(this, m_renderTarget, nullptr, nullptr, true, Color::White);	// TODO: diag

	// user override
	//OnUpdateRenderViewPoint(m_renderView);

	//DrawList* r = world->getRenderer();
	//r->renderSubView(this);
	//LN_NOTIMPLEMENTED();
}

//------------------------------------------------------------------------------
bool OffscreenWorldSubRenderView::filterRenderObject(VisualComponent* renderObject)
{
	auto* filterInfo = updateRenderObjectFilterInfo(renderObject);
	return filterInfo->effectGroup >= 0;
}

//------------------------------------------------------------------------------
detail::OffscreenFilterInfo* OffscreenWorldSubRenderView::updateRenderObjectFilterInfo(VisualComponent* renderObject)
{
	auto* filterInfo = renderObject->getOffscreenFilterInfo(m_id);

	// オーナーが this ではない。再割り当てする。
	// (renderObject が新しく作成されたオブジェクトであるか、前の Offscreen が解放され ID が返却された後、新たに作成された Offscreen が同じ ID を取得した場合)
	if (filterInfo->ownerOffscreenView != this)
	{
		filterInfo->ownerOffscreenView = this;
		filterInfo->effectGroup = 0;
	}
	return filterInfo;
}











//==============================================================================
// SkyComponent
//==============================================================================
//------------------------------------------------------------------------------
SkyComponent::SkyComponent()
{
}

//------------------------------------------------------------------------------
SkyComponent::~SkyComponent()
{
}

//------------------------------------------------------------------------------
void SkyComponent::initialize()
{
	VisualComponent::initialize();
	setLayer(LayerMask::GetLayer(BuiltinLayers::Background3D));
	setOrderInLayer(SHRT_MIN);

	{

		auto shader = ln::Shader::create(_LT("D:/Proj/LN/HC1/External/Lumino/Source/LuminoEngine/Source/Scene/Resource/Sky.fx"));
		m_skyMaterial = newObject<CommonMaterial>();
		m_skyMaterial->setShader(shader);
	}
}

//------------------------------------------------------------------------------
void SkyComponent::onRender2(RenderingContext* renderer)
{
	//

	{
		Matrix ref;
		//m_skyMaterial->setMatrixParameter("_refrect", ref);
		if (g_ofs)
		{
			ref = Matrix::makeReflection(Plane(Vector3::UnitY));
		}

		//auto* cam = renderer->getCurrentCamera();
		const auto& cam = renderer->getRenderView()->m_cameraInfo;
		Matrix refVP = ref * cam.viewProjMatrix;// ->getViewProjectionMatrix();
		auto vtow = [refVP](const Vector3& pos) { return Vector3::unproject(pos, refVP, 0, 0, 640, 480, 0.3f, 1000); };

#if 0
		Vector3 frustumRayTL = Vector3::normalize(cam->viewportToWorldPoint(Vector3(0, 0, 1)) - cam->viewportToWorldPoint(Vector3(0, 0, 0)));
		Vector3 frustumRayTR = Vector3::normalize(cam->viewportToWorldPoint(Vector3(640, 0, 1)) - cam->viewportToWorldPoint(Vector3(640, 0, 0)));
		Vector3 frustumRayBL = Vector3::normalize(cam->viewportToWorldPoint(Vector3(0, 480, 1)) - cam->viewportToWorldPoint(Vector3(0, 480, 0)));
#else
		Vector3 frustumRayTL = Vector3::normalize(vtow(Vector3(0, 0, 1)) - vtow(Vector3(0, 0, 0))/*cam->viewportToWorldPoint(Vector3(0, 0, 0))*/);
		Vector3 frustumRayTR = Vector3::normalize(vtow(Vector3(640, 0, 1)) - vtow(Vector3(640, 0, 0))/*cam->viewportToWorldPoint(Vector3(640, 0, 0))*/);
		Vector3 frustumRayBL = Vector3::normalize(vtow(Vector3(0, 480, 1)) - vtow(Vector3(0, 480, 0))/*cam->viewportToWorldPoint(Vector3(0, 480, 0))*/);
#endif
		m_skyMaterial->setVectorParameter(_LT("frustumRayTL"), Vector4(frustumRayTL, 0));
		m_skyMaterial->setVectorParameter(_LT("frustumRayTR"), Vector4(frustumRayTR, 0));
		m_skyMaterial->setVectorParameter(_LT("frustumRayBL"), Vector4(frustumRayBL, 0));


		static const float EARTH_RADIUS = 6370997.0f;
		static const float EARTH_ATMOSPHERE_RADIUS = EARTH_RADIUS * 1.025f;

		Vector3 cameraPos = Vector3(0, EARTH_RADIUS/*997*/, 0);// = cam->getTransform()->position.Get();
											   //cameraPos.normalize();
		//Vector3 cameraPos = Vector3(0, 0, 10);
		//Vector3 lightPos = 1.0f * Vector3::normalize(1, -0, -1);//sunDirection.normalized();
		//Vector3 lightPos = Vector3::normalize(Vector3(0.3, -0.1, 1));
		Vector3 lightPos = Vector3::normalize(Vector3(0, 1, 0));
		//Vector3 lightPos = Vector3::normalize(Vector3(0, -0.15, 1));

		float fCameraHeight = cameraPos.getLength();
		float fCameraHeight2 = fCameraHeight * fCameraHeight;

		const float red = 0.680f;
		const float green = 0.550f;
		const float blue = 0.440f;
		const auto invWav = [](float waveLength) { return 1.0f / pow(waveLength, 4.0f); };

		Vector3 invWavelength = Vector3(invWav(red), invWav(green), invWav(blue));



		float fInnerRadius = EARTH_RADIUS;
		float fInnerRadius2 = fInnerRadius * fInnerRadius;

		float fOuterRadius = EARTH_ATMOSPHERE_RADIUS;
		float fOuterRadius2 = fOuterRadius * fOuterRadius;

		float fScale = 1.0f / (fOuterRadius - fInnerRadius);

		const float Kr = 0.0025f -0.00015f;//static_cast<float>(gui.slider(L"Kr").value);
		const float Km = 0.0010f +0.0015f;// static_cast<float>(gui.slider(L"Km").value);
		const float Esun = 1300.0f;

		float fKrESun = Kr*Esun;
		float fKmESun = Km*Esun;
		float fKr4PI = Kr*4.0f* Math::PI;
		float fKm4PI = Km*4.0f*Math::PI;

		const float rayleighScaleDepth = 0.25f;
		float fScaleDepth = rayleighScaleDepth;
		float fScaleOverScaleDepth = (1.0f / (fOuterRadius - fInnerRadius)) / rayleighScaleDepth;
		float g = -0.999f;
		float exposure = 0.05 + 0.03;// static_cast<float>(gui.slider(L"Exposure").value);


		m_skyMaterial->setVectorParameter(_LT("v3CameraPos"), Vector4(cameraPos, 0));
		m_skyMaterial->setFloatParameter(_LT("fCameraHeight"), fCameraHeight);
		m_skyMaterial->setVectorParameter(_LT("v3LightPos"), Vector4(lightPos, 0));
		m_skyMaterial->setFloatParameter(_LT("fCameraHeight2"), fCameraHeight2);
		m_skyMaterial->setVectorParameter(_LT("v3InvWavelength"), Vector4(invWavelength, 0));
		m_skyMaterial->setFloatParameter(_LT("fScale"), fScale);
		m_skyMaterial->setFloatParameter(_LT("fOuterRadius"), fOuterRadius);
		m_skyMaterial->setFloatParameter(_LT("fOuterRadius2"), fOuterRadius2);
		m_skyMaterial->setFloatParameter(_LT("fInnerRadius"), fInnerRadius);
		m_skyMaterial->setFloatParameter(_LT("fInnerRadius2"), fInnerRadius2);
		m_skyMaterial->setFloatParameter(_LT("fKrESun"), fKrESun);
		m_skyMaterial->setFloatParameter(_LT("fKmESun"), fKmESun);
		m_skyMaterial->setFloatParameter(_LT("fKr4PI"), fKr4PI);
		m_skyMaterial->setFloatParameter(_LT("fKm4PI"), fKm4PI);
		m_skyMaterial->setFloatParameter(_LT("fScaleDepth"), fScaleDepth);
		m_skyMaterial->setFloatParameter(_LT("fScaleOverScaleDepth"), fScaleOverScaleDepth);
		m_skyMaterial->setFloatParameter(_LT("g"), g);
		m_skyMaterial->setFloatParameter(_LT("exposure"), exposure);
		/*

		float3 v3CameraPos;		// The camera's current position
		float fCameraHeight;	// The camera's current height

		float3 v3LightPos;		// The direction vector to the light source
		float fCameraHeight2;	// fCameraHeight^2

		float3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
		float fScale;			// 1 / (fOuterRadius - fInnerRadius)

		float fOuterRadius;		// The outer (atmosphere) radius
		float fOuterRadius2;	// fOuterRadius^2
		float fInnerRadius;		// The inner (planetary) radius
		float fInnerRadius2;	// fInnerRadius^2

		float fKrESun;			// Kr * ESun
		float fKmESun;			// Km * ESun
		float fKr4PI;			// Kr * 4 * PI
		float fKm4PI;			// Km * 4 * PI

		float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
		float fScaleOverScaleDepth;	// fScale / fScaleDepth
		float g;
		float exposure;
		*/

		renderer->setDepthTestEnabled(false);
		renderer->setDepthWriteEnabled(false);
		renderer->blit(nullptr, nullptr, m_skyMaterial);

		//renderer->setCullingMode(CullingMode::None);
		//Matrix tt;
		////tt.scale(-1, -1, -1);
		//renderer->drawSphere(100, 8, 8, Color::White, tt, m_skyMaterial);
		//renderer->setCullingMode(CullingMode::Back);
	}



}



//==============================================================================
// MirrorComponent
//==============================================================================
//------------------------------------------------------------------------------
MirrorComponent::MirrorComponent()
{
}

//------------------------------------------------------------------------------
MirrorComponent::~MirrorComponent()
{
}

//------------------------------------------------------------------------------
void MirrorComponent::initialize()
{
	VisualComponent::initialize();

	// TODO: この辺の初期化は OnParentChanged() のようなコールバックで行う。
	// そうすれば、アタッチされた任意のワールドへ追加できる。
	m_offscreen = newObject<OffscreenWorldSubRenderView>();

	// TODO: Remove
	detail::EngineDomain::getDefaultWorld3D()->addOffscreenWorldView(m_offscreen);
	m_offscreen->hideVisual(this);

	m_material = newObject<CommonMaterial>();
	//m_material->setMaterialTexture(Texture2D::getBlackTexture());
	//m_material->setMaterialTexture(Texture2D::getWhiteTexture());
	//m_material->setShader(Shader::getBuiltinShader(BuiltinShader::Sprite));
	auto shader = ln::Shader::create(_LT("D:/Proj/LN/HC1/External/Lumino/Source/LuminoEngine/Source/Scene/Resource/Mirror.fx"));
	m_material->setShader(shader);

	//auto tex = ln::Texture2D::create(_LT("C:/Proj/LN/HC1/Assets/Data/waterbump.png");
	//m_material->setTextureParameter(_LT(_LT("xWaterBumpMap"), tex);

}
float g_time = 0;
//------------------------------------------------------------------------------
void MirrorComponent::onRender2(RenderingContext* renderer)
{
	g_time += 0.001;
	m_material->setMaterialTexture(m_offscreen->getRenderTarget());
	m_material->setVectorParameter(_LT("xCamPos"), Vector4(renderer->getRenderView()->m_cameraInfo.viewPosition,/*  getCurrentCamera()->getTransform()->position.get(),*/ 1.0));
	m_material->setFloatParameter(_LT("time"), g_time);

	// TODO: 法泉が入っていない？
	renderer->drawSquare(20, 20, 1, 1, Color::White, Matrix::Identity, m_material);
}


LN_NAMESPACE_END
