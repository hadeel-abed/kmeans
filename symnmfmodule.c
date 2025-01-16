#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>
#include "symnmf.h"

// Memory management and matrix conversion functions
double **make_input(PyObject *matrics, int rows, int cols) {
    double **new_matrics = (double **)malloc(rows * sizeof(double *));
    if (!new_matrics) {
        PyErr_NoMemory();
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        new_matrics[i] = (double *)malloc(cols * sizeof(double));
        if (!new_matrics[i]) {
            release_matrix(new_matrics, i);
            PyErr_NoMemory();
            return NULL;
        }
    }

    for (Py_ssize_t i = 0; i < rows; i++) {
        PyObject *row = PyList_GetItem(matrics, i);
        for (Py_ssize_t j = 0; j < cols; j++) {
            new_matrics[i][j] = PyFloat_AsDouble(PyList_GetItem(row, j));
        }
    }

    return new_matrics;
}

PyObject *output_func(double **matrix, int rows, int cols) {
    PyObject *result = PyList_New(rows);
    if (!result) {
        return PyErr_NoMemory();
    }

    for (Py_ssize_t i = 0; i < rows; i++) {
        PyObject *row = PyList_New(cols);
        if (!row) {
            Py_DECREF(result);
            return PyErr_NoMemory();
        }

        for (Py_ssize_t j = 0; j < cols; j++) {
            PyObject *value = PyFloat_FromDouble(matrix[i][j]);
            if (!value) {
                Py_DECREF(row);
                Py_DECREF(result);
                return PyErr_NoMemory();
            }
            PyList_SET_ITEM(row, j, value);
        }
        PyList_SET_ITEM(result, i, row);
    }
    return result;
}

// Function Wrappers
static PyObject *symnmf_run(PyObject *in1, PyObject *in2, int rows, int k) {
    double **input1 = make_input(in1, rows, k);
    if (!input1) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for input1");
        return NULL;
    }

    double **input2 = make_input(in2, rows, rows);
    if (!input2) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for input2");
        release_matrix(input1, rows); 
        return NULL;
    }

    double **optimized = optimize_H_matrix(input1, input2, rows, k, 0, 0);
    if (!optimized) {
        PyErr_SetString(PyExc_MemoryError, "Failed to optimize H matrix");
        release_matrix(input1, rows);
        release_matrix(input2, rows);
        return NULL;
    }

    PyObject *result = output_func(optimized, rows, k);
    release_matrix(optimized, rows);

    return result;
}

static PyObject *sym_run(PyObject *X, int n, int d) {
    double **A;
    PyObject *output;
    double **input = make_input(X, n, d);
    if (input == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for input matrix");
        return PyList_New(0);
    }

    A = sym(input, n, d);
    if (A == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Failed to compute similarity matrix");
        release_matrix(input, n);
        return PyList_New(0);
    }

    output = output_func(A, n, n);
    release_matrix(input, n);
    release_matrix(A, n);

    return output;
}

static PyObject *sym_func(PyObject *pyobject, PyObject *inputs) {
    int rows, cols;
    PyObject *vec;
    if (!PyArg_ParseTuple(inputs, "Oii", &vec, &rows, &cols)) {
        return NULL;
    }
    return Py_BuildValue("O", sym_run(vec, rows, cols));
}

static PyObject *symnmf_func(PyObject *pyobject, PyObject *inputs) {
    int rows, k;
    PyObject *H, *W;
    if (!PyArg_ParseTuple(inputs, "OOii", &H, &W, &rows, &k)) {
        return NULL;
    }
    return Py_BuildValue("O", symnmf_run(H, W, rows, k));
}

static PyObject *norm_run(PyObject *vec, int rows, int cols) {
    double **input = make_input(vec, rows, cols);
    if (!input) {
        return NULL;
    }

    double **normalized_matrix = compute_normalized_matrix(input, rows, cols);
    if (!normalized_matrix) {
        release_matrix(input, rows);
        return NULL;
    }

    PyObject *result = output_func(normalized_matrix, rows, rows);
    release_matrix(input, rows);
    return result;
}

static PyObject *ddg_run(PyObject *vec, int rows, int cols) {
    double **input = make_input(vec, rows, cols);
    if (!input) {
        return NULL;
    }

    double **myDdg = ddg(input, rows, cols);
    if (!myDdg) {
        release_matrix(input, rows);
        return NULL;
    }

    PyObject *output = output_func(myDdg, rows, rows);
    release_matrix(input, rows);
    return output;
}

static PyObject *ddg_func(PyObject *pyobject, PyObject *inputs) {
    int rows, cols;
    PyObject *vec;
    if (!PyArg_ParseTuple(inputs, "Oii", &vec, &rows, &cols)) {
        return NULL;
    }
    return Py_BuildValue("O", ddg_run(vec, rows, cols));
}

static PyObject *norm_func(PyObject *pyobject, PyObject *inputs) {
    int rows, cols;
    PyObject *vec;
    if (!PyArg_ParseTuple(inputs, "Oii", &vec, &rows, &cols)) {
        return NULL;
    }
    return Py_BuildValue("O", norm_run(vec, rows, cols));
}

// Module definition and initialization
static PyMethodDef functions[] = {
    {"sym", (PyCFunction)sym_func, METH_VARARGS, "Compute the similarity matrix"},
    {"ddg", (PyCFunction)ddg_func, METH_VARARGS, "Compute the diagonal degree matrix"},
    {"norm", (PyCFunction)norm_func, METH_VARARGS, "Compute the normalized similarity matrix"},
    {"symnmf", (PyCFunction)symnmf_func, METH_VARARGS, "Perform symmetric non-negative matrix factorization"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef capi_module = {
    PyModuleDef_HEAD_INIT,
    "capimodule",
    NULL,
    -1,
    functions
};

PyMODINIT_FUNC
PyInit_symnmfmodule(void)
{
    PyObject *m;
    m = PyModule_Create(&capi_module);
    if (m){return m;}
    return NULL;
}