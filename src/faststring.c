#include <Python.h>

PyNumberMethods MString_as_number;

typedef struct {
    PyObject_HEAD char *buffer;
    size_t size;
    size_t capacity;
} MString;

#define INITIAL_CAPACITY 4

static void MString_dealloc(MString *self) {
    free(self->buffer);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static Py_ssize_t MString_length(MString *self) { return self->size; }

static PyObject *MString_to_string(MString *self) {
    return PyUnicode_FromString(self->buffer);
}

static PyObject *MString_getitem(MString *self, Py_ssize_t index) {
    if (index < 0 || (size_t)index >= self->size) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        return NULL;
    }

    return PyUnicode_FromStringAndSize(self->buffer + index, 1);
}

static int MString_contains(MString *self, PyObject *value) {
    if (!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "value must be a string");
        return -1;
    }

    const char *str = PyUnicode_AsUTF8(value);
    if (!str) return -1;

    if (strstr(self->buffer, str)) {
        return 1;
    } else {
        return 0;
    }
}

static PyObject *MString_iter(MString *self) {
    return PySeqIter_New((PyObject *)self);
}

static PyObject *MString_repr(MString *self) {
    return PyUnicode_FromFormat("<MString: %s>", self->buffer);
}

static PyObject *MString_find(MString *self, PyObject *args) {
    const char *str;
    long long int idx = -1;
    if (!PyArg_ParseTuple(args, "s", &str)) return NULL;

    char *found = strstr(self->buffer, str);
    if (found != NULL) {
        idx = found - self->buffer;
    }

    return PyLong_FromLong(idx);
}

static PyObject *MString_reverse(MString *self) {
    if (self->size == 0) {
        Py_RETURN_NONE;
    }

    size_t start = 0, end = self->size - 1;
    char temp;

    while (start < end) {
        temp = self->buffer[start];
        self->buffer[start] = self->buffer[end];
        self->buffer[end] = temp;

        start++;
        end--;
    }

    Py_RETURN_NONE;
}

static int MString_setitem(MString *self, Py_ssize_t index, PyObject *value) {
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

static PyObject *MString_clear(MString *self) {
    self->size = 0;
    free(self->buffer);

    self->buffer = (char *)malloc(1 * sizeof(char));
    if (!self->buffer) return NULL;

    self->buffer[0] = '\0';

    Py_RETURN_NONE;
}

static int MString_resize(MString *self, size_t new_capacity) {
    self->buffer = (char *)realloc(self->buffer, new_capacity * sizeof(char));
    if (!self->buffer) return -1;

    self->capacity = new_capacity;
    return 0;
}

static PyObject *MString_append(MString *self, PyObject *args) {
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str)) return NULL;

    size_t str_len = strlen(str);
    if (self->size + str_len + 1 > self->capacity) {
        size_t new_capacity = self->capacity * 2;
        while (new_capacity < self->size + str_len + 1) {
            new_capacity *= 2;
        }
        if (MString_resize(self, new_capacity) < 0) return NULL;
    }

    memcpy(self->buffer + self->size, str, str_len);
    self->size += str_len;
    self->buffer[self->size] = '\0';

    Py_RETURN_NONE;
}

static PyObject *MString_insert(MString *self, PyObject *args) {
    Py_ssize_t index;
    const char *str;

    if (!PyArg_ParseTuple(args, "ns", &index, &str)) {
        return NULL;
    }

    if (index < 0 || (size_t)index > self->size) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }

    size_t str_len = strlen(str);
    if (self->size + str_len + 1 > self->capacity) {
        size_t new_capacity = self->capacity * 2;
        while (new_capacity < self->size + str_len + 1) {
            new_capacity *= 2;
        }
        if (MString_resize(self, new_capacity) < 0) {
            return NULL;
        }
    }

    memmove(self->buffer + index + str_len, self->buffer + index,
            self->size - index + 1);
    memcpy(self->buffer + index, str, str_len);
    self->size += str_len;

    Py_RETURN_NONE;
}

