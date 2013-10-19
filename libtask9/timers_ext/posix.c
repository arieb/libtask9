#include <Python.h>
#include <sys/time.h>
#include <signal.h>

static PyObject *
libtask9_start_ticker(PyObject *self, PyObject *args)
{
	struct itimerval timerspec;
	int interval_seconds;
	int err;
	
	if(!PyArg_ParseTuple(args, "i", &interval_seconds))
		return NULL;
	timerspec.it_value.tv_sec  = interval_seconds;
	timerspec.it_value.tv_usec = 0;
	timerspec.it_interval = timerspec.it_value;
	err = setitimer(ITIMER_REAL, &timerspec, NULL);
	if(err != 0)
		return PyErr_SetFromErrno(PyExc_OSError);

	Py_RETURN_NONE;
}

static PyObject *
libtask9_wait_tick(PyObject *self, PyObject *args)
{
	sigset_t set;
	int signo;
	int err;

	sigemptyset(&set);
	sigaddset(&set, SIGALRM);

	Py_BEGIN_ALLOW_THREADS
	err = sigwait(&set, &signo);
	Py_END_ALLOW_THREADS

	if(err != 0) {
		char errmsg[128];

		sprintf(errmsg, "sigwait failed with error %d (%s)", err, strerror(err));
		PyErr_SetString(PyExc_OSError, errmsg);
		return NULL;
	}
	
	return PyInt_FromLong(1);
}

#include "common.c"
