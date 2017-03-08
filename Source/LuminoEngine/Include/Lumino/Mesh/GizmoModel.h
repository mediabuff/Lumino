﻿
#pragma once

LN_NAMESPACE_BEGIN
class DrawList;

namespace tr {
class GizmoModel;
using GizmoModelPtr = RefPtr<GizmoModel>;

enum class GizmoType
{
	Translation,
	Rotation,
	Scaling,
};

class GizmoModel
	: public RefObject
{
public:
	static GizmoModelPtr Create();

public:
	void SetGizmoType(GizmoType type);

	void Setup(const Matrix& parentSpaceTransform, const AttitudeTransform& targetInitialTransform);
	
	//void SetTargetTransform(const Matrix& transform);
	const AttitudeTransform& GetTargetTransform() const;
	
	void SetViewInfo(const Vector3& viewPosition, const Matrix& view, const Matrix& proj, const SizeI& viewPixelSize);
	void SetDisplayScale(float scale);
	
	bool InjectMouseDown(int x, int y);
	bool InjectMouseMove(int x, int y);
	bool InjectMouseUp(int x, int y);
	//bool InjectKeyDown(Keys keyCode, ModifierKeys modifierKeys);

	void Render(DrawList* context);

	void AddOnTargetTransformChanged(const std::function<void(GizmoModel*)>& handler)
	{
		m_onTargetTransformChanged.Connect(handler);
	}
	void AddOnSubmitEditing(const std::function<void(GizmoModel*)>& handler)
	{
		m_onSubmitEditing.Connect(handler);
	}
	
LN_INTERNAL_ACCESS:
	
	enum class OperationType
	{
		None,
		X,
		Y,
		Z,
		XY,
		XZ,
		YZ,
		XYZ,

		//ViewXY,
		ViewZ,
	};
	
	GizmoModel();
	virtual ~GizmoModel();
	void Initialize(ln::detail::GraphicsManager* manager);

	void SubmitEditing();

	void MakeScreenFactor();
	
	OperationType GetDirectionOperationType(int x, int y, Plane* outLocalPlane = nullptr);	// translation or scaling
	OperationType GetRotationOperationType(int x, int y);
	
	Ray MakeLocalRay(int x, int y);
	void IntersectsLocalPlanes(int x, int y, bool* xz, Vector3* ptXZ, bool* xy, Vector3* ptXY, bool* yz, Vector3* ptYZ, Ray* localViewRay);

private:
	static const float CenterBoxSize;
	static const float BarRadius;
	static const float BarLendth;
	static const float OnPlaneBoxSize;
	static const float RotationRingInner;
	static const float RotationRingOuter;
	static const float RotationViewZRingInner;
	static const float RotationViewZRingOuter;
	static const float BaseOpacity;

	//RefPtr<MeshResource>	m_mesh;
	GizmoType				m_gizmoType;
	//Matrix					m_parentSpaceTransform;
	AttitudeTransform					m_targetInitialTransform;
	AttitudeTransform					m_targetTransform;	// Gizmo によって操作される Transform
	Matrix					m_gizmoInitialTransform;
	Matrix					m_gizmoTransform;	// Gizmo 自体の Transform (視点距離によるスケーリングは含まれない)
	Vector3					m_viewPosition;
	Matrix					m_view;
	Matrix					m_proj;
	float					m_displayScale;
	float					m_screenFactor;
	SizeI					m_viewPixelSize;

	OperationType			m_operationType;
	RefPtr<Material>		m_tmat;

	bool					m_dragging;
	Plane					m_draggingLocalPlane;
	Vector3					m_draggingStartLocalPosition;
	Matrix					m_draggingStartGizmoTransform;
	PointI					m_draggingStartViewPixelPoint;

	Event2<void(GizmoModel*)>	m_onTargetTransformChanged;
	Event2<void(GizmoModel*)>	m_onSubmitEditing;
};

} // namespace tr
LN_NAMESPACE_END

