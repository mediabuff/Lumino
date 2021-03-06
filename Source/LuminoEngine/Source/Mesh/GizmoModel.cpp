﻿
#include "../Internal.h"
#include <Lumino/Graphics/Material.h>
#include <Lumino/Rendering/Rendering.h>
#include <Lumino/Mesh/GizmoModel.h>
#include "../Graphics/GraphicsManager.h"

LN_NAMESPACE_BEGIN
namespace tr {

const float GizmoModel::CenterBoxSize = 0.15f;
const float GizmoModel::BarRadius = 0.05f;
const float GizmoModel::BarLendth = 1.0f;
const float GizmoModel::OnPlaneBoxSize = 0.4f;
const float GizmoModel::RotationRingInner = 0.9f;
const float GizmoModel::RotationRingOuter = 1.0f;
const float GizmoModel::RotationViewZRingInner = 1.0f;
const float GizmoModel::RotationViewZRingOuter = 1.1f;
const float GizmoModel::BaseOpacity = 0.5f;

//------------------------------------------------------------------------------
GizmoModelPtr GizmoModel::create()
{
	auto ptr = GizmoModelPtr::makeRef();
	ptr->initialize(ln::detail::GraphicsManager::getInstance());
	return ptr;
}

//------------------------------------------------------------------------------
GizmoModel::GizmoModel()
	: m_gizmoType(GizmoType::Translation)//GizmoType::Scaling)//GizmoType::Rotation)//
	, m_displayScale(1.25f)
	, m_operationType(OperationType::None)
	, m_dragging(false)
{
}

//------------------------------------------------------------------------------
GizmoModel::~GizmoModel()
{
}

//------------------------------------------------------------------------------
void GizmoModel::initialize(ln::detail::GraphicsManager* manager)
{
	m_tmat = newObject<CommonMaterial>();
	m_tmat->setBlendMode(BlendMode::Alpha);
	m_tmat->setCullingMode(CullingMode::None);
}

//------------------------------------------------------------------------------
void GizmoModel::setGizmoType(GizmoType type)
{
	m_gizmoType = type;
}

//------------------------------------------------------------------------------
void GizmoModel::setup(const Matrix& parentSpaceTransform, const AttitudeTransform& targetInitialTransform)
{
	m_targetInitialTransform = targetInitialTransform;
	Matrix targetWorld = parentSpaceTransform * Matrix(targetInitialTransform);
	m_gizmoTransform = targetWorld;
	m_gizmoInitialTransform = m_gizmoTransform;
}

////------------------------------------------------------------------------------
//void GizmoModel::SetTargetTransform(const Matrix& transform)
//{
//	m_targetTransform = transform;
//}

//------------------------------------------------------------------------------
const AttitudeTransform& GizmoModel::GetTargetTransform() const
{
	return m_targetTransform;
}

//------------------------------------------------------------------------------
void GizmoModel::setViewInfo(const Vector3& viewPosition, const Matrix& view, const Matrix& proj, const SizeI& viewPixelSize)
{
	m_viewPosition = viewPosition;
	m_view = view;
	m_proj = proj;
	m_viewPixelSize = viewPixelSize;
	
	makeScreenFactor();
}

//------------------------------------------------------------------------------
void GizmoModel::setDisplayScale(float scale)
{
	m_displayScale = scale;
}

//------------------------------------------------------------------------------
bool GizmoModel::injectMouseDown(int x, int y)
{
	if (!m_dragging)
	{
		m_operationType = getDirectionOperationType(x, y, &m_draggingLocalPlane);
		if (m_operationType != OperationType::None)
		{
			m_draggingLocalPlane.intersects(makeLocalRay(x, y), &m_draggingStartLocalPosition);
			m_draggingStartGizmoTransform = m_gizmoTransform;
			m_draggingStartViewPixelPoint.set(x, y);
			m_dragging = true;
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
bool GizmoModel::injectMouseMove(int x, int y)
{
	if (m_dragging)
	{
		Vector3 localOffaet;
		m_draggingLocalPlane.intersects(makeLocalRay(x, y), &localOffaet);
		localOffaet -= m_draggingStartLocalPosition;

		localOffaet *= m_screenFactor;

		switch (m_gizmoType)
		{
			case ln::tr::GizmoType::Translation:
			{
				switch (m_operationType)
				{
				case ln::tr::GizmoModel::OperationType::X:
					localOffaet = Vector3(localOffaet.x, 0, 0);
					break;
				case ln::tr::GizmoModel::OperationType::Y:
					localOffaet = Vector3(0, localOffaet.y, 0);
					break;
				case ln::tr::GizmoModel::OperationType::Z:
					localOffaet = Vector3(0, 0, localOffaet.z);
					break;
				case ln::tr::GizmoModel::OperationType::XY:
					localOffaet = Vector3(localOffaet.x, localOffaet.y, 0);
					break;
				case ln::tr::GizmoModel::OperationType::XZ:
					localOffaet = Vector3(localOffaet.x, 0, localOffaet.z);
					break;
				case ln::tr::GizmoModel::OperationType::YZ:
					localOffaet = Vector3(0, localOffaet.y, localOffaet.z);
					break;
				case ln::tr::GizmoModel::OperationType::XYZ:
					break;
				}

				m_gizmoTransform = m_draggingStartGizmoTransform * Matrix::makeTranslation(localOffaet);
				m_targetTransform.translation = m_targetInitialTransform.translation + localOffaet;
				break;
			}
			case ln::tr::GizmoType::Rotation:
			{
				Matrix rot;
				switch (m_operationType)
				{
				case ln::tr::GizmoModel::OperationType::X:
					rot.rotateAxis(m_draggingLocalPlane.Normal, -atan2(localOffaet.y, localOffaet.z));
					break;
				case ln::tr::GizmoModel::OperationType::Y:
					rot.rotateAxis(m_draggingLocalPlane.Normal, -atan2(localOffaet.z, localOffaet.x));
					break;
				case ln::tr::GizmoModel::OperationType::Z:
					rot.rotateAxis(m_draggingLocalPlane.Normal, atan2(localOffaet.y, localOffaet.x));
					break;
				case ln::tr::GizmoModel::OperationType::XYZ:
				{
					Ray ray1 = makeLocalRay(x + 1, y);
					Ray ray2 = makeLocalRay(x, y + 1);
					Vector3 lp1, lp2;
					m_draggingLocalPlane.intersects(ray1, &lp1);
					m_draggingLocalPlane.intersects(ray2, &lp2);
					lp1 -= m_draggingStartLocalPosition;
					lp2 -= m_draggingStartLocalPosition;
					Vector3 raxis = Vector3::normalize(lp1 - localOffaet);
					Vector3 taxis = Vector3::normalize(lp2 - localOffaet);
					rot.rotateAxis(raxis, (y - m_draggingStartViewPixelPoint.y) * -0.05);
					rot.rotateAxis(taxis, (x - m_draggingStartViewPixelPoint.x) * 0.05);
					break;
				}
				case ln::tr::GizmoModel::OperationType::ViewZ:
					rot.rotateAxis(m_draggingLocalPlane.Normal, atan2(localOffaet.y, localOffaet.x));
					break;
				}

				m_gizmoTransform = rot * m_draggingStartGizmoTransform;
				m_targetTransform.rotation = m_targetInitialTransform.rotation * Quaternion::makeFromRotationMatrix(rot);
				break;
			}
			case ln::tr::GizmoType::Scaling:
			{
				float s = Vector3::dot(localOffaet, Vector3::Ones);
				switch (m_operationType)
				{
				case ln::tr::GizmoModel::OperationType::X:
					localOffaet = Vector3(s, 0, 0);
					break;
				case ln::tr::GizmoModel::OperationType::Y:
					localOffaet = Vector3(0, s, 0);
					break;
				case ln::tr::GizmoModel::OperationType::Z:
					localOffaet = Vector3(0, 0, s);
					break;
				case ln::tr::GizmoModel::OperationType::XY:
					localOffaet = Vector3(s, s, 0);
					break;
				case ln::tr::GizmoModel::OperationType::XZ:
					localOffaet = Vector3(s, 0, s);
					break;
				case ln::tr::GizmoModel::OperationType::YZ:
					localOffaet = Vector3(0, s, s);
					break;
				case ln::tr::GizmoModel::OperationType::XYZ:
					localOffaet = Vector3(s, s, s);
					break;
				}

				//localOffaet += Vector3::Ones;
				//m_gizmoTransform = Matrix::makeScaling(localOffaet) * m_draggingStartGizmoTransform;
				m_targetTransform.scale = m_targetInitialTransform.scale + localOffaet;
				break;
			}
		}

		m_onTargetTransformChanged.raise(this);
		return true;
	}
	else
	{
		m_operationType = getDirectionOperationType(x, y);
	}
	return false;
}

//------------------------------------------------------------------------------
bool GizmoModel::injectMouseUp(int x, int y)
{
	if (m_dragging)
	{
		submitEditing();

		m_operationType = getDirectionOperationType(x, y);

		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void GizmoModel::render(DrawList* context)
{
	Matrix gizmoMat;
	gizmoMat.scale(m_screenFactor);
	gizmoMat *= m_gizmoTransform;
	context->setTransform(gizmoMat);	// TODO: old


	Color c;

	float s = OnPlaneBoxSize;
	float s2 = s / 2;

	switch (m_gizmoType)
	{
		case ln::tr::GizmoType::Translation:
		{
			float r = BarRadius;
			float d = BarLendth;
			float cr = r * 2.5;

			// X axis
			c = (m_operationType == OperationType::X) ? Color::White : Color::Red;
			context->drawCylinder(r, d, 8, 1, c, Matrix::makeRotationZ(-Math::PIDiv2) * Matrix::makeTranslation(d / 2, 0, 0));
			context->drawCone(cr, r * 6, 8, c, Matrix::makeRotationZ(-Math::PIDiv2) * Matrix::makeTranslation(d + cr, 0, 0));

			// Y axis
			c = (m_operationType == OperationType::Y) ? Color::White : Color::Green;
			context->drawCylinder(r, d, 8, 1, c, Matrix::makeTranslation(0, d / 2, 0));
			context->drawCone(cr, r * 6, 8, c, Matrix::makeTranslation(0, d + cr, 0));

			// Z axis
			c = (m_operationType == OperationType::Z) ? Color::White : Color::Blue;
			context->drawCylinder(r, d, 8, 1, c, Matrix::makeRotationX(Math::PIDiv2) * Matrix::makeTranslation(0, 0, d / 2));
			context->drawCone(cr, r * 6, 8, c, Matrix::makeRotationX(Math::PIDiv2) * Matrix::makeTranslation(0, 0, d + cr));

			// center
			c = (m_operationType == OperationType::XYZ) ? Color::White : Color::Yellow;
			context->drawSphere(CenterBoxSize, 8, 8, c);

			// YZ plane
			c = (m_operationType == OperationType::YZ) ? Color::White : Color(1, 0, 0, BaseOpacity);
			context->drawSquare(s, s, 1, 1, c, Matrix::makeRotationZ(-Math::PIDiv2) * Matrix::makeTranslation(0, s2, s2), m_tmat);

			// XZ plane
			c = (m_operationType == OperationType::XZ) ? Color::White : Color(0, 1, 0, BaseOpacity);
			context->drawSquare(s, s, 1, 1, c, Matrix::makeTranslation(s2, 0, s2), m_tmat);

			// XY plane
			c = (m_operationType == OperationType::XY) ? Color::White : Color(0, 0, 1, BaseOpacity);
			context->drawSquare(s, s, 1, 1, c, Matrix::makeRotationX(Math::PIDiv2) * Matrix::makeTranslation(s2, s2, 0), m_tmat);

			break;
		}
		case ln::tr::GizmoType::Rotation:
		{
			const float i1 = RotationRingInner;
			const float o1 = RotationRingOuter;

			// center
			c = (m_operationType == OperationType::XYZ) ? Color::White : Color::Yellow;
			context->drawSphere(CenterBoxSize, 8, 8, c);

			// X
			c = (m_operationType == OperationType::X) ? Color::White : Color(1, 0, 0, BaseOpacity);
			context->drawArc(0, Math::PI2, i1, o1, 32, c, Matrix::makeRotationZ(-Math::PIDiv2), m_tmat);

			// Y
			c = (m_operationType == OperationType::Y) ? Color::White : Color(0, 1, 0, BaseOpacity);
			context->drawArc(0, Math::PI2, i1, o1, 32, c, Matrix::Identity, m_tmat);

			// Z
			c = (m_operationType == OperationType::Z) ? Color::White : Color(0, 0, 1, BaseOpacity);
			context->drawArc(0, Math::PI2, i1, o1, 32, c, Matrix::makeRotationX(Math::PIDiv2), m_tmat);

			Matrix viewInv = Matrix::makeScaling(m_screenFactor) * Matrix::makeInverse(m_view);
			viewInv.m41 = m_gizmoTransform.m41;
			viewInv.m42 = m_gizmoTransform.m42; 
			viewInv.m43 = m_gizmoTransform.m43;
			context->setTransform(viewInv);

			c = (m_operationType == OperationType::ViewZ) ? Color::White : Color(1, 1, 0, BaseOpacity);
			context->drawArc(0, Math::PI2, RotationViewZRingInner, RotationViewZRingOuter, 32, c, Matrix::makeRotationX(Math::PIDiv2), m_tmat);
			break;
		}
		case ln::tr::GizmoType::Scaling:
		{
			float r = BarRadius;
			float d = BarLendth;

			// X axis
			c = (m_operationType == OperationType::X) ? Color::White : Color::Red;
			context->drawCylinder(r, d, 8, 1, c, Matrix::makeRotationZ(-Math::PIDiv2) * Matrix::makeTranslation(d / 2, 0, 0));
			context->drawBox(Box(CenterBoxSize), c, Matrix::makeTranslation(d, 0, 0));

			// Y axis
			c = (m_operationType == OperationType::Y) ? Color::White : Color::Green;
			context->drawCylinder(r, d, 8, 1, c, Matrix::makeTranslation(0, d / 2, 0));
			context->drawBox(Box(CenterBoxSize), c, Matrix::makeTranslation(0, d, 0));

			// Z axis
			c = (m_operationType == OperationType::Z) ? Color::White : Color::Blue;
			context->drawCylinder(r, d, 8, 1, c, Matrix::makeRotationX(Math::PIDiv2) * Matrix::makeTranslation(0, 0, d / 2));
			context->drawBox(Box(CenterBoxSize), c, Matrix::makeTranslation(0, 0, d));

			// center
			c = (m_operationType == OperationType::XYZ) ? Color::White : Color::Yellow;
			context->drawBox(Box(CenterBoxSize), c);

			// YZ plane
			c = (m_operationType == OperationType::YZ) ? Color::White : Color(1, 0, 0, BaseOpacity);
			context->drawSquare(s, s, 1, 1, c, Matrix::makeRotationZ(-Math::PIDiv2) * Matrix::makeTranslation(0, s2, s2), m_tmat);

			// XZ plane
			c = (m_operationType == OperationType::XZ) ? Color::White : Color(0, 1, 0, BaseOpacity);
			context->drawSquare(s, s, 1, 1, c, Matrix::makeTranslation(s2, 0, s2), m_tmat);

			// XY plane
			c = (m_operationType == OperationType::XY) ? Color::White : Color(0, 0, 1, BaseOpacity);
			context->drawSquare(s, s, 1, 1, c, Matrix::makeRotationX(Math::PIDiv2) * Matrix::makeTranslation(s2, s2, 0), m_tmat);

			break;
		}
	}

	context->setTransform(Matrix::Identity);


}

//------------------------------------------------------------------------------
void GizmoModel::submitEditing()
{
	m_targetInitialTransform = m_targetTransform;
	
	// 拡大・回転をリセット
	m_gizmoTransform = Matrix::makeTranslation(m_gizmoTransform.getPosition());
	m_gizmoInitialTransform = m_gizmoTransform;

	m_dragging = false;

	m_onSubmitEditing.raise(this);
}

//------------------------------------------------------------------------------
void GizmoModel::makeScreenFactor()
{
	Matrix viewproj = m_view * m_proj;
	Vector4 trf = Vector4(m_gizmoTransform.getPosition(), 1.0f);
	trf = Vector4::transform(trf, viewproj);
	m_screenFactor = m_displayScale * 0.15f * trf.w;
}

//------------------------------------------------------------------------------
GizmoModel::OperationType GizmoModel::getDirectionOperationType(int x, int y, Plane* outLocalPlane)
{
 	bool xz, xy, yz;
	Vector3 ptXZ, ptXY, ptYZ;
	Ray localViewRay;
	intersectsLocalPlanes(x, y, &xz, &ptXZ, &xy, &ptXY, &yz, &ptYZ, &localViewRay);
	
	if (xz && xy && yz &&
		(abs(ptXZ.x) <= CenterBoxSize && (ptXZ.z) <= CenterBoxSize) &&
		(abs(ptXY.x) <= CenterBoxSize && (ptXY.y) <= CenterBoxSize) &&
		(abs(ptYZ.y) <= CenterBoxSize && (ptYZ.z) <= CenterBoxSize))
	{
		if (outLocalPlane) *outLocalPlane = Plane(-localViewRay.direction);
		return OperationType::XYZ;
	}

	switch (m_gizmoType)
	{
		case ln::tr::GizmoType::Translation:
		case ln::tr::GizmoType::Scaling:
		{
			if (xz)
			{
				if (outLocalPlane) *outLocalPlane = Plane(Vector3::UnitY);
				if ((ptXZ.x >= 0) && (ptXZ.x <= 1) && (fabs(ptXZ.z) < 0.1f)) return OperationType::X;
				if ((ptXZ.z >= 0) && (ptXZ.z <= 1) && ( fabs(ptXZ.x) < 0.1f)) return OperationType::Z;
				if ((ptXZ.x < OnPlaneBoxSize) && (ptXZ.z < OnPlaneBoxSize) && (ptXZ.x > 0) && (ptXZ.z > 0))  return OperationType::XZ;
			}
			if (xy)
			{
				if (outLocalPlane) *outLocalPlane = Plane(Vector3::UnitZ);
				if ((ptXY.x >= 0 ) && (ptXY.x <= 1) && (fabs(ptXY.y) < 0.1f)) return OperationType::X;
				if ((ptXY.y >= 0 ) && (ptXY.y <= 1) && (fabs(ptXY.x) < 0.1f)) return OperationType::Y;
				if ((ptXY.x < OnPlaneBoxSize) && (ptXY.y < OnPlaneBoxSize) && (ptXY.x > 0) && (ptXY.y > 0)) return OperationType::XY;
			}
			if (yz)
			{
				if (outLocalPlane) *outLocalPlane = Plane(Vector3::UnitX);
				if ((ptYZ.y >= 0) && (ptYZ.y <= 1) && (fabs(ptYZ.z) < 0.1f)) return OperationType::Y;
				if ((ptYZ.z >= 0) && (ptYZ.z <= 1) && (fabs(ptYZ.y) < 0.1f)) return OperationType::Z;
				if ((ptYZ.y < OnPlaneBoxSize) && (ptYZ.z < OnPlaneBoxSize) && (ptYZ.y > 0) && (ptYZ.z > 0)) return OperationType::YZ;
			}
			break;
		}
		case ln::tr::GizmoType::Rotation:
		{
			if (xz)
			{
				if (outLocalPlane) *outLocalPlane = Plane(Vector3::UnitY);
				float d = ptXZ.getLength();
				if (RotationRingInner <= d && d <= RotationRingOuter) return OperationType::Y;
			}
			if (xy)
			{
				if (outLocalPlane) *outLocalPlane = Plane(Vector3::UnitZ);
				float d = ptXY.getLength();
				if (RotationRingInner <= d && d <= RotationRingOuter) return OperationType::Z;
			}
			if (yz)
			{
				if (outLocalPlane) *outLocalPlane = Plane(Vector3::UnitX);
				float d = ptYZ.getLength();
				if (RotationRingInner <= d && d <= RotationRingOuter) return OperationType::X;
			}

			if (xz && xy && yz)
			{
				Plane localPlane(-localViewRay.direction);
				if (outLocalPlane) *outLocalPlane = localPlane;

				Vector3 pt;
				localPlane.intersects(localViewRay, &pt);
				float d = pt.getLength();
				if (RotationViewZRingInner <= d && d <= RotationViewZRingOuter) return OperationType::ViewZ;
			}
			break;
		}
	}

	return OperationType::None;
}

//------------------------------------------------------------------------------
GizmoModel::OperationType GizmoModel::getRotationOperationType(int x, int y)
{
 	bool xz, xy, yz;
	Vector3 ptXZ, ptXY, ptYZ;
	Ray ray;
	intersectsLocalPlanes(x, y, &xz, &ptXZ, &xy, &ptXY, &yz, &ptYZ, &ray);
	
	LN_NOTIMPLEMENTED();
	
	return OperationType::None;
}

//------------------------------------------------------------------------------
Ray GizmoModel::makeLocalRay(int x, int y)
{
	Matrix gizmoMat;
	gizmoMat.scale(m_screenFactor);
	gizmoMat *= m_gizmoInitialTransform;

	Matrix viewproj = m_view * m_proj;

	Ray ray;
	ray.origin = m_viewPosition;
	ray.direction = Vector3::unproject(Vector3(x, y, 0), viewproj, 0, 0, m_viewPixelSize.width, m_viewPixelSize.height);
	ray.direction = ray.direction - ray.origin;

	// ray を gizmo のローカル空間へ変換する
	Matrix gizmoInv = Matrix::makeInverse(gizmoMat);
	{
		//ray.transformCoord(gizmoInv);
		ray.direction += ray.origin;
		ray.origin.transformCoord(gizmoInv);
		ray.direction.transformCoord(gizmoInv);
		ray.direction -= ray.origin;
		ray.direction.normalize();
	}

	return ray;
}

//------------------------------------------------------------------------------
void GizmoModel::intersectsLocalPlanes(int x, int y, bool* xz, Vector3* ptXZ, bool* xy, Vector3* ptXY, bool* yz, Vector3* ptYZ, Ray* localViewRay)
{
	Ray ray = makeLocalRay(x, y);
	
	// 各軸平面と衝突判定
	*xz = Plane(Vector3::UnitY).intersects(ray, ptXZ);
	*xy = Plane(Vector3::UnitZ).intersects(ray, ptXY);
	*yz = Plane(Vector3::UnitX).intersects(ray, ptYZ);
	
	*localViewRay = ray;
}

} // namespace tr
LN_NAMESPACE_END

