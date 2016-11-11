#include "coroutine.h"

#include <stdlib.h>
#include <assert.h>
#include "ucontext.h"

CoroutineScheduler::CoroutineScheduler() :_index(0), _running(-1)
{
	//_coroutineContext = new ucontext_t;
	_mainContext = new ucontext_t;

	for (size_t i = 0; i < COROUTINE_MAX_COUNT; i++)
		_id2routine[i] = nullptr;
}
CoroutineScheduler::~CoroutineScheduler() { if (_mainContext) { delete _mainContext; _mainContext = nullptr; } }

void CoroutineScheduler::Schedule()
{
	Coroutine* cor = GetInstance()->_id2routine[GetInstance()->_running];

	cor->func();

	GetInstance()->_running = -1;
	cor->status = CoroutineScheduler::CO_FINISHED;
	swapcontext(&cor->cxt, GetInstance()->_mainContext);
}


int CoroutineScheduler::_DestroyCoroutine(int id)
{
	Coroutine* cor = _id2routine[id];
	if (!cor) return -1;

	delete cor;
	_id2routine[id] = nullptr;
	return id;
}

int CoroutineScheduler::_CreateCoroutine(CoroutineFunc func)
{
	for (size_t i = 0; i < COROUTINE_MAX_COUNT; i++)
	{
		if (_id2routine[(_index + i) % COROUTINE_MAX_COUNT] == nullptr)
		{
			Coroutine* cor = new Coroutine();

			if (cor == nullptr) return -1;

			cor->func = func;
			cor->status = CoroutineScheduler::CO_READY;


			int index = (i + _index++) % COROUTINE_MAX_COUNT;
			index %= COROUTINE_MAX_COUNT;
			_id2routine[index] = cor;
			return index;
		}
	}
	return -1;
}

void CoroutineScheduler::_Yield()
{
	if (_running < 0) return;

	int cur = _running;
	_running = -1;

	Coroutine* cor = _id2routine[cur];

	cor->status = CoroutineScheduler::CO_SUSPENDED;

	swapcontext(&cor->cxt, _mainContext);
}
void CoroutineScheduler::_WaitCondition(const std::function<bool(void)>& _conditionFunc)
{
	while (!_conditionFunc())
		_Yield();
}
void CoroutineScheduler::_ResumeCoroutine(int id)
{
	Coroutine* cor = _id2routine[id];
	if (cor == nullptr || cor->status == CoroutineScheduler::CO_RUNNING)
		return;

	switch (cor->status)
	{
	case CoroutineScheduler::CO_READY:
	{
		getcontext(&cor->cxt);

		cor->status = CoroutineScheduler::CO_RUNNING;
		cor->cxt.uc_stack.ss_sp = cor->stack;
		cor->cxt.uc_stack.ss_size = COROUTINE_STACK_SIZE;
		// sucessor context.
		cor->cxt.uc_link = _mainContext;

		_running = id;
		// setup coroutine context
		makecontext(&cor->cxt, (void(*)())(Schedule), 0);
		swapcontext(_mainContext, &cor->cxt);
	}
	break;
	case CoroutineScheduler::CO_SUSPENDED:
	{
		_running = id;
		cor->status = CoroutineScheduler::CO_RUNNING;

		swapcontext(_mainContext, &cor->cxt);
	}
	break;
	default:
		break;
	}


	if (_running == -1 && cor->status == CoroutineScheduler::CO_FINISHED)
		_DestroyCoroutine(id);
}

bool CoroutineScheduler::_IsCoroutineAlive(int id)
{
	return _id2routine[id] != nullptr;
}