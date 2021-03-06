﻿#include <TestConfig.h>
#include <Lumino/Threading/Semaphore.h>
#include <Lumino/Threading/Thread.h>

class IntegrateTest_Threading_Semaphore : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static int g_value = 0;

//---------------------------------------------------------------------
TEST_F(IntegrateTest_Threading_Semaphore, Basic)
{
	Semaphore sem(2, 2);

	ConditionFlag flag(false);
	DelegateThread thr1, thr2, thr3, thr4;
	
	g_value = 0;
	auto func = [&sem, &flag](){ sem.lock(); g_value++; flag.wait(); sem.unlock(); };

	thr1.start(Delegate<void()>(func));
	thr2.start(Delegate<void()>(func));
	thr3.start(Delegate<void()>(func));
	thr4.start(Delegate<void()>(func));

	Thread::sleep(100);

	// この時点で2つはlock()で止まっている
	ASSERT_EQ(2, g_value);

	// 先に flag.wait() で待っているものを動かす→unlock()にたどり着く
	flag.setTrue();

	Thread::sleep(100);
	ASSERT_EQ(4, g_value);
}
