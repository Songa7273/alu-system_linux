#include <Python.h>
#include <stdio.h>
#include <limits.h>

void print_python_int(PyObject *p)
{
    unsigned long int value = 0;
    unsigned long int limit = ULONG_MAX;
    Py_ssize_t i, size;
    digit *digits;
    int negative = 0;

    printf("[.] integer object info\n");

    if (!PyLong_Check(p))
    {
        printf("  [ERROR] Invalid Int Object\n");
        return;
    }

    size = Py_SIZE(p);

    if (size < 0)
    {
        negative = 1;
        size = -size;
    }

    digits = ((PyLongObject *)p)->ob_digit;

    for (i = size - 1; i >= 0; i--)
    {
        if (value > (limit >> PyLong_SHIFT))
        {
            printf("  C unsigned long int overflow\n");
            return;
        }

        value = (value << PyLong_SHIFT) + digits[i];

        if (i == 0)
            break;
    }

    if (negative)
        printf("  value: -%lu\n", value);
    else
        printf("  value: %lu\n", value);
}
