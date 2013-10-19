static
PyObject* libtask9_mask_sigvtalrm(PyObject *self, PyObject *args)
{
	sigset_t mask;
	int err;

	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	err = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	if(err != 0) {
		char errmsg[128];

		sprintf(errmsg, "pthread_sigmask failed with error %d (%s)", err, strerror(err));
		PyErr_SetString(PyExc_OSError, errmsg);
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyMethodDef libtask9_timer_methods[] = {
	{"mask_sigvtalrm", libtask9_mask_sigvtalrm, METH_VARARGS},
	{"start_ticker", libtask9_start_ticker, METH_VARARGS},
	{"wait_tick", libtask9_wait_tick, METH_VARARGS},
	{NULL, NULL, 0, NULL}	/* Sentinel */
};

PyMODINIT_FUNC
init_libtask9_timers(void)
{
	Py_InitModule("_libtask9_timers", libtask9_timer_methods);
}
