﻿#include "TestConfig.h"
#include <Lumino/UI/UIFrameWindow.h>
#include "../../../Source/EngineManager.h"
#include "../../../Source/Graphics/GraphicsManager.h"
#include "../../../Source/UI/UIManager.h"

//------------------------------------------------------------------------------
void TestEnv::SetUp()
{
}

//------------------------------------------------------------------------------
void TestEnv::TearDown()
{
}

//------------------------------------------------------------------------------
void TestEnv::saveScreenShot(const Char* filePath)
{
	EngineManager::Instance->getGraphicsManager()->getMainSwapChain()->getBackBuffer()->readSurface()->save(filePath);
}

//------------------------------------------------------------------------------
bool TestEnv::EqualsScreenShot(const Char* filePath, int passRate)
{
	bool r = TestEnv::EqualsBitmapFile(EngineManager::Instance->getGraphicsManager()->getMainSwapChain()->getBackBuffer()->readSurface(), filePath, passRate);
	return r;
}

//------------------------------------------------------------------------------
Color32 MixPixels(RawBitmap* bmp, int x, int y)
{
	const Color32& c = bmp->getPixel(x, y);
	int r = c.r; int g = c.g; int b = c.b; int a = c.a;
	int count = 1;

	if (y > 0) {
		if (x > 0) {
			const Color32& c = bmp->getPixel(x - 1, y - 1);
			r += c.r; g += c.g; b += c.b; a += c.a; ++count;
		}
		{
			const Color32& c = bmp->getPixel(x, y - 1);
			r += c.r; g += c.g; b += c.b; a += c.a; ++count;
		}
		if (x < bmp->getSize().width - 1) {
			const Color32& c = bmp->getPixel(x + 1, y - 1);
			r += c.r; g += c.g; b += c.b; a += c.a; ++count;
		}
	}
	{
		if (x > 0) {
			const Color32& c = bmp->getPixel(x - 1, y);
			r += c.r; g += c.g; b += c.b; a += c.a; ++count;
		}
		if (x < bmp->getSize().width - 1) {
			const Color32& c = bmp->getPixel(x + 1, y);
			r += c.r; g += c.g; b += c.b; a += c.a; ++count;
		}
	}
	if (y < bmp->getSize().height - 1) {
		if (x > 0) {
			const Color32& c = bmp->getPixel(x - 1, y + 1);
			r += c.r; g += c.g; b += c.b; a += c.a; ++count;
		}
		{
			const Color32& c = bmp->getPixel(x, y + 1);
			r += c.r; g += c.g; b += c.b; a += c.a; ++count;
		}
		if (x < bmp->getSize().width - 1) {
			const Color32& c = bmp->getPixel(x + 1, y + 1);
			r += c.r; g += c.g; b += c.b; a += c.a; ++count;
		}
	}

	return Color32(r / count, g / count, b / count, a / count);
}

bool TestEnv::EqualsBitmapFile(RawBitmap* bmp1, const Char* filePath, int passRate)
{
	RawBitmap bmp2(filePath);

	bool ignoreAlpha = true;

	int colorRange = 255 - (255 * passRate / 100);
	int pass = 0;
	int fail = 0;

	for (int y = 0; y < bmp1->getSize().height; ++y)
	{
		for (int x = 0; x < bmp1->getSize().width; ++x)
		{
			Color32 c1 = MixPixels(bmp1, x, y);
			Color32 c2 = MixPixels(&bmp2, x, y);
			if (abs(c1.r - c2.r) <= colorRange &&
				abs(c1.g - c2.g) <= colorRange &&
				abs(c1.b - c2.b) <= colorRange &&
				(ignoreAlpha || abs(c1.a - c2.a) <= colorRange))
			{
				++pass;
			}
			else
			{
				++fail;
			}
		}
	}

	int thr = ((bmp1->getSize().height * bmp1->getSize().width) * passRate / 100);
	return pass >= thr;
}

