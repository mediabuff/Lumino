#pragma once
#include "Common.h"

LN_NAMESPACE_BEGIN

/**
	@brief	�����_�����O�Ɋւ���@�\��񋟂��܂��B
*/
class Graphics
{
public:

	/**
		@brief		�����_�����O���J�n���܂��B
		@details	Application::UpdateFrame() ���g�p����ꍇ�A���̊֐����Ăяo���K�v�͂���܂���B
	*/
	//void BeginRendering();
	
	/**
		@brief		�����_�����O���I�����܂��B
	*/
	//void EndRendering();

	static void ChangeDirectX9Device(void* id3d9device);
};

LN_NAMESPACE_END