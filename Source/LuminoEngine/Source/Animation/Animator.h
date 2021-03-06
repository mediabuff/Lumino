﻿
#pragma once
#include <Lumino/Animation/Common.h>
#include <Lumino/Animation/AnimationCurve.h>
#include "Detail.h"

LN_NAMESPACE_BEGIN
namespace detail { class AnimationLayer; }

/** 
	@note	SkinnedMesh に対応し、インスタンス扱い。
*/
class Animator
	: public RefObject
{
public:
	static const int MaxLayers = 8;

public:
	Animator();
	~Animator();

public:

	/// 作成
	void create(detail::IAnimationTargetElement* element);

	/// 再生中であるかを確認する
	bool isPlaying() const;

	/// 再生
	void play(const Char* name, float duration = 0.3f/*, PlayMode mode = PlayMode_StopSameLayer*/);

	///// ブレンド (アニメーションの再生には影響しない。停止中のアニメーションがこの関数によって再生開始されることはない)
	//void Blend(const lnKeyChar* animName, lnFloat targetWeight, lnFloat fadeLength);

	///// クロスフェード
	//void CrossFade(const lnKeyChar* animName, lnFloat fadeLength, PlayMode mode = StopSameLayer);

	///// 前のアニメーションが終了した後、再生を開始する
	//void PlayQueued(const lnKeyChar* animName, QueueMode queueMode = CompleteOthers, PlayMode playMode = StopSameLayer);

	///// 前のアニメーションが終了するとき、クロスフェードで再生を開始する
	//void CrossFadeQueued(const lnKeyChar* animName, lnFloat fadeLength, QueueMode queueMode = CompleteOthers, PlayMode playMode = StopSameLayer);

	///// 同レイヤー内のアニメーション再生速度の同期
	//void SyncLayer(int layer);

	/// アニメーションを進める
	void advanceTime(double elapsedTime);

public:

	/// アニメーションクリップの追加
	void addAnimationClip(AnimationClip* animationClip, int layer = 0);

	/// アニメーションクリップの削除
	void removeAnimationClip(AnimationClip* animationClip, int layer = 0);

	/// AnimationTargetEntity の検索 (見つからなければ NULL)
	detail::AnimationTargetAttributeEntity* findAnimationTargetAttributeEntity(const String& name);


	/// AnimationApplyTarget の追加 (できるだけ AnimationController 作成直後に設定すること。重複禁止)
	//void addAnimationApplyTarget(IAnimationApplyTarget* target);

LN_INTERNAL_ACCESS:
	//void AddLayer();

private:
	typedef List<detail::AnimationTargetAttributeEntity>	AnimationTargetAttributeEntityList;

	detail::IAnimationTargetElement*	m_element;
	AnimationTargetAttributeEntityList	m_animationTargetAttributeEntityList;

	std::array<Ref<detail::AnimationLayer>, MaxLayers>	m_layerList;

	///// 追加済み AnimationApplyTarget の検索
	//AnimationApplyTargetState* findAnimationApplyTargetState(const lnKeyChar* naem);

	//typedef std::vector<AnimationState*>			AnimationStateArray;
	//typedef std::vector<AnimationApplyTargetState>	AnimationApplyTargetStateArray;

	//AnimationStateArray				mAnimationStateArray;
	//AnimationApplyTargetStateArray	mAnimationApplyTargetStateArray;
};

namespace detail
{

class AnimationLayer
	: public RefObject
{
public:
	typedef SortedArray<String, Ref<AnimationState>>	AnimationStateList;

	Animator*			m_owner;
	AnimationStateList	m_animationStateList;

	AnimationLayer(Animator* owner);
	void createStateAndAttachClip(AnimationClip* animationClip);
	void removeStateByClip(AnimationClip* animationClip);
	void transitionState(const StringRef& name, float duration);
	void advanceTime(float elapsedTime);
	AnimationState* findAnimationState(const StringRef& clipName);
};

} // namespace detail

LN_NAMESPACE_END
