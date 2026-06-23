#include <Python.h>
#include <object.h>
#include <listobject.h>
#include <bytesobject.h>
#include <stdio.h>

void print_python_bytes(PyObject *p);

/**
 * print_python_list - Prints basic details about a CPython list.
 * @p: Generic PyObject structure pointer representing a list.
 */
void print_python_list(PyObject *p)
{
	Py_ssize_t size, allocated, i;
	PyListObject *list;
	PyObject *item;

	if (!PyList_Check(p))
		return;

	list = (PyListObject *)p;
	size = ((PyVarObject *)p)->ob_size;
	allocated = list->allocated;

	printf("[*] Python list info\n");
	printf("[*] Size of the Python List = %ld\n", (long)size);
	printf("[*] Allocated = %ld\n", (long)allocated);

	for (i = 0; i < size; i++)
	{
		item = list->ob_item[i];
		printf("Element %ld: %s\n", (long)i, item->ob_type->tp_name);
		if (strcmp(item->ob_type->tp_name, "bytes") == 0)
			print_python_bytes(item);
	}
}

/**
 * print_python_bytes - Prints tracking details about a CPython bytes instance.
 * @p: Generic PyObject structure pointer representing a bytes instance.
 */
void print_python_bytes(PyObject *p)
{
	Py_ssize_t size, limit, i;
	PyBytesObject *bytes;

	printf("[.] bytes object info\n");
	if (!PyBytes_Check(p))
	{
		printf("  [ERROR] Invalid Bytes Object\n");
		return;
	}

	bytes = (PyBytesObject *)p;
	size = ((PyVarObject *)p)->ob_size;

	printf("  size: %ld\n", (long)size);
	printf("  trying string: %s\n", bytes->ob_sval);

	/* Determine maximum printing limit (up to 10 bytes total) */
	limit = size + 1;
	if (limit > 10)
		limit = 10;

	printf("  first %ld bytes:", (long)limit);
	for (i = 0; i < limit; i++)
	{
		printf(" %02x", (unsigned char)bytes->ob_sval[i]);
	}
	printf("\n");
}
