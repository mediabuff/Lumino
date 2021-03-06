﻿
#pragma once
#include <list>
#include <Lumino/Threading/Thread.h>
#include "../MidiDecoder.h"
#include "lndmusic.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_AUDIO_BEGIN
namespace detail
{

class DirectMusicManager;

/**
	@brief      DirectMusic 上での再生シーケンスひとつ分を扱うクラス

	@details	音声ひとつ分を再生するために必要な一連のインターフェースを扱うクラスです。
				(Performance、Segment 等をまとめたもの)

	@note		DirectMusic はパフォーマンスを作成する前にセグメントを作成することができる。
				スレッドで DirectMusic を初期化する場合、初期化前にセグメントを作成しておけば時間の節約とかになるかもしれないけど、
				セグメントをパフォーマンスよりも先に作成すると不具合が起こる。(全部ピアノ音源になるとか)
*/
class DirectMusicSegment
	: public RefObject
{
public:
	/// (midiStream は保持しないため、参照カウントは変化させません。ただし、コンストラクタに渡す前に fillBuffer() を呼んでおく必要があります)
	DirectMusicSegment(DirectMusicManager* manager, IDirectMusicPerformance8* dmPerformance, MidiDecoder* midiStream);
	virtual ~DirectMusicSegment();

public:

	/// 音量の設定 (0.0～1.0)
	void setVolume(float volume);

	/// ピッチの設定 (0.5 ～ 2.0 が基本。範囲外も一応可。)
	void setPitch(float pitch);

	/// 全体の再生時間の取得 (ミュージックタイム単位)
    uint32_t getTotalTime() const;

	/// 再生位置の取得 (ミュージックタイム単位)
	uint32_t getPlayPosition() const;

	/// 再生位置の設定 (ミュージックタイム単位)
	void setPlayPosition(uint32_t time);

	/// 再生中かを判定 (音が鳴っているか)
    bool isPlaying() const;

	/// ループ情報の設定
	///		begin と length はミュージックタイム単位。
	///		 length が 0 の場合は begin の位置から終端まで再生する。
	void setLoopState( bool isLoop, uint32_t begin, uint32_t length );

	/// 再生
    void play();

	/// 停止
    void stop();

private:
    IDirectMusicPerformance8*   m_dmPerformance;
    IDirectMusicAudioPath*      m_dmAudioPath;
    IDirectMusicSegment8*		m_dmSegment;
	IDirectMusicSegmentState8*	m_dmSegmentState;
    IDirectSoundBuffer8*        m_dsSoundBuffer;
};


/**
	@brief      DirectMusic 上での再生シーケンスひとつ分を扱うクラス
	@details	IDirectMusic8 インターフェイスはひとつのアプリケーションにつき
				ひとつしか作成できないため、このクラスはシングルトンで実装。
				http://msdn.microsoft.com/ja-jp/library/cc353890.aspx
*/
class DirectMusicManager
{
public:
	static const int MusicTimeBase = 768;

public:

	/// 渡す初期化データ
    struct ConfigData
    {
        HWND				WindowHandle;
		DirectMusicMode		DMInitMode;
		float				ReverbLevel;
    };

	/// 再生要求を持つクラスの基底
    class PlayerObject
    {
    public:
        PlayerObject(){}
        virtual ~PlayerObject();

        /// 再生要求リストに追加されている状態で DirectMusic の初期化が完了したときに呼ばれる
        virtual void onFinishDMInit( IDirectMusicPerformance8* dm_performance_ ) = 0;
    };

public:

	/// 初期化
	///		DMInitMode が LN_DMINITMODE_NOTUSE の場合はインスタンスを作成しない。getInstance() は NULL を返す。
	static void initialize( const ConfigData& configData );

	/// 終了処理
	static void dispose();

	///インスタンスへのポインタの取得
    static DirectMusicManager* getInstance() { return m_instance; }

public:

	/// 初期化が完了しているかを判定する
	bool isInitialized() const { return m_performanceInited.isTrue(); }

	/// エラー状態の取得 (エラー番号(内部用)。 正常な場合は 0)
    uint32_t getErrorState();

	float getReverbLevel() const { return m_reverbLevel; }

	/// 再生要求の追加 (既に同じオブジェクトが追加されている場合はなにもしません。)
	void addPlayRequest( PlayerObject* obj );

	///  再生要求の削除
	///		DirectMusic の初期化中にオーディオプレイヤーが解放された
	///		時に、この関数でリストから外しますが、
	///		PlayerObject クラスのデストラクタで呼ばれるため
	///		通常は呼び出す必要はありません。
	void removePlayRequest( PlayerObject* obj );

	/// IDirectMusicPerformance8 の作成
	///		isInitialized() によって初期化の完了を確認してから呼び出してください。
    IDirectMusicPerformance8* createDMPerformance();

	/// 再生要求リストをチェックして、可能なら再生する
    void polling();

private:
    DirectMusicManager();
    ~DirectMusicManager();

	/// 初期化
    void internalInitialize( const ConfigData& configData );

	/// ひとつめのパフォーマンスの初期化 ( 別スレッドで初期化する場合はそこから呼ばれる )
	void initPerformance();

	/// (参照カウントなどに変化はなし)
	//IDirectMusicPerformance8* InternalCreateDMPerformance();

	/// 最初の IDirectMusicPerformance8 の初期化スレッド
	void thread_InitPerformance();

private:

	typedef List<PlayerObject*> PlayRequestList;

    HWND						m_windowHandle;
	DirectMusicMode				m_initMode;
	IDirectSound8*				m_directSound;
	IDirectMusic8*				m_directMusic;
	IDirectMusicPerformance8*	m_firstPerformance;	///< 初期化スレッドによって作成された IDirectMusicPerformance8

	DelegateThread				m_initThread;		///< 初期化スレッド
    ConditionFlag				m_performanceInited;     ///< 初期化済みフラグ
    Mutex						m_mutex;
    Mutex						m_listLock;

    PlayRequestList				m_playRequestList;
    uint32_t					mErrorState;
	float						m_reverbLevel;

	static DirectMusicManager*	m_instance;
};

} // namespace detail
LN_NAMESPACE_AUDIO_END
LN_NAMESPACE_END
