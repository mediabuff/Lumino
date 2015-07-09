﻿
#pragma once

#include "../Internal.h"
#include "../../include/Lumino/Graphics/VertexBuffer.h"
#include "../../include/Lumino/Graphics/GraphicsManager.h"

namespace Lumino
{
namespace Graphics
{

//=============================================================================
// VertexBuffer
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VertexBuffer* VertexBuffer::Create(const VertexElement* vertexElements, int elementsCount, int vertexCount, const void* data, DeviceResourceUsage usage)
{
	return Create(Internal::Manager, vertexElements, elementsCount, vertexCount, data, usage);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VertexBuffer* VertexBuffer::Create(GraphicsManager* manager, const VertexElement* vertexElements, int elementsCount, int vertexCount, const void* data, DeviceResourceUsage usage)
{
	LN_THROW(manager != NULL, ArgumentException);
	RefPtr<Device::IVertexBuffer> deviceObj(
		manager->GetGraphicsDevice()->CreateVertexBuffer(vertexElements, elementsCount, vertexCount, data, usage));
	return LN_NEW VertexBuffer(deviceObj);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VertexBuffer::VertexBuffer(Device::IVertexBuffer* deviceObj)
	: m_deviceObj(deviceObj)
	, m_initialUpdate(true)
{
	LN_SAFE_ADDREF(m_deviceObj);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	LN_SAFE_RELEASE(m_deviceObj);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer* VertexBuffer::Lock()
{
	// まだ1度も SetVertexBufferCommand に入っていない場合は直接 Lock で書き換えできる
	if (m_initialUpdate) {
		m_lockedBuffer.Attach(m_deviceObj->Lock(), m_deviceObj->GetByteCount());
	}
	else {
		LN_THROW(0, NotImplementedException);
	}
	return &m_lockedBuffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void VertexBuffer::Unlock()
{
	if (m_initialUpdate) {
		m_deviceObj->Unlock();
	}
	else {
		LN_THROW(0, NotImplementedException);
	}
}

} // namespace Graphics
} // namespace Lumino
