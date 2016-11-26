﻿
#include <LuminoEngine.h>
#include <Lumino/Scene/MeshModelObject.h>
#include <Lumino/Scene/Light.h>
using namespace ln;

void Main()
{
	Engine::Initialize();

	Engine::GetDefaultSceneGraph3D()->VisibleGridPlane = true;

	auto cb = RefPtr<CylinderMouseMoveCameraBehavior>::MakeRef();
	Camera::GetMain3DCamera()->SetCameraBehavior(cb);


	//auto mesh = SkinnedMesh::Create(_T("D:/MMD/Materials/モデル/Appearance Miku/Appearance Miku_BDEF.pmx"));

	//auto tex = Texture2D::Create("D:/Proj/Volkoff/External/Lumino/test/UnitTest/Scene/TestData/Dice1.png");
	//auto sprite1 = Sprite3D::Create(1, 1, tex); sprite1->SetSrcRect(0, 0, 50, 50);
	//auto sprite2 = Sprite3D::Create(1, 1, tex); sprite1->SetSrcRect(0, 0, 50, 50);
	//auto sprite3 = Sprite3D::Create(1, 1, tex); sprite1->SetSrcRect(0, 0, 50, 50);
	//auto sprite4 = Sprite3D::Create(1, 1, tex); sprite1->SetSrcRect(0, 0, 50, 50);
	//auto sprite5 = Sprite3D::Create(1, 1, tex); sprite1->SetSrcRect(0, 0, 50, 50);
	//auto sprite6 = Sprite3D::Create(1, 1, tex); sprite1->SetSrcRect(0, 0, 50, 50);


	//auto tonePE = ToneImageEffect::Create();
	//tonePE->SetTone(ToneF(-0.1, -0.25, -0.5, 1.0));
	//CameraViewportLayer::GetDefault3D()->GetImageEffects()->Add(tonePE);

	auto blur = ScreenMotionBlurImageEffect::Create();
	blur->SetAmount(0.5f);
	blur->SetRadialScale(1.05f);
	CameraViewportLayer::GetDefault3D()->GetImageEffects()->Add(blur);

	//Engine::GetMainLight3D()->SetPosition();

	int map[5*5] =
	{
		1, 0, 0, 0, 1,
		1, 0, 0, 0, 1,
		1, 0, 0, 0, 1,
		1, 0, 0, 0, 1,
		1, 0, 0, 0, 1,
	};

	auto t = Assets::LoadTexture("D:/Proj/Volkoff/Volkoff/Data/Graphics/MapChip_1.png");

	auto m_tileset = TileSet::Create();
	m_tileset->SetImageSource(t);
	m_tileset->SetTileSize(SizeI(20, 20));


	auto tilemapModel = TileMapModel::Create();
	tilemapModel->SetTileSet(m_tileset);

	auto m_tileLayer = TileLayer::Create();
	m_tileLayer->Resize(5, 5);
	tilemapModel->GetLayers()->Add(m_tileLayer);

	auto mTilemap = TileMap::Create3D();
	mTilemap->SetTileMap(tilemapModel);
	mTilemap->SetPriority(1000);
	mTilemap->SetDepthWriteEnabled(false);

	for (int i = 0; i < 5 * 5; ++i)
	{
		m_tileLayer->SetTileId(i % 100, i / 100, map[i]);
	}

	while (Engine::Update())
	{
	}
}
