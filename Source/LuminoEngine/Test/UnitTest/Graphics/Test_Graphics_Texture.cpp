#include <TestConfig.h>
#if 0
#include "../../../../src/Graphics/GraphicsManager.h"

class Test_Graphics_Texture : public ::testing::Test
{
protected:
	
	virtual void SetUp() 
	{
	}
	virtual void TearDown() 
	{
	}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Graphics_Texture, SetSubData)
{
	Ref<RawFont> font(RawFont::CreateBuiltInBitmapFontInternal(detail::GraphicsManager::GetInstance()->GetFontManager(), 7));
	FontGlyphBitmap* fb = font->LookupGlyphBitmap('S', 0);

	auto texture = Texture2D::create(32, 32);
	//texture->blt(0, 0, fb->GlyphBitmap);
	texture->Blt(28, 0, fb->GlyphBitmap);

	auto sprite = Sprite2DComponent::create(texture);



	Engine::UpdateFrame();

	TestEnv::SaveScreenShot(LN_TEMPFILE("test.png"));
}

#endif

