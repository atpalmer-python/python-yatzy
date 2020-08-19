#include <Python.h>
#include "structmember.h"
#include "roll.h"

#define RANDOM_DIE_VALUE    ((rand() % 6) + 1)

static PyObject *_ensure_arg_len(PyObject *tuple) {
    if(PyTuple_Size(tuple) == ROLL_NUM_DIE)
        return tuple;
    PyErr_SetString(PyExc_ValueError, "Must provide 5 args");
    return NULL;
}

static Py_ssize_t _ensure_valid_index(Py_ssize_t i) {
    if(!ROLL_INDEX_IS_VALID(i)) {
        PyErr_Format(PyExc_IndexError, "Index is out of range: %d", i);
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
    new->rolls_left = 2;
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
    new->rolls_left = 2;
    for(int i = 0; i < 5; ++i) {
        new->dice[i] = RANDOM_DIE_VALUE;
    }
    return (PyObject *)new;
}

static PyObject *Roll_hold(PyObject *self, PyObject *argv[], Py_ssize_t argc) {
    if(ROLL_ROLLS_LEFT(self) == 0) {
        PyErr_SetString(PyExc_RuntimeError, "No rolls left");
        return NULL;
    }

    int hold[5] = {0};  /* boolean T/F. Set index to 1 if holding */
    for(int i = 0; i < argc; ++i) {
        int val = PyLong_AsLong(argv[i]);
        if(PyErr_Occurred())
            return NULL;
        if(_ensure_valid_index(val) < 0)
            return NULL;
        /* No need to validate whether `val` is already held.
         * Just allow duplicates in argv */
        hold[val] = 1;
    }

    PyTypeObject *cls = Py_TYPE(self);
    Roll *new = (Roll *)cls->tp_alloc(cls, 0);
    new->rolls_left = ROLL_ROLLS_LEFT(self) - 1;
    for(int i = 0; i < 5; ++i) {
        new->dice[i] = hold[i] ? ROLL_DIE_VALUE(self, i) : RANDOM_DIE_VALUE;
    }
    return (PyObject *)new;
}

static PyObject *Roll_Repr(PyObject *self) {
    static const size_t RESULT_LEN = 16;
    char result[RESULT_LEN] = {0};
    char *p = result;

    *p++ = '[';
    for(int i = 0; i < 5; ++i) {
        if(i) {
            *p++ = ',';
            *p++ = ' ';
        }
        sprintf(p++, "%d", ROLL_DIE_VALUE(self, i));
    }
    *p++ = ']';
    *p++ = '\0';

    if(p - result != RESULT_LEN) {
        PyErr_SetString(PyExc_SystemError, "Error creating repr: corrupted state");
        return NULL;
    }

    return PyUnicode_FromString(result);
}

static PyMethodDef Roll_methods[] = {
    {"roll", (PyCFunction)Roll_roll, METH_CLASS | METH_NOARGS},
    {"hold", (PyCFunction)Roll_hold, METH_FASTCALL},
    {0},
};

static PyMemberDef Roll_members[] = {
    {"rolls_left", T_INT, offsetof(Roll, rolls_left), READONLY},
    {0}
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
    .tp_members = Roll_members,
    .tp_repr = Roll_Repr,
};

