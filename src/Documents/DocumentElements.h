
#pragma once
#include <Lumino/Documents/Common.h>
#include <Lumino/UI/LayoutElement.h>
#include "../Graphics/GraphicsManager.h"

LN_NAMESPACE_BEGIN
namespace detail {
class DocumentsManager;
class Inline;

/**
	@brief
*/
class Document
	: public Object
{
public:
	Document();
	virtual ~Document();
	void Initialize(DocumentsManager* manager);

private:
	DocumentsManager*	m_manager;
};

/**
	@brief
*/
class TextElement
	: public Object
	, public ILayoutElement
{
public:
	TextElement();
	virtual ~TextElement();
	void Initialize(DocumentsManager* manager);

	/** �t�H���g�t�@�~������ݒ肵�܂��B*/
	void SetFontFamily(const StringRef& value) { m_fontData.Family = value; m_fontDataModified = true; }

	/** �t�H���g�t�@�~�������擾���܂��B*/
	const String& GetFontFamily() const { return m_fontData.Family; }

	/** �t�H���g�T�C�Y��ݒ肵�܂��B*/
	void SetFontSize(int value) { m_fontData.Size = value; m_fontDataModified = true; }

	/** �t�H���g�T�C�Y���擾���܂��B*/
	int GetFontSize() const { return m_fontData.Size; }

	/** �t�H���g�̑����L����ݒ肵�܂��B*/
	void SetFontBold(bool value) { m_fontData.IsBold = value; m_fontDataModified = true; }

	/** �t�H���g�̑����L�����擾���܂��B*/
	bool IsFontBold() const { return m_fontData.IsBold; }

	/** �t�H���g�̃C�^���b�N�̗L����ݒ肵�܂��B*/
	void SetFontItalic(bool value) { m_fontData.IsItalic = value; m_fontDataModified = true; }

	/** �t�H���g�̃C�^���b�N�̗L�����擾���܂��B*/
	bool IsFontItalic() const { return m_fontData.IsItalic; }

	/** �t�H���g�̃A���`�G�C���A�X�L����ݒ肵�܂��B*/
	void SetFontAntiAlias(bool value) { m_fontData.IsAntiAlias = value; m_fontDataModified = true; }

	/** �t�H���g�̃A���`�G�C���A�X�L�����擾���܂��B*/
	bool IsFontAntiAlias() const { return m_fontData.IsAntiAlias; }


	virtual void Render(IDocumentsRenderer* renderer);

protected:
	virtual void OnFontDataChanged(const FontData& newData);

	// ILayoutElement interface
	virtual SizeF MeasureOverride(const SizeF& constraint);
	virtual const PointF& GetLayoutPosition() const override;
	virtual const SizeF& GetLayoutSize() const override;
	virtual const ThicknessF& GetLayoutMargin() const override;
	virtual const ThicknessF& GetLayoutPadding() const override;
	virtual AlignmentAnchor GetLayoutAnchor() const override;
	virtual HorizontalAlignment GetLayoutHorizontalAlignment() const override;
	virtual VerticalAlignment GetLayoutVerticalAlignment() const override;
	virtual ILayoutElement* GetLayoutParent() const override;
	//virtual int GetLayoutChildCount() const override;
	//virtual ILayoutElement* GetLayoutChild(int index) const override;
	virtual VerticalAlignment* GetLayoutContentVerticalAlignment() override;
	virtual HorizontalAlignment* GetLayoutContentHorizontalAlignment() override;
	virtual const SizeF& GetLayoutDesiredSize() const override;
	virtual void SetLayoutDesiredSize(const SizeF& size) override;
	virtual void SetLayoutFinalLocalRect(const RectF& rect) override;

LN_INTERNAL_ACCESS:
	DocumentsManager* GetManager() const { return m_manager; }
	void SetParent(TextElement* parent) { m_parent = parent; }
	TextElement* GetParent() const { return m_parent; }

private:
	DocumentsManager*		m_manager;
	FontData				m_fontData;
	bool					m_fontDataModified;

	PointF					m_position;
	SizeF					m_size;
	ThicknessF				m_margin;
	ThicknessF				m_padding;
	AlignmentAnchor			m_anchor;
	HorizontalAlignment		m_horizontalAlignment;
	VerticalAlignment		m_verticalAlignment;
	TextElement*			m_parent;
	SizeF					m_desiredSize;
	RectF					m_finalLocalRect;
};

/**
	@brief	�R���e���c���O���[�v�����Ēi���ɂ��邽�߂Ɏg�p�����B
*/
class Paragraph
	: public TextElement
{
public:
	Paragraph();
	virtual ~Paragraph();
	void Initialize(DocumentsManager* manager);

	void AddInline(Inline* inl);
	void ClearInlines();

	virtual void Render(IDocumentsRenderer* renderer);

private:
	Array<RefPtr<Inline>>	m_inlines;
};

/**
	@brief
*/
class Inline
	: public TextElement
{
public:
	Inline();
	virtual ~Inline();
	void Initialize(DocumentsManager* manager);

private:
};

/**
	@brief
*/
class Run
	: public Inline
{
public:
	Run();
	virtual ~Run();
	void Initialize(DocumentsManager* manager);

	void SetText(const StringRef& text);

protected:
	// TextElement interface
	virtual void OnFontDataChanged(const FontData& newData) override;
	//virtual SizeF MeasureLayout() override;
	//virtual void ArrangeLayout(const RectF& finalLocalRect) override;
	virtual void Render(IDocumentsRenderer* renderer) override;

	// ILayoutElement interface
	virtual SizeF MeasureOverride(const SizeF& constraint);

private:
	GenericStringBuilderCore<UTF32>	m_text;
	RefPtr<GlyphRun>				m_glyphRun;
};

/**
	@brief
*/
class Span
	: public Inline
{
public:
	Span();
	virtual ~Span();
	void Initialize(DocumentsManager* manager);

private:
	// Inline List
};

} // namespace detail
LN_NAMESPACE_END