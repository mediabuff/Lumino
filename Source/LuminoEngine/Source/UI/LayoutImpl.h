
#pragma once

LN_NAMESPACE_BEGIN
namespace detail {

template<class TPanel>
class LayoutImpl
{
public:
	using BaseMeasureOverrideCallback = Size(*)(TPanel* panel, const Size& constraint);

	//------------------------------------------------------------------------------
	static Size UILayoutPanel_measureOverride(TPanel* panel, const Size& constraint, BaseMeasureOverrideCallback baseCallback)
	{
		Size desiredSize = baseCallback(static_cast<TPanel*>(panel), constraint);
		int childCount = panel->getLayoutChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			ILayoutElement* child = panel->getLayoutChild(i);

			child->measureLayout(constraint);
			const Size& childDesiredSize = child->getLayoutDesiredSize();

			desiredSize.width = std::max(desiredSize.width, childDesiredSize.width);
			desiredSize.height = std::max(desiredSize.height, childDesiredSize.height);
		}
		return desiredSize;
	}

	//------------------------------------------------------------------------------
	static Size UILayoutPanel_arrangeOverride(TPanel* panel, const Vector2& offset, const Size& finalSize)
	{
		int childCount = panel->getLayoutChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			ILayoutElement* child = panel->getLayoutChild(i);
			Size childDesiredSize = child->getLayoutDesiredSize();
			childDesiredSize.width = std::max(finalSize.width, childDesiredSize.width);
			childDesiredSize.height = std::max(finalSize.height, childDesiredSize.height);
			child->arrangeLayout(Rect(offset.x, offset.y, childDesiredSize));
		}
		return finalSize;
	}

	//------------------------------------------------------------------------------
	static Size UIStackPanel_measureOverride(ILayoutPanel* panel, const Size& constraint, Orientation orientation)
	{
		Size size = constraint;

		if (orientation == Orientation::Horizontal)
		{
			// 横に並べる場合、幅の制限を設けない
			size.width = std::numeric_limits<float>::infinity();
		}
		else
		{
			// 縦に並べる場合、高さの制限を設けない
			size.height = std::numeric_limits<float>::infinity();
		}

		Size desiredSize;
		int childCount = panel->getLayoutChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			ILayoutElement* child = panel->getLayoutChild(i);
			child->measureLayout(size);

			const Size& childDesiredSize = child->getLayoutDesiredSize();
			if (orientation == Orientation::Horizontal || orientation == Orientation::ReverseHorizontal)
			{
				desiredSize.width += childDesiredSize.width;
				desiredSize.height = std::max(desiredSize.height, childDesiredSize.height);
			}
			else
			{
				desiredSize.width = std::max(desiredSize.width, childDesiredSize.width);
				desiredSize.height += child->getLayoutDesiredSize().height;
			}
		}

		return desiredSize;
	}

	//------------------------------------------------------------------------------
	static Size UIStackPanel_arrangeOverride(TPanel* panel, const Size& finalSize, Orientation orientation)
	{
		//ILayoutPanel* basePanel = static_cast<ILayoutPanel*>(panel);
		//Size childrenBoundSize(finalSize.width, finalSize.height);		
		ILayoutPanel* basePanel = static_cast<ILayoutPanel*>(panel);
		const Thickness& padding = static_cast<ILayoutElement*>(panel)->getLayoutPadding();
		Size childrenBoundSize(finalSize.width - (padding.left + padding.right), finalSize.height - (padding.top + padding.bottom));

		float prevChildSize = 0;
		float rPos = 0;
		//Rect childRect(0, 0, 0, 0);
		Rect childRect(padding.left, padding.top, 0, 0);
		int childCount = basePanel->getLayoutChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			ILayoutElement* child = basePanel->getLayoutChild(i);
			const Size& childDesiredSize = child->getLayoutDesiredSize();

			switch (orientation)
			{
			case Orientation::Horizontal:
				childRect.x += prevChildSize;
				prevChildSize = childDesiredSize.width;
				childRect.width = prevChildSize;
				childRect.height = childrenBoundSize.height;
				break;
			case Orientation::Vertical:
				childRect.y += prevChildSize;
				prevChildSize = childDesiredSize.height;
				childRect.width = childrenBoundSize.width;
				childRect.height = prevChildSize;
				break;
			case Orientation::ReverseHorizontal:
				prevChildSize = childDesiredSize.width;
				rPos -= prevChildSize;
				childRect.x = childrenBoundSize.width + rPos;
				childRect.width = prevChildSize;
				childRect.height = childrenBoundSize.height;
				break;
			case Orientation::ReverseVertical:
				prevChildSize = childDesiredSize.height;
				rPos -= prevChildSize;
				childRect.y = childrenBoundSize.height + rPos;
				childRect.width = childrenBoundSize.width;
				childRect.height = prevChildSize;
				break;
			default:
				assert(0);
				break;
			}

			child->arrangeLayout(childRect);
		}

		return finalSize;
	}

	//------------------------------------------------------------------------------
	static Size UIGridLayout_measureOverride(TPanel* panel, const Size& constraint, BaseMeasureOverrideCallback baseCallback)
	{
		ILayoutPanel* basePanel = static_cast<ILayoutPanel*>(panel);
		int rowDefCount = basePanel->getLayoutGridRowDefinitionCount();
		int colDefCount = basePanel->getLayoutGridColumnDefinitionCount();

		int childCount = basePanel->getLayoutChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			ILayoutElement* child = basePanel->getLayoutChild(i);

			// まずは子を measure
			child->measureLayout(constraint);

			// child が配置されるべき row と column を探す
			int rowIdx = child->getLayoutRow();
			int colIdx = child->getLayoutColumn();

			rowIdx = (0 <= rowIdx && rowIdx < rowDefCount) ? rowIdx : 0;
			colIdx = (0 <= colIdx && colIdx < colDefCount) ? colIdx : 0;

			detail::GridDefinitionData* row = (rowIdx < rowDefCount) ? basePanel->getLayoutGridRowDefinition(rowIdx) : nullptr;
			detail::GridDefinitionData* col = (colIdx < colDefCount) ? basePanel->getLayoutGridColumnDefinition(colIdx) : nullptr;

			// 子要素の DesiredSize (最低サイズ) を測るのは、セルのサイズ指定が "Auto" の時だけでよい。
			const Size& childDesiredSize = child->getLayoutDesiredSize();
			if (row != nullptr && row->type == GridLengthType::Auto)
			{
				row->desiredSize = std::max(row->desiredSize, childDesiredSize.height);
			}
			if (col != nullptr && col->type == GridLengthType::Auto)
			{
				col->desiredSize = std::max(col->desiredSize, childDesiredSize.width);
			}
		}

		// 各セルの DesiredSize を集計して、Grid 全体の DesiredSize を求める
		Size desiredSize = baseCallback(panel, constraint);
		for (int iRow = 0; iRow < rowDefCount; iRow++)
		{
			desiredSize.height += basePanel->getLayoutGridRowDefinition(iRow)->getAvailableDesiredSize();
		}
		for (int iCol = 0; iCol < colDefCount; iCol++)
		{
			desiredSize.width += basePanel->getLayoutGridColumnDefinition(iCol)->getAvailableDesiredSize();
		}

		return desiredSize;
	}

	//------------------------------------------------------------------------------
	static Size UIGridLayout_arrangeOverride(TPanel* panel, const Size& finalSize)
	{
		//ILayoutPanel* basePanel = static_cast<ILayoutPanel*>(panel);
		//Size childrenBoundSize(finalSize.width, finalSize.height);
		ILayoutPanel* basePanel = static_cast<ILayoutPanel*>(panel);
		const Thickness& padding = static_cast<ILayoutElement*>(panel)->getLayoutPadding();
		Size childrenBoundSize(finalSize.width - (padding.left + padding.right), finalSize.height - (padding.top + padding.bottom));


		// "Auto" と "Pixel" 指定である Row/Column の最終サイズを確定させる。
		// また、"*" である行列の数をカウントする。
		Size totalActual = Size::Zero;
		float starRowCount = 0.0f;
		float starColCount = 0.0f;
		int rowDefCount = basePanel->getLayoutGridRowDefinitionCount();
		int colDefCount = basePanel->getLayoutGridColumnDefinitionCount();
		for (int iRow = 0; iRow < rowDefCount; iRow++)
		{
			auto* row = basePanel->getLayoutGridRowDefinition(iRow);
			if (row->type == GridLengthType::Auto || row->type == GridLengthType::Pixel)
			{
				row->actualSize = row->getAvailableDesiredSize();
				totalActual.height += row->actualSize;
			}
			else
			{
				starRowCount += row->getRatioSize();
			}
		}
		for (int iCol = 0; iCol < colDefCount; iCol++)
		{
			auto* col = basePanel->getLayoutGridColumnDefinition(iCol);
			if (col->type == GridLengthType::Auto || col->type == GridLengthType::Pixel)
			{
				col->actualSize = col->getAvailableDesiredSize();
				totalActual.width += col->actualSize;
			}
			else
			{
				starColCount += col->getRatioSize();
			}
		}

		// "1*" 分のセルの領域を計算する
		Size starUnit(
			(starColCount != 0.0f) ? (childrenBoundSize.width - totalActual.width) / starColCount : 0.0f,
			(starRowCount != 0.0f) ? (childrenBoundSize.height - totalActual.height) / starRowCount : 0.0f);
		starUnit.width = std::max(0.0f, starUnit.width);	// 負値はダメ
		starUnit.height = std::max(0.0f, starUnit.height);	// 負値はダメ

		// "*" 指定である Row/Column の最終サイズを確定させ、
		// 全セルのオフセット (位置) も確定させる
		Point totalOffset;
		for (int iRow = 0; iRow < rowDefCount; iRow++)
		{
			auto* row = basePanel->getLayoutGridRowDefinition(iRow);
			if (row->type == GridLengthType::Ratio)
			{
				row->actualSize = starUnit.height * row->getRatioSize();
			}

			row->adjustActualSize();

			// セルY座標確定
			row->actualOffset = totalOffset.y;
			totalOffset.y += row->actualSize;
		}
		for (int iCol = 0; iCol < colDefCount; iCol++)
		{
			auto* col = basePanel->getLayoutGridColumnDefinition(iCol);
			if (col->type == GridLengthType::Ratio)
			{
				col->actualSize = starUnit.width * col->getRatioSize();
			}

			col->adjustActualSize();

			// セルX座標確定
			col->actualOffset = totalOffset.x;
			totalOffset.x += col->actualSize;
		}

		// 子要素の最終位置・サイズを確定させる
		int childCount = basePanel->getLayoutChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			ILayoutElement* child = basePanel->getLayoutChild(i);
			int rowIdx = child->getLayoutRow();
			int colIdx = child->getLayoutColumn();
			int rowSpan = child->getLayoutRowSpan();
			int colSpan = child->getLayoutColumnSpan();
			rowSpan = std::max(1, rowSpan);	// 最低 1
			colSpan = std::max(1, colSpan);	// 最低 1
			rowSpan = std::min(rowSpan, rowIdx + rowDefCount);	// 最大値制限
			colSpan = std::min(colSpan, colIdx + colDefCount);	// 最大値制限

			// Span を考慮してサイズを確定
			//Rect rect = Rect(0, 0, 0, 0);
			Rect rect = Rect(padding.left, padding.top, 0, 0);
			if (rowDefCount == 0)	// is empty
			{
				rect.height = childrenBoundSize.height;
			}
			else
			{
				rect.y += basePanel->getLayoutGridRowDefinition(rowIdx)->actualOffset;
				for (int iRow = 0; iRow < rowSpan; ++iRow)
				{
					rect.height += basePanel->getLayoutGridRowDefinition(rowIdx + iRow)->actualSize;
				}
			}
			if (colDefCount == 0)	// is empty
			{
				rect.width = childrenBoundSize.width;
			}
			else
			{
				rect.x += basePanel->getLayoutGridColumnDefinition(colIdx)->actualOffset;
				for (int iCol = 0; iCol < colSpan; ++iCol)
				{
					rect.width += basePanel->getLayoutGridColumnDefinition(colIdx + iCol)->actualSize;
				}
			}

			// Arrange
			child->arrangeLayout(rect);
		}

		return finalSize;
	}
};

} // namespace detail
LN_NAMESPACE_END
