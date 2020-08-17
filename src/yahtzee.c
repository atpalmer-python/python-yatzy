#include <Python.h>
#include "yahtzee_roll.h"


typedef struct {
    PyObject_HEAD
} Scorecard;

static PyObject *_ensure_Roll(PyObject *arg) {
    if(Roll_CheckExact(arg))
        return arg;
    PyErr_SetString(PyExc_TypeError, "Argument must be Roll");
    return NULL;
}

static PyObject *Scorecard_score_as_ones(PyObject *self, PyObject *arg) {
    if(!_ensure_Roll(arg))
        return NULL;
    int result = 0;
    for(int i = 0; i < ROLL_NUM_DIE; ++i) {
        int value = ROLL_DIE_VALUE(arg, i);
        if(value == 1)
            result += value;
    }
    return PyLong_FromLong(result);
}

static PyMethodDef scorecard_methods[] = {
    {"score_as_ones", Scorecard_score_as_ones, METH_O},
    {0},
};

static PyTypeObject Scorecard_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Scorecard",
    .tp_doc = "",
    .tp_basicsize = sizeof(Scorecard),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_methods = scorecard_methods,
};

static struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "yahtzee",
    .m_doc = "",
    .m_size = 0,
    .m_methods = NULL,
};

PyMODINIT_FUNC PyInit_yahtzee(void) {
    if(PyType_Ready(&Roll_Type) < 0)
        return NULL;
    if(PyType_Ready(&Scorecard_Type) < 0)
        return NULL;

    PyObject *m = PyModule_Create(&module);

    Py_INCREF(&Roll_Type);
    PyModule_AddObject(m, "Roll", (PyObject *)&Roll_Type);

    Py_INCREF(&Scorecard_Type);
    PyModule_AddObject(m, "Scorecard", (PyObject *)&Scorecard_Type);

    srand(time(NULL));

    return m;
}

