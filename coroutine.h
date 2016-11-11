#ifndef COROUTINE_H
#define COROUTINE_H

#include <stdint.h>
#include <functional>
#include "ucontext.h"
#include "singleton.h"

#define COROUTINE_STACK_SIZE 1024 * 24  // At least 17 KB size of stack
#define COROUTINE_MAX_COUNT 32

typedef std::function<void(void)> CoroutineFunc;

class Coroutine
{
public:
	int status;
	ucontext_t cxt;
	CoroutineFunc func;

	char stack[COROUTINE_STACK_SIZE];
};

class CoroutineScheduler : public Singleton<CoroutineScheduler>
{
	APPLY_SINGLETON_WITH_NO_DEFUALT_CONSTRUCTOR(CoroutineScheduler)
public:
	~CoroutineScheduler();
	enum Status
	{
		CO_READY,
		CO_SUSPENDED,
		CO_RUNNING,
		CO_FINISHED
	};

	//ucontext_t* _coroutineContext;

	int _index;
	int _running;

	ucontext_t* _mainContext;
	Coroutine*  _id2routine[COROUTINE_MAX_COUNT];

public:
	static void Schedule();

	int  _DestroyCoroutine(int id);
	int  _CreateCoroutine(CoroutineFunc func);
	void _Yield();
	void _WaitCondition(const std::function<bool(void)>& _conditionFunc);
	void _ResumeCoroutine(int id);
	bool _IsCoroutineAlive(int id);
};

#undef  Yield
#define DestroyCoroutine(id)	CoroutineScheduler::GetInstance()->_DestroyCoroutine(id)
#define IsCoroutineAlive(id)	CoroutineScheduler::GetInstance()->_IsCoroutineAlive(id)
#define WaitCondition(func)		CoroutineScheduler::GetInstance()->_WaitCondition(func)
#define Yield()					CoroutineScheduler::GetInstance()->_Yield()
#define CreateCoroutine(func)	CoroutineScheduler::GetInstance()->_CreateCoroutine(func)
#define ResumeCoroutine(id)		CoroutineScheduler::GetInstance()->_ResumeCoroutine(id)
#define StartCoroutine(id)		CoroutineScheduler::GetInstance()->_ResumeCoroutine(id)

#endif