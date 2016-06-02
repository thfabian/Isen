/**
 *                       _________ _______   __
 *                      /  _/ ___// ____/ | / /
 *                      / / \__ \/ __/ /  |/ /
 *                    _/ / ___/ / /___/ /|  /
 *                   /___//____/_____/_/ |_/
 *
 *  Isentropic model - ETH Zurich
 *  Copyright (C) 2016  Fabian Thuering (thfabian@student.ethz.ch)
 *
 *  This file is distributed under the MIT Open Source License. See LICENSE.TXT for details.
 */

#include <Isen/Output.h>
#include <Isen/Python/IsenPython.h>
#include <Isen/Python/PySolver.h>
#include <Isen/Python/PyLogger.h>

ISEN_NAMESPACE_BEGIN

void translateIsenException(const IsenException& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

ISEN_NAMESPACE_END

// Python3 init methods have to return void and NumPy returns
#if PY_VERSION_HEX >= 0x03000000
#undef NUMPY_IMPORT_ARRAY_RETVAL
#define NUMPY_IMPORT_ARRAY_RETVAL
#endif

BOOST_PYTHON_MODULE(IsenPythonCxx)
{
    using namespace boost::python;

    // Import numpy
    numeric::array::set_module_and_type("numpy", "ndarray");
    import_array();

    // Version
    scope().attr("__version__") = ISEN_VERSION_STRING;

    // Archive Types
    enum_<Isen::Output::ArchiveType>("ArchiveType")
        .value("Unknown", Isen::Output::Unknown)
        .value("Text", Isen::Output::Text)
        .value("Xml", Isen::Output::Xml)
        .value("Binary", Isen::Output::Binary);

    // Exception
    register_exception_translator<Isen::IsenException>(&Isen::translateIsenException);

    // PyLogger
    class_<Isen::PyLogger>("Logger")
        .def("enable", &Isen::PyLogger::enable)
        .def("disable", &Isen::PyLogger::disable);

    // PySolver
    class_<Isen::PySolver>("Solver", "Solver's Docstring", init<boost::python::optional<const char*>>())
        .def("init", &Isen::PySolver::init, PySolver_overload_init())
        .def("run", &Isen::PySolver::run)
        .def("getField", &Isen::PySolver::getField)
        .def("write", &Isen::PySolver::write, PySolver_overload_write())
        .def("print_", &Isen::PySolver::print);
}
