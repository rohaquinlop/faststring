#include <Python.h>

PyNumberMethods StringBuilder_as_number;

typedef struct {
    PyObject_HEAD char *buffer;
    size_t size;
    size_t capacity;
} StringBuilder;

#define INITIAL_CAPACITY 4

static void StringBuilder_dealloc(StringBuilder *self) {
    free(self->buffer);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int StringBuilder_resize(StringBuilder *self, size_t new_capacity) {
    char *new_buffer =
        (char *)realloc(self->buffer, new_capacity * sizeof(char));
    if (!new_buffer) return -1;

    self->buffer = new_buffer;
    self->capacity = new_capacity;
    return 0;
}

static PyObject *StringBuilder_append(StringBuilder *self, PyObject *args) {
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str)) return NULL;

    size_t str_len = strlen(str);
    if (self->size + str_len + 1 > self->capacity) {
        size_t new_capacity = self->capacity * 2;
        while (new_capacity < self->size + str_len + 1) {
            new_capacity *= 2;
        }
        if (StringBuilder_resize(self, new_capacity) < 0) return NULL;
    }

    memcpy(self->buffer + self->size, str, str_len);
    self->size += str_len;
    self->buffer[self->size] = '\0';

    Py_RETURN_NONE;
}

static int StringBuilder_setitem(StringBuilder *self, Py_ssize_t index,
                                 PyObject *value) {
    if (index < 0 || (size_t)index >= self->size) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        return -1;
    }

    if (!PyUnicode_Check(value) || PyUnicode_GetLength(value) != 1) {
        PyErr_SetString(PyExc_TypeError,
                        "Value must be a single character string");
        return -1;
    }

    const char *char_value = PyUnicode_AsUTF8(value);
    if (!char_value) return -1;

    self->buffer[index] = char_value[0];
    return 0;
}

static Py_ssize_t StringBuilder_length(StringBuilder *self) {
    return self->size;
}

static PyObject *StringBuilder_to_string(StringBuilder *self) {
    return PyUnicode_FromString(self->buffer);
}

static PyObject *StringBuilder_find(StringBuilder *self, PyObject *args) {
    const char *str;
    long long int idx = -1;
    if (!PyArg_ParseTuple(args, "s", &str)) return NULL;

    char *found = strstr(self->buffer, str);
    if (found != NULL) {
        idx = found - self->buffer;
    }

    return PyLong_FromLong(idx);
}

static PyObject *StringBuilder_clear(StringBuilder *self) {
    self->size = 0;
    free(self->buffer);

    self->buffer = (char *)malloc(1 * sizeof(char));
    if (!self->buffer) return NULL;

    self->buffer[0] = '\0';

    Py_RETURN_NONE;
}

static PyMethodDef StringBuilderMethods[] = {
    {"append", (PyCFunction)StringBuilder_append, METH_VARARGS,

     "Append a string."},
    {"to_string", (PyCFunction)StringBuilder_to_string, METH_NOARGS,

     "Get the concatenated string."},
    {"find", (PyCFunction)StringBuilder_find, METH_VARARGS,

     "Return the index of the first occurrence of the substring."},
    {"clear", (PyCFunction)StringBuilder_clear, METH_NOARGS,

     "Clear the buffer."},
    {NULL},
};

static PySequenceMethods StringBuilder_as_sequence = {
    .sq_length = (lenfunc)StringBuilder_length,
    .sq_ass_item = (ssizeobjargproc)StringBuilder_setitem,
};

static int StringBuilder_init(StringBuilder *self, PyObject *args,
                              PyObject *kwds) {
    const char *initial_str = NULL;
    static char *kwlist[] = {"initial_str", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &initial_str)) {
        return -1;
    }

    self->size = 0;
    self->capacity = INITIAL_CAPACITY;
    self->buffer = (char *)malloc(self->capacity * sizeof(char));
    if (!self->buffer) return -1;

    self->buffer[0] = '\0';

    if (initial_str) {
        size_t str_len = strlen(initial_str);
        if (str_len + 1 > self->capacity) {
            size_t new_capacity = self->capacity * 2;
            while (new_capacity < str_len + 1) {
                new_capacity *= 2;
            }
            if (StringBuilder_resize(self, new_capacity) < 0) return -1;
        }

        memcpy(self->buffer, initial_str, str_len);
        self->size = str_len;
        self->buffer[self->size] = '\0';
    }

    return 0;
}