static PyObject *MString_replace(MString *self, PyObject *args) {
    const char *old_str, *new_str;
    if (!PyArg_ParseTuple(args, "ss", &old_str, &new_str)) return NULL;

    size_t old_len = strlen(old_str);
    size_t new_len = strlen(new_str);

    if (old_len == 0) {
        PyErr_SetString(PyExc_ValueError, "old_str must not be empty");
        return NULL;
    }

    size_t count = 0;
    char *found = self->buffer;
    while ((found = strstr(found, old_str)) != NULL) {
        count++;
        found += old_len;
    }

    if (count == 0) {
        Py_RETURN_NONE;
    }

    size_t new_size = self->size + count * (new_len - old_len);
    if (new_size + 1 > self->capacity) {
        size_t new_capacity = self->capacity * 2;
        while (new_capacity < new_size + 1) {
            new_capacity *= 2;
        }
        if (MString_resize(self, new_capacity) < 0) return NULL;
    }

    char *old_buffer = self->buffer;
    self->buffer = (char *)malloc(self->capacity * sizeof(char));
    if (!self->buffer) {
        free(old_buffer);
        return PyErr_NoMemory();
    }

    char *write_ptr = self->buffer;
    char *read_ptr = old_buffer;
    char *found_ptr = old_buffer;

    while ((found_ptr = strstr(read_ptr, old_str)) != NULL) {
        size_t len = found_ptr - read_ptr;
        memcpy(write_ptr, read_ptr, len);
        write_ptr += len;
        memcpy(write_ptr, new_str, new_len);
        write_ptr += new_len;
        read_ptr = found_ptr + old_len;
    }

    size_t len = self->size - (read_ptr - old_buffer);
    memcpy(write_ptr, read_ptr, len);
    write_ptr += len;
    *write_ptr = '\0';

    free(old_buffer);
    self->size = new_size;

    Py_RETURN_NONE;
}

static PyObject *MString_subscript(MString *self, PyObject *key) {
    if (PyIndex_Check(key)) {
        Py_ssize_t index = PyNumber_AsSsize_t(key, PyExc_IndexError);
        if (index == -1 && PyErr_Occurred()) {
            return NULL;
        }
        if (index < 0 || index >= (Py_ssize_t)self->size) {
            PyErr_SetString(PyExc_IndexError, "Index out of range");
            return NULL;
        }
        return PyUnicode_FromStringAndSize(self->buffer + index, 1);
    } else if (PySlice_Check(key)) {
        Py_ssize_t start, stop, step, slicelength;

        if (PySlice_Unpack(key, &start, &stop, &step) < 0) return NULL;

        slicelength = PySlice_AdjustIndices(self->size, &start, &stop, step);
        if (slicelength <= 0) return PyUnicode_FromString("");
        return PyUnicode_FromStringAndSize(self->buffer + start, slicelength);
    } else {
        PyErr_SetString(PyExc_TypeError, "Index must be an integer or slice");
        return NULL;
    }
}

static int MString_ass_subscript(MString *self, PyObject *key,
                                 PyObject *value) {
    if (PyIndex_Check(key)) {
        Py_ssize_t index = PyNumber_AsSsize_t(key, PyExc_IndexError);
        if (index == -1 && PyErr_Occurred()) return -1;
        if (index < 0 || index >= (Py_ssize_t)self->size) {
            PyErr_SetString(PyExc_IndexError, "index out of range");
            return -1;
        }
        if (!PyUnicode_Check(value) || PyUnicode_GetLength(value) != 1) {
            PyErr_SetString(PyExc_TypeError,
                            "value must be a single character string");
            return -1;
        }

        const char *char_value = PyUnicode_AsUTF8(value);
        if (!char_value) return -1;
        self->buffer[index] = char_value[0];
        return 0;
    } else if (PySlice_Check(key)) {
        Py_ssize_t start, stop, step, slicelength;
        if (PySlice_Unpack(key, &start, &stop, &step) < 0) return -1;

        slicelength = PySlice_AdjustIndices(self->size, &start, &stop, step);

        const char *str = PyUnicode_AsUTF8(value);
        if (!str) return -1;

        size_t str_len = strlen(str);
        size_t new_size = self->size - slicelength + str_len;

        if (new_size + 1 > self->capacity) {
            size_t new_capacity = self->capacity * 2;
            while (new_capacity < new_size + 1) {
                new_capacity *= 2;
            }
            if (MString_resize(self, new_capacity) < 0) return -1;
        }

        memmove(self->buffer + start + str_len, self->buffer + stop,
                self->size - stop + 1);
        memcpy(self->buffer + start, str, str_len);
        self->size = new_size;
        return 0;
    } else {
        PyErr_SetString(PyExc_TypeError,
                        "sequence index must be an integer or slice");
        return -1;
    }
}

