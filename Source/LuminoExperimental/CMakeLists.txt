cmake_minimum_required (VERSION 3.0.0)
project (LuminoExperimental)


#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
#------------------------------
# Source files.
set(LN_SOURCES_PCH
	Source/LuminoExperimental.PCH.cpp
	Source/LuminoExperimental.PCH.h
)

set(LN_SOURCES_Editor
	Include/LuminoExperimental/Editor/Editor.h
	Source/Editor/Editor.cpp
)
set(LN_SOURCES_Scene
	Include/LuminoExperimental/Scene/Mirror.h
	Source/Scene/Mirror.cpp
)
set(LN_SOURCES_UI
	Include/LuminoExperimental/UI/UIDiagnosticsWindow.h
	Source/UI/UIDiagnosticsWindow.cpp
)

#--------------------------------------
# Grouping.
source_group("PCH"				FILES ${LN_SOURCES_PCH})
source_group("Editor"			FILES ${LN_SOURCES_Editor})
source_group("Scene"			FILES ${LN_SOURCES_Scene})
source_group("UI"				FILES ${LN_SOURCES_UI})

set(LN_SOURCES
	Include/LuminoExperimental.h
	${LN_SOURCES_PCH}
	${LN_SOURCES_Editor}
	${LN_SOURCES_Scene}
	${LN_SOURCES_UI}
)

#------------------------------------------------------------------------------
# Apply options
#------------------------------------------------------------------------------
if (WIN32)

	add_definitions("/EHsc")	# for VS2015 error

elseif (APPLE)
	set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

elseif (UNIX)

endif()

set(LN_INCLUDE_DIRECTORIES
	${PROJECT_SOURCE_DIR}/Include
	${PROJECT_SOURCE_DIR}/../LuminoCore/Include
	${PROJECT_SOURCE_DIR}/../LuminoEngine/Include
)
include_directories(${LN_INCLUDE_DIRECTORIES} ${LN_PLATFORM_INCLUDE_DIRS})

#------------------------------------------------------------------------------
# Output
#------------------------------------------------------------------------------
add_library(LuminoExperimental STATIC ${LN_SOURCES})
ln_add_common_property(LuminoExperimental)
target_include_directories(LuminoExperimental PUBLIC ${LN_DEPENDENCIES_INCLUDES})
ln_add_pch(LuminoExperimental "Source/LuminoExperimental.PCH.h" "Source/LuminoExperimental.PCH.cpp")

