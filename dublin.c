#include <Python.h>
#include <structmember.h>


typedef struct {
    PyObject_HEAD
    PyObject *name;
} WhiskyObject;


static void
whisky_dealloc(WhiskyObject *self)
{
    fprintf(stdout, "whisky_dealloc\n");
    Py_XDECREF(self->name);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
whisky_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    fprintf(stdout, "whisky_new\n");
    WhiskyObject *self = NULL;

    self = (WhiskyObject *) type->tp_alloc(type, 0);    
    if (self != NULL) {
        self->name = PyUnicode_FromString("");
        if (self->name == NULL) {
            Py_DECREF(self);
            return NULL;
        }
    }
    fprintf(stdout, "self: %p", self);
    return (PyObject *) self;
}

static int
whisky_init(WhiskyObject *self, PyObject *args, PyObject *kwds)
{
    fprintf(stdout, "whisky_init\n");
    static char *kwlist[] = {
        "name", NULL
    };

    PyObject *whisky_name = NULL;
    PyObject *tmp = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist,
                                     &whisky_name)) {
        return -1;
    }

    if (whisky_name != NULL) {
        tmp = self->name;
        Py_INCREF(whisky_name);
        self->name = whisky_name;
        Py_XDECREF(tmp);
    }

    return 0;
}

static PyMemberDef whisky_members[] = {
    {"name", T_OBJECT_EX, offsetof(WhiskyObject, name), 0, "whisky name"},
    {NULL} // Sentinel    
};

static PyObject *
whisky_title_name(WhiskyObject *self, PyObject *Py_UNUSED(ignored))
{
    if (self->name == NULL) {
        PyErr_SetString(PyExc_AttributeError, "name");
        return NULL;
    }

    PyObject *title_name = PyObject_CallMethod(self->name, "title", NULL);
    if (title_name == NULL) {
        return NULL;
    }
    Py_INCREF(title_name);
    return title_name;
}

PyDoc_STRVAR(
    whisky_title_name__doc__,
    "Return the name of the Whisky via str.title()"
);

static PyMethodDef whisky_methods[] = {
    {
        "title_name",
        (PyCFunction) whisky_title_name,
        METH_NOARGS,
        whisky_title_name__doc__,
    },
    {NULL} // Sentinel
};

static PyTypeObject whisky_Type = {
    PyVarObject_HEAD_INIT(0, 0)
    .tp_name = "dublin.Whisky",
    .tp_basicsize = sizeof(WhiskyObject),
    .tp_itemsize = 0,
    .tp_new = whisky_new,
    .tp_init = (initproc) whisky_init,
    .tp_dealloc = (destructor) whisky_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = whisky_members,
    .tp_methods = whisky_methods,
};


static PyObject *
dublin_new_whisky_function(PyObject *self, PyObject *args)
{
    PyObject *whisky = NULL;
    fprintf(stdout, "dublin_new_whisky_function: %p %p\n", self, args);

    whisky = PyObject_CallFunction((PyObject *)&whisky_Type, NULL);

    if (whisky == NULL) {
        return NULL;
    }

    if (!PyObject_TypeCheck(whisky, &whisky_Type)) {
        PyErr_Format(PyExc_TypeError, "Ce n'est pas un type Whisky %.100s", Py_TYPE(whisky)->tp_name);
        Py_DECREF(whisky);
        return NULL;
    }
    fprintf(stdout, "type: %s\n", Py_TYPE(whisky)->tp_name);
    fprintf(stdout, "dublin_new_whisky_function: %p\n", whisky);
    return whisky;
}

PyDoc_STRVAR(
    dublin_new_whisky_function__doc__,
    "New whisky"
);

static struct PyMethodDef dublin_methods[] = {
    {
        "new_whisky",
        (PyCFunction) dublin_new_whisky_function,
        METH_NOARGS,
        dublin_new_whisky_function__doc__
    },
    {NULL, NULL, 0, NULL}
};

PyDoc_STRVAR(
    dublin_module__doc__,
    "Dublin Module"
);

static struct PyModuleDef dublin_module = {
    PyModuleDef_HEAD_INIT,
    "dublin",
    dublin_module__doc__,
    -1,
    dublin_methods,
};

PyMODINIT_FUNC
PyInit_dublin(void)
{
    PyObject *module = NULL;

    if (PyType_Ready(&whisky_Type) < 0) {
        return NULL;
    }

    module = PyModule_Create(&dublin_module);
    if (module == NULL) {
        return NULL;
    }

    Py_INCREF(&whisky_Type);
    PyModule_AddObject(module, "Whisky", (PyObject *) &whisky_Type);

    return module;
}