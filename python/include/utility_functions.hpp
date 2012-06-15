//
// Copyright (C) 2011-12, Dynamic NDArray Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DND__UTILITY_FUNCTIONS_HPP_
#define _DND__UTILITY_FUNCTIONS_HPP_

#include <stdint.h>
#include <sstream>

#include "Python.h"

namespace pydnd {

/**
 * A container class for managing the local lifetime of
 * PyObject *.
 *
 * Throws an exception if the object passed into the constructor
 * is NULL.
 */
class pyobject_ownref {
    PyObject *m_obj;

    // Non-copyable
    pyobject_ownref(const pyobject_ownref&);
    pyobject_ownref& operator=(const pyobject_ownref&);
public:
    explicit pyobject_ownref(PyObject* obj)
        : m_obj(obj)
    {
        if (obj == NULL) {
            throw std::runtime_error("propagating a Python exception...need a mechanism to do that through Cython with a C++ exception");
        }
    }

    ~pyobject_ownref()
    {
        Py_DECREF(m_obj);
    }

    // Returns a borrowed reference
    operator PyObject *()
    {
        return m_obj;
    }
};

PyObject* intptr_array_as_tuple(int size, const intptr_t *array);

} // namespace pydnd

#endif // _DND__UTILITY_FUNCTIONS_HPP_