static PyMethodDef MStringMethods[] = {
    {"append", (PyCFunction)MString_append, METH_VARARGS, "Append a string."},
    {"to_string", (PyCFunction)MString_to_string, METH_NOARGS,
     "Get the concatenated string."},
    {"find", (PyCFunction)MString_find, METH_VARARGS,
     "Return the index of the first occurrence of the substring."},
    {"clear", (PyCFunction)MString_clear, METH_NOARGS, "Clear the buffer."},
    {"insert", (PyCFunction)MString_insert, METH_VARARGS,
     "Insert a string at the specified index."},
    {"reverse", (PyCFunction)MString_reverse, METH_NOARGS,
     "Reverse the string."},
    {"replace", (PyCFunction)MString_replace, METH_VARARGS,
     "Replace occurences of a substring."},
    {NULL},
};

static PySequenceMethods MString_as_sequence = {
    .sq_length = (lenfunc)MString_length,
    .sq_item = (ssizeargfunc)MString_getitem,
    .sq_ass_item = (ssizeobjargproc)MString_setitem,
    .sq_contains = (objobjproc)MString_contains,
};

static PyMappingMethods MString_as_mapping = {
    .mp_length = (lenfunc)MString_length,
    .mp_subscript = (binaryfunc)MString_subscript,
    .mp_ass_subscript = (objobjargproc)MString_ass_subscript,
};

static int MString_init(MString *self, PyObject *args, PyObject *kwds) {
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
            if (MString_resize(self, new_capacity) < 0) return -1;
        }

        memcpy(self->buffer, initial_str, str_len);
        self->size = str_len;
        self->buffer[self->size] = '\0';
    }

    return 0;
}

static PyTypeObject MStringType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "faststring.MString",
    .tp_basicsize = sizeof(MString),
    .tp_dealloc = (destructor)MString_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = MStringMethods,
    .tp_init = (initproc)MString_init,
    .tp_new = PyType_GenericNew,
    .tp_as_number = &MString_as_number,
    .tp_as_sequence = &MString_as_sequence,
    .tp_as_mapping = &MString_as_mapping,
    .tp_str = (reprfunc)MString_to_string,
    .tp_repr = (reprfunc)MString_repr,
    .tp_iter = (getiterfunc)MString_iter,
};

static PyObject *MString_add(MString *self, PyObject *other) {
    const char *str = NULL;
    size_t str_len = 0;

    if (PyObject_TypeCheck(other, &MStringType)) {
        MString *sb = (MString *)other;
        str = sb->buffer;
        str_len = sb->size;
    } else if (PyUnicode_Check(other)) {
        str = PyUnicode_AsUTF8(other);
        if (!str) return NULL;
        str_len = strlen(str);
    } else {
        PyErr_SetString(
            PyExc_TypeError,
            "Can only concatenate with a MString or a Python string.");
        return NULL;
    }

    MString *new_ms = PyObject_New(MString, &MStringType);
    if (!new_ms) return PyErr_NoMemory();

    new_ms->size = self->size + str_len;
    new_ms->capacity = self->capacity;
    new_ms->buffer = (char *)malloc(new_ms->capacity * sizeof(char));
    if (!new_ms->buffer) {
        PyErr_NoMemory();
        Py_DECREF(new_ms);
        return NULL;
    }

    if (new_ms->size + 1 > new_ms->capacity) {
        size_t new_capacity = new_ms->capacity * 2;
        while (new_capacity < new_ms->size + 1) {
            new_capacity *= 2;
        }
        if (MString_resize(new_ms, new_capacity) < 0) return NULL;
    }

    memcpy(new_ms->buffer, self->buffer, self->size);
    memcpy(new_ms->buffer + self->size, str, str_len);
    new_ms->buffer[new_ms->size] = '\0';

    return (PyObject *)new_ms;
}

