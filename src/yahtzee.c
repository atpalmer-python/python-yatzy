#include <Python.h>
#include "yahtzee_roll.h"
#include "yahtzee_scorecard.h"

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

