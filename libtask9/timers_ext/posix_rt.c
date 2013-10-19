#include <Python.h>
#include <pthread.h>
#include <time.h> /* before signal.h on FreeBSD for timer_create/timer_settime/timer_delete*/
#include <signal.h>
#include <stdio.h>

static const char TID_CAPSULE[] = "_libtask9_timers.tid";

static void
libtask9_ticker_dtor(PyObject *obj)
{
	timer_t tid;

	tid = (timer_t)PyCapsule_GetPointer(obj, TID_CAPSULE);
	timer_delete(tid);
}

static PyObject *
libtask9_start_ticker(PyObject *self, PyObject *args)
{
	timer_t tid;
	struct sigevent sev;
	struct itimerspec timerspec;
	int interval_seconds;
	int err;

	memset(&sev, 0, sizeof sev);
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = /*SIGVTALRM*/ SIGALRM;

	if(!PyArg_ParseTuple(args, "i", &interval_seconds))
		return NULL;

	err = timer_create(CLOCK_MONOTONIC, &sev, &tid);
	if(err != 0)
		return PyErr_SetFromErrno(PyExc_OSError);

	timerspec.it_value.tv_sec  = interval_seconds;
	timerspec.it_value.tv_nsec = 0;
	timerspec.it_interval = timerspec.it_value;
	err = timer_settime(tid, 0, &timerspec, NULL);
	if(err != 0)
		return PyErr_SetFromErrno(PyExc_OSError);

	return PyCapsule_New((void*)tid, TID_CAPSULE, libtask9_ticker_dtor);
}

static const int debug_restarts = 0;

static PyObject *
libtask9_wait_tick(PyObject *self, PyObject *args)
{
	siginfo_t siginfo;
	sigset_t set;
	struct timespec start, finish;
	int interrupted;
	int err;

	sigemptyset(&set);
	sigaddset(&set, /*SIGVTALRM*/SIGALRM);

	Py_BEGIN_ALLOW_THREADS

if(debug_restarts)
	clock_gettime(CLOCK_MONOTONIC, &start);

	for(interrupted=0;;) {
		err = sigwaitinfo(&set, &siginfo);
		if((err == -1) && (errno == EINTR)) {

if(debug_restarts)
	interrupted = 1;

			continue;
		}
		break;
	}

if(debug_restarts)
	clock_gettime(CLOCK_MONOTONIC, &finish);

	Py_END_ALLOW_THREADS

	if(err != /*SIGVTALRM*/ SIGALRM)
		return PyErr_SetFromErrno(PyExc_OSError);

if(debug_restarts) {
	if(interrupted)
		fprintf(stderr, "interrupted!\n");
	fprintf(stderr, "start: %ld %ld, finish: %ld %ld\n",
		start.tv_sec, start.tv_nsec, finish.tv_sec, finish.tv_nsec);
}

	return PyInt_FromLong(1+siginfo.si_overrun);
}

#include "common.c"
