#include <Python.h>
#include "yahtzee_roll.h"


static PyObject *_ensure_arg_len(PyObject *tuple) {
    if(PyTuple_Size(tuple) == ROLL_NUM_DIE)
        return tuple;
    PyErr_SetString(PyExc_ValueError, "Must provide 5 args");
    return NULL;
}

static Py_ssize_t _ensure_valid_index(Py_ssize_t i) {
    if(!ROLL_INDEX_IS_VALID(i)) {
        PyErr_SetString(PyExc_IndexError, "Index is out of range.");
        return -1;
    }
    return i;
}

static int _tuple_item_as_die_value(PyObject *tuple, Py_ssize_t i) {
    if(_ensure_valid_index(i) < 0)
        return -1;
    PyObject *pyval = PyTuple_GET_ITEM(tuple, i);
    int val = PyLong_AsLong(pyval);
    if(PyErr_Occurred())
        return -1;
    if(!ROLL_DIE_VALUE_IS_VALID(val)) {
        PyErr_SetString(PyExc_ValueError, "Invalid die value.");
        return -1;
    }
    return val;
}

static PyObject *Roll_New(PyTypeObject *tp, PyObject *args, PyObject *kwargs) {
    if(!_ensure_arg_len(args))
        return NULL;

    Roll *new = (Roll *)tp->tp_alloc(tp, 0);
    for(int i = 0; i < ROLL_NUM_DIE; ++i) {
        new->dice[i] = _tuple_item_as_die_value(args, i);
        if(PyErr_Occurred())
            goto err;
    }
    return (PyObject *)new;

err:
    tp->tp_free((PyObject *)new);
    return NULL;
}

static PyObject *Roll_Item(PyObject *self, Py_ssize_t i) {
    if(_ensure_valid_index(i) < 0)
        return NULL;
    return PyLong_FromLong(ROLL_DIE_VALUE(self, i));
}

static PySequenceMethods Roll_as_sequence = {
    .sq_item = Roll_Item,
};

static PyObject *Roll_roll(PyTypeObject *cls, PyObject *args) {
    Roll *new = (Roll *)cls->tp_alloc(cls, 0);
    for(int i = 0; i < 5; ++i) {
        new->dice[i] = (rand() % 6) + 1;
    }
    return (PyObject *)new;
}

static PyMethodDef Roll_methods[] = {
    {"roll", (PyCFunction)Roll_roll, METH_CLASS | METH_NOARGS},
    {0},
};

PyTypeObject Roll_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Roll",
    .tp_doc = "",
    .tp_basicsize = sizeof(Roll),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Roll_New,
    .tp_as_sequence = &Roll_as_sequence,
    .tp_methods = Roll_methods,
};

