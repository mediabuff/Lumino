﻿#include <TestConfig.h>
#include <Lumino/Scene/StaticMesh.h>

class Test_Graphics_MeshResource : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_Graphics_MeshResource, addSquare)
{
	// 自己照明
	auto material = CommonMaterial::create();
	material->setBuiltinColorParameter(CommonMaterial::EmissiveParameter, Color::White);

	// <Test> 四角形1つ
	{
		auto mesh = MeshResource::create();
		mesh->addSquare(
			Vertex{ Vector3(-2, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Red },
			Vertex{ Vector3(-2, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Green },
			Vertex{ Vector3(2, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Blue },
			Vertex{ Vector3(2, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::White });

		LN_TEST_BEGIN_FRAME;
		Engine::renderFrame();
		Engine::getWorld3D()->getRenderer()->drawMesh(mesh, 0, material);
		LN_TEST_END_FRAME;
		ASSERT_TRUE(TestEnv::CheckScreenShot(LN_LOCALFILE("Result/Test_Graphics_MeshResource.AddSquare1.png"), 95));
	}

	// <Test> 描画しつつ追加
	{
		auto mesh = MeshResource::create();

		// 1回目
		mesh->addSquare(
			Vertex{ Vector3(-2, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Red },
			Vertex{ Vector3(-2, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Green },
			Vertex{ Vector3(0, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Blue },
			Vertex{ Vector3(0, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::White });
		LN_TEST_BEGIN_FRAME;
		Engine::renderFrame();
		Engine::getWorld3D()->getRenderer()->drawMesh(mesh, 0, material);
		LN_TEST_END_FRAME;
		ASSERT_TRUE(TestEnv::CheckScreenShot(LN_LOCALFILE("Result/Test_Graphics_MeshResource.AddSquare2.png"), 95));

		// 2回目
		mesh->addSquare(
			Vertex{ Vector3(0, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Red },
			Vertex{ Vector3(0, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Green },
			Vertex{ Vector3(2, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Blue },
			Vertex{ Vector3(2, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::White });
		LN_TEST_BEGIN_FRAME;
		Engine::renderFrame();
		Engine::getWorld3D()->getRenderer()->drawMesh(mesh, 0, material);
		LN_TEST_END_FRAME;
		ASSERT_TRUE(TestEnv::CheckScreenShot(LN_LOCALFILE("Result/Test_Graphics_MeshResource.AddSquare3.png"), 95));
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Graphics_MeshResource, clear)
{
	// 自己照明
	auto material = CommonMaterial::create();
	material->setBuiltinColorParameter(CommonMaterial::AmbientParameter, Color::White);

	// <Test> 描画しつつ追加
	{
		auto mesh = MeshResource::create();

		// 1回目
		mesh->addSquare(
			Vertex{ Vector3(-2, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Red },
			Vertex{ Vector3(-2, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Green },
			Vertex{ Vector3(0, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Blue },
			Vertex{ Vector3(0, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::White });
		LN_TEST_BEGIN_FRAME;
		Engine::renderFrame();
		Engine::getWorld3D()->getRenderer()->drawMesh(mesh, 0, material);
		LN_TEST_END_FRAME;
		ASSERT_TRUE(TestEnv::CheckScreenShot(LN_LOCALFILE("Result/Test_Graphics_MeshResource.Clear1.png"), 95));

		// 2回目
		mesh->clear();		// clear
		mesh->addSquare(
			Vertex{ Vector3(0, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Red },
			Vertex{ Vector3(0, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Green },
			Vertex{ Vector3(2, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Blue },
			Vertex{ Vector3(2, 2, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::White });
		LN_TEST_BEGIN_FRAME;
		Engine::renderFrame();
		Engine::getWorld3D()->getRenderer()->drawMesh(mesh, 0, material);
		LN_TEST_END_FRAME;
		ASSERT_TRUE(TestEnv::CheckScreenShot(LN_LOCALFILE("Result/Test_Graphics_MeshResource.Clear2.png"), 95));
	}
}

//------------------------------------------------------------------------------
//TEST_F(Test_Graphics_MeshResource, addLine)
//{
//	// 自己照明
//	auto material = CommonMaterial::create();
//	material->setBuiltinColorParameter(CommonMaterial::EmissiveParameter, Color::White);
//
//	// <Test> addLine
//	{
//		auto mesh = MeshResource::create();
//		mesh->addSections(1);
//		mesh->getSection(0)->primitiveType = PrimitiveType_LineList;
//		mesh->addLine(
//			Vertex{ Vector3(-2, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Red },
//			Vertex{ Vector3(2, 0, 0), Vector2(0, 0), Vector3(0, 0, 1), Color::Green });
//
//		LN_TEST_BEGIN_FRAME;
//		Engine::renderFrame();
//		Engine::getWorld3D()->getRenderer()->drawMesh(mesh, 0, material);
//		LN_TEST_END_FRAME;
//		ASSERT_TRUE(TestEnv::CheckScreenShot(LN_LOCALFILE("Result/Test_Graphics_MeshResource.AddLine1.png"), 95));
//	}
//}

//------------------------------------------------------------------------------
//TEST_F(Test_Graphics_MeshResource, Geometry)
//{
//
//	ScopedCameraPosition cp(5, 5, -5);
//	{
//		auto mesh = TeapotMesh::create();
//
//		Engine::update();
//
//		ASSERT_TRUE(TestEnv::CheckScreenShot(LN_LOCALFILE("Result/Test_Graphics_MeshResource.Geometry1.png"), 95, true));
//	}
//}
//
