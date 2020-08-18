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
    int full_house;
    int small_straight;
    int large_straight;
    int chance;
    int yahtzee;  /* Yahtzees are different. Store count (not score) */
} Scorecard;

#define SCORECARD(self)                 ((Scorecard *)self)
#define SCORECARD_FIELD(self, f)        (SCORECARD(self)->f)
#define SCORECARD_HAS_VAL(self, f)      (!SLOT_IS_FREE(SCORECARD_FIELD(self, f)))
#define SCORECARD_VAL(self, f)          (SCORECARD_HAS_VAL(self, f) ? SCORECARD_FIELD(self, f) : 0)
#define YAHTZEE_TOTAL(self)   \
    (SCORECARD_VAL(self, yahtzee) ? (50 + ((SCORECARD_VAL(self, yahtzee) - 1) * 100)) : 0)

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
    new->full_house = -1;
    new->small_straight = -1;
    new->large_straight = -1;
    new->chance = -1;
    new->yahtzee = -1;

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
        + SCORECARD_VAL(self, full_house)
        + SCORECARD_VAL(self, small_straight)
        + SCORECARD_VAL(self, large_straight)
        + SCORECARD_VAL(self, chance)
        + YAHTZEE_TOTAL(self)
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
    if(_apply_score(result, &SCORECARD(self)->twos) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_threes(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 3);
    if(result < 0)
        return NULL;
    if(_apply_score(result, &SCORECARD(self)->threes) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_fours(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 4);
    if(result < 0)
        return NULL;
    if(_apply_score(result, &SCORECARD(self)->fours) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_fives(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 5);
    if(result < 0)
        return NULL;
    if(_apply_score(result, &SCORECARD(self)->fives) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_sixes(PyObject *self, PyObject *arg) {
    int result = _score_top(arg, 6);
    if(result < 0)
        return NULL;
    if(_apply_score(result, &SCORECARD(self)->sixes) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static int _roll_total(PyObject *roll) {
    if(!_ensure_Roll(roll))
        return -1;
    int result = 0;
    for(int i = 0; i < ROLL_NUM_DIE; ++i) {
        int value = ROLL_DIE_VALUE(roll, i);
        result += value;
    }
    return result;
}

typedef int _Counts[6];

#define COUNTS_INIT                 {0}
#define COUNTS_FOR_DIE(self, die)   ((self)[(die) - 1])

static void *_roll_counts(PyObject *roll, _Counts counts) {
    if(!_ensure_Roll(roll))
        return NULL;
    for(int i = 0; i < ROLL_NUM_DIE; ++i) {
        int value = ROLL_DIE_VALUE(roll, i);
        if(!ROLL_DIE_VALUE_IS_VALID(value))
            goto err;
        ++COUNTS_FOR_DIE(counts, value);
    }
    return counts;

err:
    PyErr_SetString(PyExc_SystemError, "Roll contained invalid die value");
    return NULL;
}

#define KIND_EXACT      (0)
#define KIND_AT_LEAST   (5)

static int _has_kind(_Counts counts, int kind, int max_diff) {
    /*
     * Exact match: max_diff = 0
     * Greater or equal: max_diff = 5 (big enough to include all dice)
     */
    for(int die = 1; die <= 6; ++die) {
        int diff = COUNTS_FOR_DIE(counts, die) - kind;
        if(diff >= 0 && diff <= max_diff)
            return 1;
    }
    return 0;
}

static PyObject *Scorecard_score_as_three_of_a_kind(PyObject *self, PyObject *arg) {
    _Counts counts = COUNTS_INIT;
    if(!_roll_counts(arg, counts))
        return NULL;
    int result = _has_kind(counts, 3, KIND_AT_LEAST) ? _roll_total(arg) : 0;
    if(_apply_score(result, &SCORECARD(self)->three_of_a_kind) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_four_of_a_kind(PyObject *self, PyObject *arg) {
    _Counts counts = COUNTS_INIT;
    if(!_roll_counts(arg, counts))
        return NULL;
    int result = _has_kind(counts, 4, KIND_AT_LEAST) ? _roll_total(arg) : 0;
    if(_apply_score(result, &SCORECARD(self)->four_of_a_kind) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_full_house(PyObject *self, PyObject *arg) {
    _Counts counts = COUNTS_INIT;
    if(!_roll_counts(arg, counts))
        return NULL;

    int result = (_has_kind(counts, 3, KIND_EXACT) && _has_kind(counts, 2, KIND_EXACT)) ? 25 : 0;

    if(_apply_score(result, &SCORECARD(self)->full_house) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static int _straight_len(_Counts counts) {
    int maxlen = 0, len = 0;
    for(int die = 1; die <= 6; ++die) {
        if(!COUNTS_FOR_DIE(counts, die))
            len = 0;
        else
            ++len;
        maxlen = (len > maxlen) ? len : maxlen;
    }
    return maxlen;
}

static PyObject *Scorecard_score_as_small_straight(PyObject *self, PyObject *arg) {
    _Counts counts = COUNTS_INIT;
    if(!_roll_counts(arg, counts))
        return NULL;
    int result = _straight_len(counts) >= 4 ? 30 : 0;
    if(_apply_score(result, &SCORECARD(self)->small_straight) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_large_straight(PyObject *self, PyObject *arg) {
    _Counts counts = COUNTS_INIT;
    if(!_roll_counts(arg, counts))
        return NULL;
    int result = _straight_len(counts) >= 5 ? 40 : 0;
    if(_apply_score(result, &SCORECARD(self)->large_straight) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_chance(PyObject *self, PyObject *arg) {
    int result = _roll_total(arg);
    if(result < 0)
        return NULL;
    if(_apply_score(result, &SCORECARD(self)->chance) < 0)
        return NULL;
    return PyLong_FromLong(result);
}

static PyObject *Scorecard_score_as_yahtzee(PyObject *self, PyObject *arg) {
    _Counts counts = COUNTS_INIT;
    if(!_roll_counts(arg, counts))
        return NULL;

    if(SCORECARD(self)->yahtzee == 0) {
        /* Don't use SLOT_IS_FREE for Yahtzees. >0 OK. */
        PyErr_SetString(PyExc_RuntimeError, "Invalid choice. Slot is already in use");
        return NULL;
    }

    if(!_has_kind(counts, 5, KIND_EXACT)) {
        SCORECARD(self)->yahtzee = 0;
        return PyLong_FromLong(0);
    }

    int orig = YAHTZEE_TOTAL(self);
    int new_count = SCORECARD(self)->yahtzee < 0 ? 1 : SCORECARD(self)->yahtzee + 1;
    SCORECARD(self)->yahtzee = new_count;
    int result = YAHTZEE_TOTAL(self) - orig;

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
    {"score_as_full_house", Scorecard_score_as_full_house, METH_O},
    {"score_as_small_straight", Scorecard_score_as_small_straight, METH_O},
    {"score_as_large_straight", Scorecard_score_as_large_straight, METH_O},
    {"score_as_chance", Scorecard_score_as_chance, METH_O},
    {"score_as_yahtzee", Scorecard_score_as_yahtzee, METH_O},
    {0},
};

PyTypeObject Scorecard_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Scorecard",
    .tp_doc = "",
    .tp_basicsize = sizeof(Scorecard),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Scorecard_New,
    .tp_methods = scorecard_methods,
};

