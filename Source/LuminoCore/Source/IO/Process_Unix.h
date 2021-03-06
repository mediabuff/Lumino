﻿#include "../Internal.h"
#include <sys/types.h> 
#include <sys/wait.h>
#include <Lumino/Base/ElapsedTimer.h>
#include <Lumino/IO/Process.h>

#if defined(LN_OS_MAC)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char **environ;
#endif

LN_NAMESPACE_BEGIN
namespace detail {
	
//==============================================================================
// ProcessImpl
//==============================================================================
class ProcessImpl
{
public:
	ProcessImpl();
	~ProcessImpl();
	
	void start(const ProcessStartInfo& startInfo, ProcessStartResult* outResult);
	bool waitForExit(int timeoutMSec);
	ProcessStatus getState();
	int getExitCode();
	void TryGetExitCode();
	void dispose();
	
private:
	pid_t   m_pid;
	//InternalPipeStream*		m_stdinPipeStream;
	//InternalPipeStream*		m_stdoutPipeStream;
	//InternalPipeStream*		m_stderrPipeStream;
	int						m_exitCode;
	bool					m_crashed;
	bool					m_disposed;
};
	
	
//------------------------------------------------------------------------------
ProcessImpl::ProcessImpl()
	: m_pid(0)
{
}

//------------------------------------------------------------------------------
ProcessImpl::~ProcessImpl()
{
}

//------------------------------------------------------------------------------
void ProcessImpl::start(const ProcessStartInfo& startInfo, ProcessStartResult* outResult)
{
	pid_t pid = fork();
	if (pid == 0)
	{
		// 子プロセス側はこの if に入る
		
		std::string program = startInfo.program.getString().toStdString();
		
		auto argList = startInfo.args.split(_LT(" "));
		
		char** argv = new char *[argList.getCount() + 2];
		argv[0] = ::strdup(program.c_str());     // 書き込み可能なポインタを渡さなければならないので strdup
		for (int i = 0; i < argList.getCount(); ++i)
		{
			argv[i + 1] = ::strdup(argList[i].toStdString().c_str());
		}
		argv[argList.getCount() + 1] = NULL;
		
		execve(argv[0], argv, environ);
		
		// ここには execve が失敗したときだけ来る。
	}
	else
	{
		// 親プロセス側
	}

}

//------------------------------------------------------------------------------
bool ProcessImpl::waitForExit(int timeoutMSec)
{
	ElapsedTimer timer;
	bool exit = false;
	do
	{
		int status;
		pid_t pid = waitpid(m_pid, &status, WNOHANG);	// WNOHANG: 状態変化が起こった子プロセスがない場合にすぐに復帰する。
		LN_THROW(pid >= 0, IOException);
		if (pid == 0) {
			// 状態変化していなかった。実行中。
		}
		else
		{
			if (WIFEXITED(status))
			{
				// 子プロセスが正常終了の場合
				m_exitCode = WEXITSTATUS(status); //子プロセスの終了コード
				exit = true;
			}
			else
			{
				// 子プロセスが異常終了の場合
			}
		}
		
		Thread::sleep(1);
		
	} while(timeoutMSec == -1 || timer.getElapsed() < timeoutMSec);
	
	if (exit)
	{
		// いろいろ閉じる
		dispose();
	}
	else {
		// タイムアウトした
	}
	return true;
}

//------------------------------------------------------------------------------
ProcessStatus ProcessImpl::getState()
{
	LN_NOTIMPLEMENTED();
	return ProcessStatus::Finished;
}

//------------------------------------------------------------------------------
int ProcessImpl::getExitCode()
{
	return m_exitCode;
}

//------------------------------------------------------------------------------
void ProcessImpl::TryGetExitCode()
{
}

//------------------------------------------------------------------------------
void ProcessImpl::dispose()
{
	if (!m_disposed)
	{
		m_disposed = true;
	}
}


} // namespace detail



#if 0
//=============================================================================
// Process
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void process::start(const PathName& program, const String& args)
{}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool process::WaitForExit(int timeoutMSec)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ProcessStatus process::GetState()
{
	// ※ http://linuxjm.osdn.jp/html/LDP_man-pages/man2/wait.2.html には wait を実行しないとゾンビ状態になる、とあるが、
	//    mono のソースを見る限りだと waitpid でも大丈夫なようだ。（TODO: 要動作確認）
	//	  wait は終了処理で実行しておく。
	
	int status;
	pid_t pid = waitpid(m_pid, &status, WNOHANG);	// WNOHANG: 状態変化が起こった子プロセスがない場合にすぐに復帰する。
	if (pid == 0) {
		// 状態変化していなかった。実行中。
		return ProcessStatus::Running;
	}
	
	// 正常終了
	if (WIFEXITED(status)) {
		// 終了コード
		m_exitCode = WEXITSTATUS(status);
		return ProcessStatus::Finished;
	}
	// 異常終了
	else {
		return ProcessStatus::Crashed;
	}
	//waitid
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void process::TryGetExitCode()
{
	//GetState();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void process::dispose()
{
	if (!m_disposed)
	{
		m_disposed = true;
	}
}
//#endif
#endif

LN_NAMESPACE_END

