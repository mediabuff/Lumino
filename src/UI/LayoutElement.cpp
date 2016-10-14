
#include "Internal.h"
#include <Lumino/UI/LayoutElement.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// ILayoutElement
//==============================================================================

//------------------------------------------------------------------------------
ILayoutElement::ILayoutElement()
{
}

//------------------------------------------------------------------------------
ILayoutElement::~ILayoutElement()
{
}

//------------------------------------------------------------------------------
void ILayoutElement::MeasureLayout(const SizeF& availableSize)
{
	//// �������t���O�����̗v�f�ɓ`�d������
	//if (m_parent != nullptr)
	//{
	//	// �t�H���g�� MeasureOverride() �̒��ōX�V����
	//	m_invalidateFlags |= (m_parent->m_invalidateFlags & detail::InvalidateFlags::Font);
	//}

	// �e�v�f����q�v�f��z�u�ł���͈�(availableSize)���󂯎��ADesiredSize ���X�V����B
	// �@ Pane �\[measure()   �c ���͈͓̔��Ȃ�z�u�ł����]�� Button
	// �A Pane ��[DesiredSize �c ���Ⴀ���̃T�C�Y�ł��肢���܂�]�\ Button		�����̎��_�� inf ��Ԃ����Ƃ����蓾��B
	// �B Pane �\[arrange()   �c ���̎q�v�f�Ƃ̌��ˍ����ōŏI�T�C�Y�̓R����]�� Button
	// http://www.kanazawa-net.ne.jp/~pmansato/wpf/wpf_ctrl_arrange.htm

	// Margin ���l������
	const ThicknessF& margin = GetLayoutMargin();
	float marginWidth = margin.Left + margin.Right;
	float marginHeight = margin.Top + margin.Bottom;
	SizeF localAvailableSize(
		std::max(availableSize.width - marginWidth, 0.0f),
		std::max(availableSize.height - marginHeight, 0.0f));

	SizeF desiredSize = MeasureOverride(localAvailableSize);

	// Margin ���l������
	desiredSize.width += marginWidth;
	desiredSize.height += marginHeight;

	SetLayoutDesiredSize(desiredSize);
}

//------------------------------------------------------------------------------
void ILayoutElement::ArrangeLayout(const RectF& finalLocalRect)
{
	// finalLocalRect �͂��̗v�f��z�u�ł���̈�T�C�Y�B�ƁA�e�v�f���ł̃I�t�Z�b�g�B
	// �v�f�ɒ��ڐݒ肳��Ă���T�C�Y�����傫�����Ƃ�����B
	// TODO: HorizontalAlignment �����l�����āA�ŏI�I�ȍ��W�ƃT�C�Y�����肷��B
	//		 ���̗v�f�̃T�C�Y���ȗ�����Ă���΁AStretch �Ȃ�T�C�Y�͍ő�ɁA����ȊO�Ȃ�ŏ��ɂȂ�B

	SizeF arrangeSize;

	// ���̗v�f�̃T�C�Y�������I�Ɏw�肳��Ă���ꍇ�͂������D�悷��
	const SizeF& size = GetLayoutSize();
	arrangeSize.width = Math::IsNaNOrInf(size.width) ? finalLocalRect.width : size.width;
	arrangeSize.height = Math::IsNaNOrInf(size.height) ? finalLocalRect.height : size.height;

	ILayoutElement* parent = GetLayoutParent();
	HorizontalAlignment  hAlign = GetLayoutHorizontalAlignment();
	VerticalAlignment    vAlign = GetLayoutVerticalAlignment();
	HorizontalAlignment* parentHAlign = (parent != nullptr) ? parent->GetLayoutContentHorizontalAlignment() : nullptr;
	VerticalAlignment*   parentVAlign = (parent != nullptr) ? parent->GetLayoutContentVerticalAlignment() : nullptr;
	if (parentHAlign != nullptr) hAlign = *parentHAlign;
	if (parentVAlign != nullptr) vAlign = *parentVAlign;

	const SizeF& ds = GetLayoutDesiredSize();
	RectF arrangeRect;
	detail::LayoutHelper::AdjustHorizontalAlignment(arrangeSize, ds, hAlign, &arrangeRect);
	detail::LayoutHelper::AdjustVerticalAlignment(arrangeSize, ds, vAlign, &arrangeRect);

	// Margin ���l������ (0 �ȉ��ɂ͏o���Ȃ�)
	const ThicknessF& margin = GetLayoutMargin();
	float marginWidth = margin.Left + margin.Right;
	float marginHeight = margin.Top + margin.Bottom;
	arrangeRect.width = std::max(arrangeRect.width - marginWidth, 0.0f);
	arrangeRect.height = std::max(arrangeRect.height - marginHeight, 0.0f);


	SizeF renderSize = ArrangeOverride(arrangeRect.GetSize());
	RectF thisFinalLocalRect;
	thisFinalLocalRect.x = finalLocalRect.x + margin.Left + arrangeRect.x;
	thisFinalLocalRect.y = finalLocalRect.y + margin.Top + arrangeRect.y;
	thisFinalLocalRect.width = renderSize.width;
	thisFinalLocalRect.height = renderSize.height;
	SetLayoutFinalLocalRect(thisFinalLocalRect);
}

//------------------------------------------------------------------------------
SizeF ILayoutElement::MeasureOverride(const SizeF& constraint)
{
	// �߂�l�́Aconstraint �̐����̒��ŁA�q�v�f�����C�A�E�g���邽�߂ɕK�v�ȍŏ��T�C�Y�B
	// ���[�U�[�w��̃T�C�Y������ꍇ�͂����Ԃ��B
	// �������Aconstraint �𒴂��邱�Ƃ͂ł��Ȃ��B

	const SizeF& size = GetLayoutSize();
	SizeF desiredSize;
	// NaN �̏ꍇ�A���̗v�f�Ƃ��ĕK�v�ȍŏ��T�C�Y�� 0 �ƂȂ�B
	desiredSize.width = Math::IsNaNOrInf(size.width) ? 0.0f : size.width;
	desiredSize.height = Math::IsNaNOrInf(size.height) ? 0.0f : size.height;
	desiredSize.width = std::min(desiredSize.width, constraint.width);
	desiredSize.height = std::min(desiredSize.height, constraint.height);

	return desiredSize;
}

//------------------------------------------------------------------------------
SizeF ILayoutElement::ArrangeOverride(const SizeF& finalSize)
{
	return finalSize;
}

LN_NAMESPACE_END