static PyObject *MString_inplace_add(MString *self, PyObject *other) {
    const char *str = NULL;
    size_t str_len = 0;

    if (PyObject_TypeCheck(other, &MStringType)) {
        MString *sb = (MString *)other;
        str = sb->buffer;
        str_len = sb->size;
    } else if (PyUnicode_Check(other)) {
        str = PyUnicode_AsUTF8(other);
        if (!str) return NULL;
        str_len = strlen(str);
    } else {
        PyErr_SetString(
            PyExc_TypeError,
            "Can only concatenate with a MString or a Python string.");
        return NULL;
    }

    if (self->size + str_len + 1 > self->capacity) {
        size_t new_capacity = self->capacity * 2;
        while (new_capacity < self->size + str_len + 1) {
            new_capacity *= 2;
        }
        if (MString_resize(self, new_capacity) < 0) return NULL;
    }

    memcpy(self->buffer + self->size, str, str_len);
    self->size += str_len;
    self->buffer[self->size] = '\0';

    Py_INCREF(self);
    return (PyObject *)self;
}

static PyObject *MString_nb_multiply(MString *self, PyObject *value) {
    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be an integer.");
        return NULL;
    }

    long int n = PyLong_AsLong(value);
    if (n < 0) {
        PyErr_SetString(PyExc_ValueError,
                        "Argument must be a positive integer.");
        return NULL;
    }

    MString *new_ms = PyObject_New(MString, &MStringType);
    if (!new_ms) return PyErr_NoMemory();

    new_ms->size = self->size * n;
    new_ms->capacity = self->capacity;
    new_ms->buffer = (char *)malloc(new_ms->capacity * sizeof(char));
    if (!new_ms->buffer) {
        PyErr_NoMemory();
        Py_DECREF(new_ms);
        return NULL;
    }

    if (new_ms->size + 1 > new_ms->capacity) {
        size_t new_capacity = new_ms->capacity * 2;
        while (new_capacity < new_ms->size + 1) {
            new_capacity *= 2;
        }
        if (MString_resize(new_ms, new_capacity) < 0) {
            PyErr_SetString(PyExc_MemoryError, "Memory allocation failed.");
            Py_DECREF(new_ms);
            return NULL;
        }
    }

    for (Py_ssize_t i = 0; i < n; i++) {
        memcpy(new_ms->buffer + (i * self->size), self->buffer, self->size);
    }
    new_ms->buffer[new_ms->size] = '\0';

    return (PyObject *)new_ms;
}

static PyObject *MString_inplace_multiply(MString *self, PyObject *value) {
    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be an integer.");
        return NULL;
    }

    long int n = PyLong_AsLong(value);
    if (n < 0) {
        PyErr_SetString(PyExc_ValueError,
                        "Argument must be a positive integer.");
        return NULL;
    }

    size_t new_size = self->size * n;
    if (new_size + 1 > self->capacity) {
        size_t new_capacity = self->capacity * 2;
        while (new_capacity < new_size + 1) {
            new_capacity *= 2;
        }
        if (MString_resize(self, new_capacity) < 0) {
            PyErr_SetString(PyExc_MemoryError, "Memory allocation failed.");
            return NULL;
        }
    }

    for (Py_ssize_t i = 0; i < n; i++) {
        memcpy(self->buffer + (i * self->size), self->buffer, self->size);
    }
    self->size = new_size;
    self->buffer[self->size] = '\0';

    Py_INCREF(self);
    return (PyObject *)self;
}

PyNumberMethods MString_as_number = {
    .nb_add = (binaryfunc)MString_add,
    .nb_inplace_add = (binaryfunc)MString_inplace_add,
    .nb_multiply = (binaryfunc)MString_nb_multiply,
    .nb_inplace_multiply = (binaryfunc)MString_inplace_multiply,
};

static PyMethodDef faststringMethods[] = {{NULL, NULL, 0, NULL}};

static struct PyModuleDef faststringmodule = {
    PyModuleDef_HEAD_INIT, "faststring",
    "Python library for mutable string manipulation.", -1, faststringMethods};

PyMODINIT_FUNC PyInit_faststring(void) {
    PyObject *m;
    if (PyType_Ready(&MStringType) < 0) return NULL;

    m = PyModule_Create(&faststringmodule);
    if (!m) return NULL;

    Py_INCREF(&MStringType);
    PyModule_AddObject(m, "MString", (PyObject *)&MStringType);

    return m;
}