bool TestEnv::CheckScreenShot(const Char* filePath, int passRate, bool save)
{
	if (save)
	{
		saveScreenShot(filePath);
		return true;
	}
	else
	{
		return EqualsScreenShot(filePath, passRate);
	}
}
//------------------------------------------------------------------------------
void TestEnv::WaitRendering()
{
	Application::getMainWindow()->getSwapChain()->WaitForPresent();
}










//------------------------------------------------------------------------------
void EngineInitalize()
{
	EngineSettings::addAssetsDirectory(LN_LOCALFILE("../../../../Samples/Assets"));

	Engine::initialize();

	// テストしやすいように固定フレームレートにする
	Engine::setFrameUpdateMode(FrameUpdateMode::Fixed);


	RawFont::registerFontFile(LN_LOCALFILE("../../../../Tools/VLGothic/VL-Gothic-Regular.ttf"));
	RawFont::getDefaultFont()->setName(_LT("VL Gothic"));
	RawFont::getDefaultFont()->setSize(20);
	Font::getDefault()->setFamily(_LT("VL Gothic"));
	Font::getDefault()->setSize(20);

	// 背景はグレーにしておくと加算合成のテストとか、いろいろ都合がよい
	Engine::getDefault3DLayer()->setBackgroundColor(Color::Gray);

	Engine::getCamera3D()->getCameraComponent()->setCameraDirection(CameraDirection::lookAt);

	{
		auto buttonNormalBrush = TextureBrush::create(detail::UIManager::getInstance()->getDefaultSkinTexture());
		buttonNormalBrush->setSourceRect(0, 0, 32, 32);
		buttonNormalBrush->getBorderThickness(8, 8, 8, 8);
		buttonNormalBrush->getImageDrawMode(BrushImageDrawMode::BoxFrame);
		buttonNormalBrush->setWrapMode(BrushWrapMode::Stretch);

		auto* res = detail::UIManager::getInstance()->getDefaultStyleTable();
		auto* style = res->getStyle(_LT("UIButton"));
		// base
		{
			auto* props = style->getPropertyTable();
			props->background = Ref<Brush>::staticCast(buttonNormalBrush);
			props->borderThickness = Thickness(0);
		}
	}
}

//------------------------------------------------------------------------------
GTEST_API_ int main(int argc, char **argv)
{
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	setlocale(LC_ALL, "");

#if 1	// 部分的にテストを実行したりする
	char* testArgs[] = {
		argv[0],
		"--gtest_filter=Test_Rendering_Light.*",
		"--gtest_break_on_failure",
	};
	argc = sizeof(testArgs) / sizeof(char*);
	testing::InitGoogleTest(&argc, (char**)testArgs);
#else
	testing::InitGoogleTest(&argc, argv);
#endif
	::testing::AddGlobalTestEnvironment(new TestEnv());

	{
		Logger::initialize(_LT("test_log.txt"));

		int scale = 1;
		EngineSettings::setMainWindowSize(SizeI(160 * scale, 120 * scale));
		EngineSettings::setMainBackBufferSize(SizeI(160 * scale, 120 * scale));
		EngineSettings::setGraphicsRenderingType(GraphicsRenderingType::Threaded);//GraphicsRenderingType::Immediate);//
		detail::EngineSettings::instance.defaultSkinFilePath = LN_LOCALFILE("UI/Data/Skin.png");
	}
	
	{
		EngineSettings::setGraphicsAPI(GraphicsAPI::DirectX9);

		EngineInitalize();
		int r = RUN_ALL_TESTS();
		Engine::terminate();
		if (r != 0) return r;
	}
	//{
	//	EngineSettings::SetGraphicsAPI(GraphicsAPI::OpenGL);

	//	EngineInitalize();
	//	int r = RUN_ALL_TESTS();
	//	Engine::terminate();
	//	if (r != 0) return r;
	//}

	//RUN_ALL_TESTS();
	//return RUN_ALL_TESTS();
	return 0;
}
