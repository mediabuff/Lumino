
set(LN_SOURCES_SampleBrowser
	Sample_Lumino_Basic.cpp
	Sample_Graphics2D_Sprite2D.cpp
	Sample_Input_VirtualController.cpp
	Sample_Support_DebugAndTestSupport.cpp
	SampleBrowser.cpp
	SampleBrowser.PCH.cpp
	SampleBrowser.PCH.h
)

include_directories(
	"${PROJECT_SOURCE_DIR}/Source/LuminoCore/Include"
	"${PROJECT_SOURCE_DIR}/Source/LuminoEngine/Include"
)

add_executable(SampleBrowser ${LN_SOURCES_SampleBrowser})
target_link_libraries(SampleBrowser LuminoEngine LuminoLaunch)
add_dependencies(SampleBrowser LuminoEngine LuminoLaunch)
ln_add_pch(SampleBrowser "SampleBrowser.PCH.h" "SampleBrowser.PCH.cpp")

