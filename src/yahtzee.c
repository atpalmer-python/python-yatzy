#include <Python.h>
#include "yahtzee_roll.h"

#define TOP_BONUS(top)      ((top) >= 63 ? 35 : 0)
#define SLOT_IS_FREE(slot)  ((slot) < 0)

typedef struct {
    PyObject_HEAD
    /* sign-bit on for unset */
    int ones;
    int twos;
    int threes;
    int fours;
    int fives;
    int sixes;
    int three_of_a_kind;
    int four_of_a_kind;
} Scorecard;

#define SCORECARD(self)                 ((Scorecard *)self)
#define SCORECARD_FIELD(self, f)        (SCORECARD(self)->f)
#define SCORECARD_HAS_VAL(self, f)      (!SLOT_IS_FREE(SCORECARD_FIELD(self, f)))
#define SCORECARD_VAL(self, f)          (SCORECARD_HAS_VAL(self, f) ? SCORECARD_FIELD(self, f) : 0)

static PyObject *_ensure_Roll(PyObject *arg) {
    if(Roll_CheckExact(arg))
        return arg;
    PyErr_SetString(PyExc_TypeError, "Argument must be Roll");
    return NULL;
}

static PyObject *Scorecard_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    Scorecard *new = (Scorecard *)cls->tp_alloc(cls, 0);

    /* upper section */
    new->ones = -1;
    new->twos = -1;
    new->threes = -1;
    new->fours = -1;
    new->fives = -1;
    new->sixes = -1;

    /* lower section */
    new->three_of_a_kind = -1;
    new->four_of_a_kind = -1;

    return (PyObject *)new;
}

static int _top_subtotal(PyObject *self) {
    int result = SCORECARD_VAL(self, ones)
        + SCORECARD_VAL(self, twos)
        + SCORECARD_VAL(self, threes)
        + SCORECARD_VAL(self, fours)
        + SCORECARD_VAL(self, fives)
        + SCORECARD_VAL(self, sixes)
        ;
    return result;
}

static int _top_total(PyObject *self) {
    int subtotal = _top_subtotal(self);
    int bonus = TOP_BONUS(subtotal);
    int total = subtotal + bonus;
    return total;
}

static int _bottom_total(PyObject *self) {
    int result = SCORECARD_VAL(self, three_of_a_kind)
        + SCORECARD_VAL(self, four_of_a_kind)
        ;
    return result;
}

static PyObject *Scorecard_top_subtotal(PyObject *self, PyObject *unused) {
    int subtotal = _top_subtotal(self);
    return PyLong_FromLong(subtotal);
}

static PyObject *Scorecard_top_total(PyObject *self, PyObject *unused) {
    int total = _top_total(self);
    return PyLong_FromLong(total);
}

static PyObject *Scorecard_bottom_total(PyObject *self, PyObject *unused) {
    int total = _bottom_total(self);
    return PyLong_FromLong(total);
}

static PyObject *Scorecard_total(PyObject *self, PyObject *unused) {
    int total = _top_total(self) + _bottom_total(self);
    return PyLong_FromLong(total);
}

static int _score_top(PyObject *roll, uint8_t die_val) {
    if(!_ensure_Roll(roll))
        return -1;
    int result = 0;
    for(int i = 0; i < ROLL_NUM_DIE; ++i) {
        int value = ROLL_DIE_VALUE(roll, i);
        if(value == die_val)
            result += value;
    }
    return result;
}

static int _apply_score(int score, int *slot) {
    if(SLOT_IS_FREE(*slot))
        return *slot = score;
    PyErr_SetString(PyExc_RuntimeError, "Invalid choice. Slot is already in use");
    return -1;
}

static PyObject *Scorecard_score_as_ones(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 1);
    if(result < 0)
        return NULL;
    if(_apply_score(result, &SCORECARD(self)->ones) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_twos(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 2);
    if(result < 0)
        return NULL;
    SCORECARD(self)->twos = result;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_threes(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 3);
    if(result < 0)
        return NULL;
    SCORECARD(self)->threes = result;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_fours(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 4);
    if(result < 0)
        return NULL;
    SCORECARD(self)->fours = result;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_fives(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 5);
    if(result < 0)
        return NULL;
    SCORECARD(self)->fives = result;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_sixes(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 6);
    if(result < 0)
        return NULL;
    SCORECARD(self)->sixes = result;
    return PyLong_FromLong(result);
}

typedef int _Counts[6];

#define COUNTS_INIT                 {0}
#define COUNTS_FOR_DIE(self, die)   ((self)[(die) - 1])

static void _roll_counts(PyObject *roll, _Counts counts) {
    for(int i = 0; i < ROLL_NUM_DIE; ++i) {
        int value = ROLL_DIE_VALUE(roll, i);
        /* TODO: validate range */
        ++COUNTS_FOR_DIE(counts, value);
    }
}

static int _counts_of_a_kind(_Counts counts, int kind) {
    int ok_flag = 0;
    int total = 0;
    for(int die = 1; die <= 6; ++die) {
        total += die * COUNTS_FOR_DIE(counts, die);
        if(COUNTS_FOR_DIE(counts, die) == kind)
            ok_flag = 1;
    }
    return ok_flag ? total : 0;
}

static PyObject *Scorecard_score_as_three_of_a_kind(PyObject *self, PyObject *arg) {
    if(!_ensure_Roll(arg))
        return -1;
    _Counts counts = COUNTS_INIT;
    _roll_counts(arg, counts);
    int result = _counts_of_a_kind(counts, 3);
    if(_apply_score(result, &SCORECARD(self)->three_of_a_kind) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_four_of_a_kind(PyObject *self, PyObject *arg) {
    if(!_ensure_Roll(arg))
        return -1;
    _Counts counts = COUNTS_INIT;
    _roll_counts(arg, counts);
    int result = _counts_of_a_kind(counts, 4);
    if(_apply_score(result, &SCORECARD(self)->four_of_a_kind) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyMethodDef scorecard_methods[] = {
    {"top_subtotal", Scorecard_top_subtotal, METH_NOARGS},
    {"top_total", Scorecard_top_total, METH_NOARGS},
    {"bottom_total", Scorecard_bottom_total, METH_NOARGS},
    {"total", Scorecard_total, METH_NOARGS},
    {"score_as_ones", Scorecard_score_as_ones, METH_O},
    {"score_as_twos", Scorecard_score_as_twos, METH_O},
    {"score_as_threes", Scorecard_score_as_threes, METH_O},
    {"score_as_fours", Scorecard_score_as_fours, METH_O},
    {"score_as_fives", Scorecard_score_as_fives, METH_O},
    {"score_as_sixes", Scorecard_score_as_sixes, METH_O},
    {"score_as_three_of_a_kind", Scorecard_score_as_three_of_a_kind, METH_O},
    {"score_as_four_of_a_kind", Scorecard_score_as_four_of_a_kind, METH_O},
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

