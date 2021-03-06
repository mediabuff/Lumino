﻿
#pragma once
#include <Lumino/Graphics/RenderState.h>
#include <Lumino/Graphics/Common.h>
#include <Lumino/Graphics/Color.h>
#include <Lumino/Graphics/Graphics.h>

LN_NAMESPACE_BEGIN
class EngineDiagCore;
class RawBitmap;
class PlatformWindow;

LN_NAMESPACE_GRAPHICS_BEGIN

namespace Driver
{
static const int MaxVertexStreams = 16;

class ISwapChain;
class IRenderer;
class IVertexDeclaration;
class IVertexBuffer;
class IIndexBuffer;
class ITexture;
class IShader;
class IShaderVariable;
class IShaderTechnique;
class IShaderPass;

/// デバイスの状態
enum DeviceState
{
	DeviceState_Enabled = 0,		///< 描画可能
	DeviceState_Lost,				///< 消失中。デバイスをリセットする必要がある。
	DeviceState_Pausing,			///< 
};

/// テクスチャサーフェイスの種類
enum TextureType
{
	TextureType_Normal = 0,					///< カラーテクスチャ
	TextureType_RenderTarget,				///< レンダリングターゲット
	TextureType_DepthBuffer,				///< 深度バッファ
	TextureType_BackBuffer,					///< バックバッファレンダリングターゲット
	TextureType_Max,						///< (terminator)
};



/// @brief	デバイスによって生成されるオブジェクト
class IDeviceObject
	: public RefObject
{
public:
	/// デバイス消失時の処理
	virtual void onLostDevice() = 0;

	/// デバイス復帰時の処理
	virtual void onResetDevice() = 0;

protected:
	virtual ~IDeviceObject() {}
};

/// グラフィックスデバイスのリソース管理を行う
class IGraphicsDevice
	: public RefObject
{
public:

	/// 終了処理
	virtual void dispose() = 0;
	
	/// グラフィックスデバイスをライブラリ内部で生成したかを確認する
	virtual bool isStandalone() const = 0;

	/// API 種類の取得
	virtual GraphicsAPI getGraphicsAPI() const = 0;

	/// デフォルトのスワップチェインの取得
	virtual ISwapChain* getDefaultSwapChain() = 0;
	
	/// 描画インターフェイスの取得
	/// 保持してはなりません。
	virtual IRenderer* getRenderer() = 0;

	virtual IVertexDeclaration* createVertexDeclaration(const VertexElement* elements, int elementsCount) = 0;

	/// 頂点バッファの作成
	virtual IVertexBuffer* createVertexBuffer(size_t bufferSize, const void* data, ResourceUsage usage) = 0;

	/// インデックスバッファの作成 (initialData を受け取るようにすれば、後で lock 用にメモリ確保する必要がなくなる)
	virtual IIndexBuffer* createIndexBuffer(int indexCount, const void* initialData, IndexBufferFormat format, ResourceUsage usage) = 0;

	/// テクスチャの作成
	/// initialData : 初期データまたは NULL
	virtual ITexture* createTexture(const SizeI& size, bool mipmap, TextureFormat format, const void* initialData) = 0;

	/// テクスチャの作成 (環境依存の画像ファイル読み込み)
	virtual ITexture* createTexturePlatformLoading(Stream* stream, bool mipmap, TextureFormat format) = 0;

	virtual ITexture* createTexture3D(int width, int height, int depth, uint32_t mipLevels, TextureFormat format, ResourceUsage usage, const void* initialData) = 0;

	/// レンダーターゲットテクスチャの作成
	virtual ITexture* createRenderTarget(uint32_t width, uint32_t height, uint32_t mipLevels, TextureFormat format) = 0;

	/// 深度バッファの作成
	virtual ITexture* createDepthBuffer(uint32_t width, uint32_t height, TextureFormat format) = 0;

	/// シェーダの作成 (エラーの場合、インスタンスは作成されない)
	virtual IShader* createShader(const void* textData, size_t size, ShaderCompileResult* result) = 0;

	/// スワップチェインの作成
	virtual ISwapChain* createSwapChain(PlatformWindow* window) = 0;

	/// デバイスの状態を取得する
	virtual DeviceState getDeviceState() = 0;

	/// デバイスをリセットする
	virtual void resetDevice() = 0;

	/// デバイスを停止する
	virtual void onLostDevice() = 0;

	/// デバイスを再開する
	virtual void onResetDevice() = 0;

	/// 呼び出したスレッドを描画スレッドとして使用できるようにする (描画スレッドを使う場合のみ使用する)
	virtual void attachRenderingThread() = 0;

	/// 描画スレッドの終了処理
	virtual void detachRenderingThread() = 0;


	virtual void flushResource() = 0;

protected:
	virtual ~IGraphicsDevice() {}
};

/// スワップチェインのインターフェイス
class ISwapChain
	: public IDeviceObject
{
public:

	/// バックバッファを示すレンダリングターゲット
	virtual ITexture* getBackBuffer() = 0;

	/// バックバッファサイズを変更する
	virtual void resize(const SizeI& size) = 0;

	/// バックバッファの内容をウィンドウへ転送する
	///		colorBuffer を本物のバックバッファへ一度コピーしてからウィンドウに Swap する。
	///		なぜこうしているのかというと、DX9 や OpenGL がデフォルトで作成するバックバッファは細かいフォーマットの制御が効かないため。
	///		さらに、OpenGL はバックバッファを Renderer::setRenderTarget したいときはテクスチャ番号 0 をフレームバッファにアタッチする必要がある。
	///		これはカレントコンテキストに依存するため、複数ウィンドウへの描画を行いたいときに余計な制限をユーザに課すことになる。
	///		TODO: でも、今は colorBuffer は Graphics::Texture に持っているけど、Driver::SwapChain に持たせてもいいかも。
	virtual void present(ITexture* colorBuffer) = 0;

protected:
	virtual ~ISwapChain() {}
};

/// 描画とそのステート管理を行うインターフェイス
class IRenderer
	: public RefObject
{
public:


	void enterRenderState();
	void leaveRenderState();

	void begin();
	void end();


	void setRenderTarget(int index, ITexture* target);
	ITexture* getRenderTarget(int index);
	void setDepthBuffer(ITexture* buffer);
	ITexture* getDepthBuffer();

	void setRenderState(const RenderState& state) { m_requestedRenderState = state; }
	const RenderState& getRenderState() { return m_requestedRenderState; }
	virtual void setDepthStencilState(const DepthStencilState& state) { m_requestedDepthStencilState = state; }
	const DepthStencilState& getDepthStencilState() { return m_requestedDepthStencilState; }


	void setViewport(const RectI& rect);


	void setVertexDeclaration(IVertexDeclaration* vertexDeclaration);
	IVertexDeclaration* getVertexDeclaration() const;

	/// 頂点バッファの設定
	void setVertexBuffer(int streamIndex, IVertexBuffer* vertexBuffer);
	IVertexBuffer* getVertexBuffer(int streamIndex);

	/// インデックスバッファの設定
	void setIndexBuffer(IIndexBuffer* indexBuffer);

	void setShaderPass(IShaderPass* pass);

	IShaderPass* getShaderPass() const;


	/// 設定されている各種バッファをクリアする
	void clear(ClearFlags flags, const Color& color, float z = 1.0f, uint8_t stencil = 0x00);

	/// プリミティブ描画
	void drawPrimitive(PrimitiveType primitive, int startVertex, int primitiveCount);

	/// プリミティブ描画 (インデックス付き。頂点、インデックスの両方のバッファのdynamic、static が一致している必要がある)
	void drawPrimitiveIndexed(PrimitiveType primitive, int startIndex, int primitiveCount);

	void setDiag(EngineDiagCore* diag) { m_diag = diag; }

protected:
	IRenderer();
	virtual ~IRenderer();

	virtual void onEnterRenderState() = 0;
	virtual void onLeaveRenderState() = 0;
	virtual void onBeginRendering() = 0;
	virtual void onEndRendering() = 0;
	virtual void onUpdateFrameBuffers(ITexture** renderTargets, int renderTargetsCount, ITexture* depthBuffer) = 0;
	virtual	void onUpdateRenderState(const RenderState& newState, const RenderState& oldState, bool reset) = 0;
	virtual	void onUpdateDepthStencilState(const DepthStencilState& newState, const DepthStencilState& oldState, bool reset) = 0;
	virtual void onUpdatePrimitiveData(IVertexDeclaration* decls, const List<Ref<IVertexBuffer>>& vertexBuufers, IIndexBuffer* indexBuffer) = 0;
	virtual void onUpdateViewport(const RectI& viewport) = 0;
	virtual void onClear(ClearFlags flags, const Color& color, float z, uint8_t stencil) = 0;
	virtual void onDrawPrimitive(PrimitiveType primitive, int startVertex, int primitiveCount) = 0;
	virtual void onDrawPrimitiveIndexed(PrimitiveType primitive, int startIndex, int primitiveCount) = 0;

private:
	void flushStates();

	enum ModifiedFlags
	{
		Modified_None				= 0x0000,
		Modified_VertexDeclaration	= 0x0001,
		Modified_VertexBuffer		= 0x0002,
		Modified_IndexBuffer		= 0x0004,
		Modified_FrameBuffer		= 0x0008,
		Modified_Viewport			= 0x0010,
		Modified_All				= 0xFFFFFFFF,
	};

	EngineDiagCore*					m_diag;
	uint32_t						m_modifiedFlags;

	Ref<ITexture>					m_currentRenderTargets[Graphics::MaxMultiRenderTargets];
	Ref<ITexture>					m_currentDepthBuffer;
	RenderState						m_requestedRenderState;
	RenderState						m_currentRenderState;
	DepthStencilState				m_requestedDepthStencilState;
	DepthStencilState				m_currentDepthStencilState;
	RectI							m_viewportRect;
	Ref<IVertexDeclaration>			m_currentVertexDeclaration;
	List<Ref<IVertexBuffer>>		m_currentVertexBuffers;
	Ref<IIndexBuffer>				m_currentIndexBuffer;
	Ref<IShaderPass>				m_currentShaderPass;
	bool							m_rendering;
};

/// 頂点宣言
class IVertexDeclaration
	: public IDeviceObject
{
protected:
	virtual ~IVertexDeclaration() = default;
};

/// 頂点バッファのインターフェイス
class IVertexBuffer
	: public IDeviceObject
{
public:

	/// 頂点データのバイト数の取得
	virtual size_t getByteCount() const = 0;

	///// 頂点バッファリソースの使用方法の取得
	//virtual DeviceResourceUsage getUsage() const = 0;

	/// データを転送する
	virtual void setSubData(uint32_t offsetBytes, const void* data, uint32_t dataBytes) = 0;

	/// ロック
	virtual void* lock() = 0;

	/// アンロック
	virtual void unlock() = 0;

protected:
	virtual ~IVertexBuffer() {};
};

/// インデックスバッファのインターフェイス
class IIndexBuffer
	: public IDeviceObject
{
public:
	virtual size_t getByteCount() const = 0;

	/// インデックスバッファのフォーマットの取得　
	virtual IndexBufferFormat getFormat() const = 0;

	/// リソースの使用方法の取得
	virtual ResourceUsage getUsage() const = 0;

	/// データを転送する
	virtual void setSubData(uint32_t offsetBytes, const void* data, uint32_t dataBytes) = 0;

	/// ロック
	virtual void lock(void** lockedBuffer, size_t* lockedSize) = 0;

	/// アンロック
	virtual void unlock() = 0;

protected:
	virtual ~IIndexBuffer() {};
};

/// テクスチャのインターフェイス
class ITexture
	: public IDeviceObject
{
public:

	//void Clear(const Color& color);

	void setSubData(const PointI& point, RawBitmap* bitmap);



	/// テクスチャの種類の取得
	virtual TextureType getTextureType() const = 0;

	/// サーフェイスフォーマットの取得
	virtual TextureFormat getTextureFormat() const = 0;

	/// サイズの取得
    virtual const SizeI& getSize() const = 0;

	/// 実際のサイズの取得 (デバイス依存により2のべき乗倍に拡張されたサイズ)
	virtual const SizeI& getRealSize() const = 0;

	/// サンプラステートの設定
	virtual void setSamplerState(const SamplerState& state) = 0;

	/// データ転送 (TODO:部分更新は未実装…)
	// data に渡されるイメージデータは上下が反転している状態。
	virtual void setSubData(const PointI& point, const void* data, size_t dataBytes, const SizeI& dataBitmapSize) = 0;

	virtual void setSubData3D(const Box32& box, const void* data, size_t dataBytes) = 0;

	// (得られるデータは上下反転)
	virtual void getData(const RectI& rect, void* outData) = 0;

	// 得られるデータは上下反転していない。レイアウトは RGBA
	virtual void readData(void* outData) {}


	///// ロック (バッファは上下反転)
	//virtual RawBitmap* lock() = 0;

	///// アンロック
	//virtual void unlock() = 0;

protected:
	virtual ~ITexture();

public:
	//class ScopedLock
	//{
	//public:
	//	ScopedLock(ITexture* obj)
	//	{
	//		m_obj = obj;
	//		m_bitmap = m_obj->lock();
	//	}
	//	~ScopedLock()
	//	{
	//		m_obj->unlock();
	//	}
	//	RawBitmap* GetBitmap()
	//	{
	//		return m_bitmap;
	//	}
	//private:
	//	ITexture*			m_obj;
	//	RawBitmap*	m_bitmap;
	//};
};

//class ITexture2D
//	: public ITexture
//{
//public:
//
//protected:
//	virtual ~ITexture2D();
//};
//
//class ITexture3D
//	: public ITexture
//{
//public:
//
//protected:
//	virtual ~ITexture3D();
//};

/// シェーダプログラムのインターフェイス
class IShader
	: public IDeviceObject
{
public:

	/// 変数の数を取得する (GL は uniform 変数)
	virtual int getVariableCount() const = 0;

	/// 変数を取得する
	virtual IShaderVariable* getVariable(int index) const = 0;

	/// テクニックの数を取得する
	virtual int getTechniqueCount() const = 0;

	/// テクニックを取得する
	virtual IShaderTechnique* getTechnique(int index) const = 0;

public:

	/// 名前を指定して変数を取得する (見つからなければ NULL を返す)
	IShaderVariable* getVariableByName(const Char* name) const;

protected:
	virtual ~IShader() {}
};

/// シェーダプログラム内の変数のインターフェイス
/// (値の Get 系関数は、シェーダが持つメタ情報を取得するために使用する。
/// つまり、初期値の取得目的でしか使用できない)
class IShaderVariable
	: public RefObject
{
public:

	/// 変数の型の取得
	virtual ShaderVariableType getType() const = 0;

	/// 変数名の取得
	virtual const String& getName() const = 0;

	/// セマンティクス名の取得 (無い場合は NULL)
	virtual const String& getSemanticName() const = 0;

	/// 行列型の場合の行の数を取得する
	virtual int getMatrixRows() const = 0;

	/// 行列型の場合の列の数を取得する
	virtual int getMatrixColumns() const = 0;

	/// 配列型の場合の要素数を取得する (0 の場合は配列ではない)
	virtual int getArrayElements() const = 0;

	/// ブール値の設定
	virtual void setBool(bool value) = 0;

	/// ブール値の取得
	//virtual bool GetBool() = 0;

	virtual void setBoolArray(const bool* values, int count) = 0;

	/// 整数値の設定
	virtual void setInt(int value) = 0;

	/// 整数値の取得
	//virtual int GetInt() = 0;

	/// 実数値の設定
	virtual void setFloat(float value) = 0;

	/// 実数値の取得
	//virtual float GetFloat() = 0;

	virtual void setFloatArray(const float* values, int count) = 0;

	/// ベクトルの設定
	virtual void setVector(const Vector4& vec) = 0;

	/// ベクトルの取得
	//virtual const Vector4& getVector() = 0;

	/// ベクトル配列の設定
	virtual void setVectorArray(const Vector4* vectors, int count) = 0;

	/// ベクトル配列の取得
	//virtual const Vector4* getVectorArray() = 0;

	/// 行列の設定
	virtual void setMatrix(const Matrix& matrix) = 0;

	/// 行列の取得
	//virtual const Matrix& getMatrix() = 0;

	/// 行列の配列の設定
	virtual void setMatrixArray(const Matrix* matrices, int count) = 0;

	/// 行列の配列の取得
	//virtual const Matrix* getMatrixArray() = 0;

	/// テクスチャの設定
	virtual void setTexture(ITexture* texture) = 0;

	/// テクスチャの設定
	//virtual ITexture* getTexture() = 0;

	/// 文字列の取得
	//virtual const Char* GetString() = 0;

	virtual const ShaderValue& getValue() const = 0;

	/// アノテーションの数を取得する
	virtual int getAnnotationCount() = 0;

	/// アノテーションを取得する
	virtual IShaderVariable* getAnnotation(int index) = 0;

protected:
	virtual ~IShaderVariable() {}
};

/// シェーダプログラム内のテクニックのインターフェイス
class IShaderTechnique
	: public RefObject
{
public:

	/// テクニックの名前を取得する
	virtual const Char* getName() const = 0;

	/// テクニック内のパスの数を取得する
	virtual int getPassCount() const = 0;

	/// パスを取得する
	virtual IShaderPass* getPass(int index) = 0;

	/// アノテーションの数を取得する
	virtual int getAnnotationCount() = 0;

	/// アノテーションを取得する
	virtual IShaderVariable* getAnnotation(int index) = 0;

protected:
	virtual ~IShaderTechnique() {}
};

/// シェーダプログラム内のパスのインターフェイス
class IShaderPass
	: public RefObject
{
public:

	virtual IShader* getShader() const = 0;

	/// パスの名前を取得する
	virtual const Char* getName() const = 0;

	/// アノテーションの数を取得する
	virtual int getAnnotationCount() = 0;

	/// アノテーションを取得する
	virtual IShaderVariable* getAnnotation(int index) = 0;

protected:
	friend class IRenderer;
	/// パスを適用する (CommitChanges するときも再コール)
	virtual void apply() = 0;

	/// パスの適用を終了する
	//virtual void end() = 0;

protected:
	virtual ~IShaderPass() {}
};

} // namespace Driver
LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END
