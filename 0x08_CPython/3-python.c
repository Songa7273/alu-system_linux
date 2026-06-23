#include <Python.h>
#include <stdio.h>
#include <bytesobject.h>
#include <floatobject.h>

/* ================= LIST ================= */
void print_python_list(PyObject *p)
{
    PyListObject *list = (PyListObject *)p;
    Py_ssize_t size = list->ob_base.ob_size;
    Py_ssize_t i;

    printf("[*] Python list info\n");
    printf("[*] Size of the Python List = %ld\n", size);
    printf("[*] Allocated = %ld\n", list->allocated);

    for (i = 0; i < size; i++)
    {
        PyObject *item = list->ob_item[i];
        printf("Element %ld: %s\n", i, item->ob_type->tp_name);
    }
}

/* ================= BYTES ================= */
void print_python_bytes(PyObject *p)
{
    PyBytesObject *obj = (PyBytesObject *)p;
    Py_ssize_t size, i, limit;

    printf("[.] bytes object info\n");

    if (!PyBytes_Check(p))
    {
        printf("  [ERROR] Invalid Bytes Object\n");
        return;
    }

    size = obj->ob_base.ob_size;

    printf("  size: %ld\n", size);
    printf("  trying string: %s\n", obj->ob_sval);

    limit = (size >= 10) ? 10 : size + 1;

    printf("  first %ld bytes:", limit);

    for (i = 0; i < limit; i++)
        printf(" %02x", (unsigned char)obj->ob_sval[i]);

    printf("\n");
}

/* ================= FLOAT ================= */
void print_python_float(PyObject *p)
{
    PyFloatObject *obj;

    printf("[.] float object info\n");

    if (!PyFloat_Check(p))
    {
        printf("  [ERROR] Invalid Float Object\n");
        return;
    }

    obj = (PyFloatObject *)p;

    printf("  value: %.17g\n", obj->ob_fval);
}
