#include <Python.h>
#include "yahtzee_roll.h"


typedef struct {
    PyObject_HEAD
    /* sign-bit on for unset */
    int ones;
    int twos;
} Scorecard;

#define SCORECARD(self)                 ((Scorecard *)self)
#define SCORECARD_FIELD(self, f)        (SCORECARD(self)->f)
#define SCORECARD_HAS_VAL(self, f)      (SCORECARD_FIELD(self, f) >= 0)
#define SCORECARD_VAL(self, f)          (SCORECARD_HAS_VAL(self, f) ? SCORECARD_FIELD(self, f) : 0)

static PyObject *_ensure_Roll(PyObject *arg) {
    if(Roll_CheckExact(arg))
        return arg;
    PyErr_SetString(PyExc_TypeError, "Argument must be Roll");
    return NULL;
}

static PyObject *Scorecard_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    Scorecard *new = (Scorecard *)cls->tp_alloc(cls, 0);
    new->ones = -1;
    new->twos = -1;
    return (PyObject *)new;
}

static PyObject *Scorecard_total(PyObject *self, PyObject *unused) {
    int result = SCORECARD_VAL(self, ones)
        + SCORECARD_VAL(self, twos);
    return PyLong_FromLong(result);
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
    SCORECARD(self)->ones = result;
    return PyLong_FromLong(result);
}

static PyMethodDef scorecard_methods[] = {
    {"total", Scorecard_total, METH_NOARGS},
    {"score_as_ones", Scorecard_score_as_ones, METH_O},
    {0},
};

static PyTypeObject Scorecard_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Scorecard",
    .tp_doc = "",
    .tp_basicsize = sizeof(Scorecard),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Scorecard_New,
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

