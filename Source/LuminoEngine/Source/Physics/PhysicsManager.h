﻿
#pragma once
#include <Lumino/MultiThreadingObjectList.h>
#include <Lumino/Physics/Common.h>

LN_NAMESPACE_BEGIN
class RigidBodyComponent;
class Joint;

namespace detail
{
class PhysicsManager;
class PhysicsDebugDrawer;


class PhysicsManager
	: public RefObject
{
public:
	static PhysicsManager* Instance;

public:
	PhysicsManager();
	virtual ~PhysicsManager();

	void initialize();
	void dispose();

	//void AddPhysicsWorld(PhysicsWorldCore* world) { m_worldList.Add(world); }
	//void RemovePhysicsWorld(PhysicsWorldCore* world) { m_worldList.Remove(world); }
	//void DrawDebugShapesAllWorld(IDebugRenderer* renderer);

private:
	//List<PhysicsWorldCore*>	m_worldList;

//
//
//	/// stepSimulation() の前処理。メインスレッドで呼び出すこと。物理更新スレッドが実行中であれば待機するまで待つ。
//	void SyncBeforeStepSimulation();
//
//	/// 
//	void stepSimulation(float elapsedTime);
//
//	void drawDebugShapes(IDebugRenderer* renderer);
//
//public:	// internal
//
//	/// メインスレッド、または物理演算スレッドから呼ばれる。
//	void StepSimulationInternal();
//
//	void AddRigidBody(RigidBodyComponent* body);
//	void addJoint(Joint* joint);


//private:
//
//	// それぞれワールドへの追加メソッドが違うので分けておく
//	//typedef MultiThreadingRefObjectList<CollisionBody*>	CollisionBodyList;
//	//typedef MultiThreadingRefObjectList<RigidBodyComponent*>		RigidBodyList;		// ソフトボディとはワールドへの追加方法も違うので分けておく
//	//typedef MultiThreadingRefObjectList<Joint*>			JointList;
//
//	SimulationType	m_simulationType;
//
//
//	//btSoftRigidDynamicsWorld*			            m_btWorld;
//	//btSoftBodyWorldInfo							m_btSoftBodyWorldInfo;
//	//btSoftBodyRigidBodyCollisionConfiguration*	m_btCollisionConfiguration;
//
//	//RigidBodyArray			mRigidBodyArrayList;
//	//CollisionBodyArray		mCollisionBodyArray;
//	//JointArray				mJointArray;
//
//
//	ASyncSimulationState		m_asyncSimulationState;
//	//CollisionBodyList			m_collisionBodyList;
//	RigidBodyList				m_rigidBodyList;
//	JointList					m_jointList;
//
//	float						m_elapsedTime;
};

} // namespace detail
LN_NAMESPACE_END
