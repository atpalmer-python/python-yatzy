#ifndef ROLL_H
#define ROLL_H

#define ROLL_NUM_DIE                (5)

typedef struct {
    PyObject_HEAD
    uint8_t dice[ROLL_NUM_DIE];
} Roll;

#define ROLL_INDEX_IS_VALID(i)      ((i) >= 0 && (i) < ROLL_NUM_DIE)
#define ROLL_DIE_VALUE_IS_VALID(i)  ((i) >= 1 && (i) <= 6)
#define ROLL_DIE_VALUE(self, i)     (((Roll *)self)->dice[(i)])

extern PyTypeObject Roll_Type;

#define Roll_CheckExact(self) (Py_TYPE(self) == &Roll_Type)

#endif
