﻿
#define LN_INTERNAL_ACCESS				public
#define LN_PROTECTED_INTERNAL_ACCESS	public
#include <LuminoEngine.h>
#include <Lumino/Testing/TestHelper.h>
#include <Lumino/Scene/OffscreenWorldView.h>

using namespace ln;

void UIControlsGallery()
{
	Engine::initialize();


	Engine::getWorld3D()->SetVisibleGridPlane(true);

	auto cb = RefPtr<CylinderMouseMoveCameraBehavior>::makeRef();
	CameraComponent::GetMain3DCamera()->SetCameraBehavior(cb);


	//auto* uiRoot = Engine::getMainWindow();
	auto* uiRoot = Engine::getDefaultUILayer()->GetLayoutView();


	//auto listBox1 = UIListBox::create();
	//listBox1->SetWidth(200);
	//uiRoot->AddChild(listBox1);

	//auto item = listBox1->AddTextItem(_T("スプライト"));
	//auto button1 = UIButton::create(_T(">"), 20, 20);
	////button1->setPosition(PointF(2, 0));
	//button1->margin = ThicknessF(2);
	//button1->SetAnchor(AlignmentAnchor::RightOffsets | AlignmentAnchor::VCenter);
	//item->AddChild(button1);

	//listBox1->AddTextItem(_T("UI"));


	auto slider = UISlider::create(0.75, 0.5, 2.0);
	slider->setPosition(PointF(10, 10));
	slider->SetWidth(300);
	slider->SetHeight(16);
	//slider->SetOrientation(Orientation::Vertical);
	uiRoot->AddChild(slider);

	//auto sky = newObject<SkyComponent>();
	//sky->setDepthTestEnabled(false);
	//sky->setDepthWriteEnabled(false);
	//auto skyObj = ln::newObject<ln::WorldObject3D>();
	//skyObj->AddComponent(sky);
	//skyObj->setPosition(10, 0, 0);

	////auto mirror = newObject<MirrorComponent>();
	////mirror->setDepthTestEnabled(false);
	////mirror->setDepthWriteEnabled(false);
	////auto mirrorObj = ln::newObject<ln::WorldObject3D>();
	////mirrorObj->AddComponent(mirror);
	////mirrorObj->setPosition(10, 0, 0);



	////auto meshModel = ln::StaticMeshModel::
	//auto mesh = ln::StaticMeshComponent::CreateSphere(1);
	////mesh->GetStaticMeshModel()->
	//auto material = mesh->GetStaticMeshModel()->GetMaterial(0);
	//material->setMaterialTexture(ln::Assets::loadTexture(_T("D:/GameProjects/Chronicles/sky/incskies_024_png8/incskies_024_8k_.png")));
	////material->setMaterialTexture(Texture2D::getBlackTexture());
	//auto m_map = ln::newObject<ln::WorldObject3D>();
	//m_map->AddComponent(mesh);
	//mesh->setBlendMode(BlendMode::Normal);
	//mesh->setShader(Shader::getBuiltinShader(BuiltinShader::Sprite));
	//m_map->SetScale(-1.5, 1.5, 1.5);
	////m_map->SetScale(-10, 10, 10);
	//m_map->setPosition(0, 2, 0);
	////mesh->SetVisible(false);


	//CameraComponent::GetMain3DCamera()->SetReflectionPlane(Plane(Vector3::UnitY));




	//auto shader = Shader::create(StringRef(_T("C:/Proj/LuminoStudio/external/Lumino/Source/LuminoEngine/Test/Sandbox/SSBasic2D.fx")), true);
	//auto rect = Rectangle::create(RectF(0, 0, 3, 1));
	//rect->setShader(shader);
	//rect->SetAngles(Math::PI/2, 0, 0);

	//auto grid1 = UIGridLayout::create(1, 1);
	//win1->AddChild(grid1);


	//auto win1 = UIWindow::create();
	//win1->SetWidth(200);
	//win1->SetHeight(100);
	////win1->SetBackground(Brush::DimGray);
	//mainWindow->AddChild(win1);


	//auto grid1 = UIGridLayout::create(1, 1);
	//win1->AddChild(grid1);
	//auto stack1 = UIStackPanel::create();
	//grid1->AddChild(stack1);

	//auto text1 = UITextBlock::create();
	//text1->SetText(_T("text"));
	//stack1->AddChild(text1);

	auto ps1 = TransitionPostEffect::create();
	Engine::getDefault3DLayer()->AddPostEffect(ps1);
	
	//auto box1 = StaticMeshComponent::create(LN_LOCALFILE("Assets/cube.mqo"));
	//auto box1 = StaticMeshComponent::create(LN_LOCALFILE("Assets/cylinder2.mqo"));
	//auto box1 = StaticMeshComponent::create(LN_LOCALFILE("Assets/Plant1.mqo"));
	//auto box1 = StaticMeshComponent::create(_T("D:/Documents/Modeling/test4.mqo"));
	//auto mesh1 = newObject<WorldObject3D>();
	//mesh1->AddComponent(box1);
	
#if 0
	auto font = Font::getDefault();
	font->SetSize(50);

	detail::Filled filled;
	filled.initialize();
	filled.setTessellationSteps(3);
	filled.DecomposeOutlineVertices(static_cast<detail::FreeTypeFont*>(font->ResolveRawFont()), 'A');
	filled.Tessellate();
	filled.MakeEdgeStroke();

	auto meshRes = MeshResource::create();
	meshRes->ResizeVertexBuffer(filled.m_vertexList.GetCount());
	meshRes->AddSections(1);
	meshRes->GetSection(0)->MaterialIndex = 0;
	meshRes->GetSection(0)->StartIndex = 0;
	meshRes->GetSection(0)->PrimitiveNum = filled.m_triangleIndexList.GetCount() / 3;
	meshRes->GetSection(0)->primitiveType = PrimitiveType_TriangleList;
	for (int i = 0; i < filled.m_vertexList.GetCount(); i++)
	{
		meshRes->SetPosition(i, Vector3(filled.m_vertexList[i].pos, 0.0f));
		meshRes->SetColor(i, Color(0, 0, 0, filled.m_vertexList[i].alpha));
		//filled.m_vertexList[i].print();
	}
	meshRes->ResizeIndexBuffer(filled.m_triangleIndexList.GetCount(), IndexBufferFormat_UInt16);
	for (int i = 0; i < filled.m_triangleIndexList.GetCount(); i++)
	{
		meshRes->SetIndex(i, filled.m_triangleIndexList[i]);
	}

	auto mesh1 = NewObject<StaticMeshModel>(meshRes->m_manager, meshRes);
	auto mat1 = DiffuseMaterial::create();
	mat1->SetBlendMode(BlendMode::Alpha);
	mesh1->AddMaterial(mat1);

	auto mesh2 = StaticMeshComponent::create(mesh1);
#endif



#if 0
	auto* mainWindow = Engine::GetMainWindow();
	mainWindow->SetLayoutPanel(UIStackPanel::create());

	auto grid1 = UIGridLayout::create(1, 1);
	mainWindow->AddChild(grid1);

	// Button
	{
		auto stack1 = UIStackPanel::create();

		auto text1 = UITextBlock::create();
		text1->SetText(_T("Button"));
		stack1->AddChild(text1);

		auto button1 = UIButton::create();
		//button1->SetWidth(80);
		//button1->SetHeight(24);
		button1->SetText(_T("Button2"));
		stack1->AddChild(button1);

		auto button2 = UIToggleButton::create();
		button2->SetText(_T("Button2"));
		stack1->AddChild(button2);

		auto slider1 = UISlider::create();
		slider1->SetHeight(32);
		stack1->AddChild(slider1);

		auto field1 = UITextField::create();
		field1->SetWidth(200);
		field1->SetHeight(32);
		field1->SetText(_T("field"));
		field1->SetBackground(Brush::Gray);
		stack1->AddChild(field1);

		grid1->AddChild(stack1);

		// TODO: テキストサイズに合わせる(サイズ省略)
	}
#endif

	//auto text = UITextBlock::create();//TextBlock2DComponent::create(_T("Hello, world!"));
	//text->SetText(_T("Hello"));
	//text->foreground = Brush::Blue;
	//mainWindow->AddChild(text);

	//auto textbox1 = UITextBox::create();
	//textbox1->SetWidth(300);
	//textbox1->SetHeight(32);
	//textbox1->SetBackground(Brush::Gray);
	//textbox1->SetText(_T("Hello, world!"));
	//textbox1->foreground = Brush::Blue;
	//mainWindow->AddChild(textbox1);


	//auto comboBox = tr::UIComboBox::create();
	//comboBox->SetWidth(300);
	//comboBox->SetHeight(32);
	//comboBox->SetBackground(Brush::Gray);
	//mainWindow->AddChild(comboBox);


	

#if 0
	auto* mainWindow = Engine::GetMainWindow();
	mainWindow->SetLayoutPanel(UIStackPanel::create());

	auto button1 = UIButton::create();
	button1->SetSize(Size(80, 32));
	//button1->SetText(_T("Button1"));
	mainWindow->AddChild(button1);

	auto button2 = UIButton::create();
	button2->SetSize(Size(80, 32));
	mainWindow->AddChild(button2);
	button2->Focus();	// TODO: AddChild した後でないとフォーカス持てない。これでいいの？

	// TODO: 描画時、Brush が変わってない？
	auto image3 = UIImage::create(_T("C:/Proj/LuminoStudio/external/Lumino/Source/LuminoEngine/Test/UnitTest/Graphics/TestData/img2.png"));
	image3->SetSize(Size(20, 20));
	button2->AddChild(image3);


	auto image1 = UIImage::create(_T("C:/Proj/LuminoStudio/external/Lumino/Source/LuminoEngine/Test/UnitTest/Graphics/TestData/img2.png"));
	image1->SetSize(Size(64, 64));
	mainWindow->AddChild(image1);

	auto thumb1 = UIThumb::create();
	thumb1->SetSize(Size(80, 32));
	mainWindow->AddChild(thumb1);

	auto scrollBar1 = UIScrollBar::create();
	//scrollBar1->setSize(Size(Math::NaN, 32));
	scrollBar1->SetMaximum(10);
	scrollBar1->SetValue(3);
	scrollBar1->SetViewportSize(2);
	mainWindow->AddChild(scrollBar1);

	auto scrollViewer1 = UIScrollViewer::create();
	scrollViewer1->SetSize(Size(100, 100));
	scrollViewer1->SetBackground(Brush::Red);
	mainWindow->AddChild(scrollViewer1);

	auto image2 = UIImage::create(_T("C:/Proj/LuminoStudio/external/Lumino/Source/LuminoEngine/Test/UnitTest/Graphics/TestData/img2.png"));
	image2->SetSize(Size(200, 200));
	scrollViewer1->AddChild(image2);

#endif

	float t = 0;
	while (!Engine::isEndRequested())
	{
		Engine::updateFrame();

		if (Input::IsTriggered(InputButtons::OK))
		{
			ps1->Transition(1, nullptr, 0);
		}


			Engine::renderFrame();

			//Engine::getDefaultSceneGraph3D()->getRenderer()->setShader(shader);
			//Engine::getDefaultSceneGraph3D()->getRenderer()->DrawScreenRectangle();
			
		//Engine::getDefaultSceneGraph3D()->GetDebugRenderer()->DrawLinePrimitive(
		//	Vector3(0, 0, 0), Color::Red,
		//	Vector3(5, 5, 5), Color::White);

			t += 0.016;
			//shader->findVariable(_T("time"))->SetFloat(t);

		Engine::presentFrame();

		printf("%f\n", slider->getValue());


		//printf("----------\n");
		//Engine::getMainWindow()->GetRenderDiagnostic()->print();
	}

	//while (Engine::update())
	//{
	//	//printf("----\n");
	//}

	Engine::terminate();

}



