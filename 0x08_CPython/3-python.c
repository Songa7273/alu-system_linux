#include <Python.h>
#include <object.h>
#include <listobject.h>
#include <bytesobject.h>
#include <floatobject.h>
#include <stdio.h>

void print_python_bytes(PyObject *p);
void print_python_float(PyObject *p);

/**
 * print_python_list - Prints basic info about a Python list object.
 * @p: A generic PyObject pointer.
 */
void print_python_list(PyObject *p)
{
	Py_ssize_t size, allocated, i;
	PyListObject *list;
	PyObject *item;

	printf("[*] Python list info\n");
	if (!PyList_Check(p))
	{
		printf("  [ERROR] Invalid List Object\n");
		fflush(stdout);
		return;
	}

	list = (PyListObject *)p;
	size = ((PyVarObject *)p)->ob_size;
	allocated = list->allocated;

	printf("[*] Size of the Python List = %ld\n", (long)size);
	printf("[*] Allocated = %ld\n", (long)allocated);

	for (i = 0; i < size; i++)
	{
		item = list->ob_item[i];
		printf("Element %ld: %s\n", (long)i, item->ob_type->tp_name);

		if (strcmp(item->ob_type->tp_name, "bytes") == 0)
			print_python_bytes(item);
		else if (strcmp(item->ob_type->tp_name, "float") == 0)
			print_python_float(item);
	}
	fflush(stdout);
}

/**
 * print_python_bytes - Prints tracking details about a Python bytes instance.
 * @p: A generic PyObject pointer.
 */
void print_python_bytes(PyObject *p)
{
	Py_ssize_t size, limit, i;
	PyBytesObject *bytes;

	printf("[.] bytes object info\n");
	if (!PyBytes_Check(p))
	{
		printf("  [ERROR] Invalid Bytes Object\n");
		fflush(stdout);
		return;
	}

	bytes = (PyBytesObject *)p;
	size = ((PyVarObject *)p)->ob_size;

	printf("  size: %ld\n", (long)size);
	printf("  trying string: %s\n", bytes->ob_sval);

	limit = size + 1;
	if (limit > 10)
		limit = 10;

	printf("  first %ld bytes:", (long)limit);
	for (i = 0; i < limit; i++)
	{
		printf(" %02x", (unsigned char)bytes->ob_sval[i]);
	}
	printf("\n");
	fflush(stdout);
}

/**
 * print_python_float - Prints basic details about a Python float object.
 * @p: A generic PyObject pointer.
 */
void print_python_float(PyObject *p)
{
	PyFloatObject *f;
	char *num_str;

	printf("[.] float object info\n");
	if (!PyFloat_Check(p))
	{
		printf("  [ERROR] Invalid Float Object\n");
		fflush(stdout);
		return;
	}

	f = (PyFloatObject *)p;

	/* Formats the float to replicate Python's precise __str__ output string */
	num_str = PyOS_double_to_string(f->ob_fval, 'r', 0, Py_DTSF_ADD_DOT_0, NULL);
	printf("  value: %s\n", num_str);
	PyMem_Free(num_str);

	fflush(stdout);
}