static PyTypeObject StringBuilderType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "faststring.StringBuilder",
    .tp_basicsize = sizeof(StringBuilder),
    .tp_dealloc = (destructor)StringBuilder_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = StringBuilderMethods,
    .tp_init = (initproc)StringBuilder_init,
    .tp_new = PyType_GenericNew,
    .tp_as_number = &StringBuilder_as_number,
    .tp_as_sequence = &StringBuilder_as_sequence,
    .tp_str = (reprfunc)StringBuilder_to_string,
};

static PyObject *StringBuilder_add(StringBuilder *self, PyObject *other) {
    const char *str = NULL;
    size_t str_len = 0;

    if (PyObject_TypeCheck(other, &StringBuilderType)) {
        StringBuilder *sb = (StringBuilder *)other;
        str = sb->buffer;
        str_len = sb->size;
    } else if (PyUnicode_Check(other)) {
        str = PyUnicode_AsUTF8(other);
        if (!str) return NULL;
        str_len = strlen(str);
    } else {
        PyErr_SetString(
            PyExc_TypeError,
            "Can only concatenate with a StringBuilder or a Python string.");
        return NULL;
    }

    StringBuilder *new_sb = PyObject_New(StringBuilder, &StringBuilderType);
    if (!new_sb) return PyErr_NoMemory();

    new_sb->size = self->size + str_len;
    new_sb->capacity = new_sb->size + 1;
    new_sb->buffer = (char *)malloc(new_sb->capacity * sizeof(char));
    if (!new_sb->buffer) {
        PyErr_NoMemory();
        Py_DECREF(new_sb);
        return NULL;
    }

    memcpy(new_sb->buffer, self->buffer, self->size);
    memcpy(new_sb->buffer + self->size, str, str_len);
    new_sb->buffer[new_sb->size] = '\0';

    return (PyObject *)new_sb;
}

static PyObject *StringBuilder_inplace_add(StringBuilder *self,
                                           PyObject *other) {
    const char *str = NULL;
    size_t str_len = 0;

    if (PyObject_TypeCheck(other, &StringBuilderType)) {
        StringBuilder *sb = (StringBuilder *)other;
        str = sb->buffer;
        str_len = sb->size;
    } else if (PyUnicode_Check(other)) {
        str = PyUnicode_AsUTF8(other);
        if (!str) return NULL;
        str_len = strlen(str);
    } else {
        PyErr_SetString(
            PyExc_TypeError,
            "Can only concatenate with a StringBuilder or a Python string.");
        return NULL;
    }

    if (self->size + str_len + 1 > self->capacity) {
        size_t new_capacity = self->capacity * 2;
        while (new_capacity < self->size + str_len + 1) {
            new_capacity *= 2;
        }
        if (StringBuilder_resize(self, new_capacity) < 0) return NULL;
    }

    memcpy(self->buffer + self->size, str, str_len);
    self->size += str_len;
    self->buffer[self->size] = '\0';

    Py_INCREF(self);
    return (PyObject *)self;
}

PyNumberMethods StringBuilder_as_number = {
    .nb_add = (binaryfunc)StringBuilder_add,
    .nb_inplace_add = (binaryfunc)StringBuilder_inplace_add,
};

static PyMethodDef faststringMethods[] = {{NULL, NULL, 0, NULL}};

static struct PyModuleDef faststringmodule = {
    PyModuleDef_HEAD_INIT, "faststring",
    "Python library to improve performance of strings operations", -1,
    faststringMethods};

PyMODINIT_FUNC PyInit_faststring(void) {
    PyObject *m;
    if (PyType_Ready(&StringBuilderType) < 0) return NULL;

    m = PyModule_Create(&faststringmodule);
    if (!m) return NULL;

    Py_INCREF(&StringBuilderType);
    PyModule_AddObject(m, "StringBuilder", (PyObject *)&StringBuilderType);

    return m;
}
