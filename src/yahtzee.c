#include <Python.h>

typedef struct {
    PyObject_HEAD
    uint8_t dice[5];
} Roll;

#define DIE_VALUE(self, i)  (((Roll *)self)->dice[(i)])

static PyObject *Roll_New(PyTypeObject *tp, PyObject *args, PyObject *kwargs) {
    Roll *new = (Roll *)tp->tp_alloc(tp, 0);
    for(int i = 0; i < 5; ++i) {
        new->dice[i] = (rand() % 6) + 1;
    }
    return (PyObject *)new;
}

static PyObject *Roll_Item(PyObject *self, Py_ssize_t i) {
    if(i < 0 || i >= 5) {
        PyErr_SetString(PyExc_IndexError, "Index is out of range.");
        return NULL;
    }
    return PyLong_FromLong(DIE_VALUE(self, i));
}

static PySequenceMethods Roll_as_sequence = {
    .sq_item = Roll_Item,
};

static PyTypeObject Roll_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Roll",
    .tp_doc = "",
    .tp_basicsize = sizeof(Roll),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Roll_New,
    .tp_as_sequence = &Roll_as_sequence,
};

typedef struct {
    PyObject_HEAD
} Scorecard;

static PyTypeObject Scorecard_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Scorecard",
    .tp_doc = "",
    .tp_basicsize = sizeof(Scorecard),
    .tp_flags = Py_TPFLAGS_DEFAULT,
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

