
/*
	DecreaseButton �� IncreaseButton �� WPF �ł�
	<Track>
		<Track.DecreaseButton>
			<RepertButton />
		</Track.DecreaseButton>
		<Track.IncreaseButton>
			<RepertButton />
		</Track.IncreaseButton>
	</Track>
	�̂悤�Ƀv���p�e�B�ɒ��ړ���Ă���B

	������������
	PART_DecreaseButton �̂悤�Ȗ��O�����Č������Ă��ǂ���������Ȃ��B
*/

#include "../../Internal.h"
#include <Lumino/GUI/GUIManager.h>
#include <Lumino/GUI/ControlTemplate.h>
#include <Lumino/GUI/Controls/Track.h>

namespace Lumino
{
namespace GUI
{

//=============================================================================
// Track
//=============================================================================
LN_CORE_OBJECT_TYPE_INFO_IMPL(Track);
LN_UI_ELEMENT_SUBCLASS_IMPL(Track);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Track::Track(GUIManager* manager)
	: UIElement(manager)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Track::~Track()
{
}

} // namespace GUI
} // namespace Lumino
