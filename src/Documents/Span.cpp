﻿
#include "../Internal.h"
#include <Lumino/Documents/Span.h>

namespace Lumino
{
namespace Documents
{

//=============================================================================
// Span
//=============================================================================
LN_CORE_OBJECT_TYPE_INFO_IMPL(Span, Inline);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Span::Span(DocumentsManager* manager)
	: Inline(manager)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Span::~Span()
{
}

} // namespace Documents
} // namespace Lumino

