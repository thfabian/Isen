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

    // PySolver
    class_<Isen::PySolver>("Solver", "Solver's Docstring", init<boost::python::optional<const char*>>())
        .def("init", &Isen::PySolver::init, PySolver_overload_init())
        .def("run", &Isen::PySolver::run)
        .def("write", &Isen::PySolver::write, PySolver_overload_write())
        .def("setVerbosity", &Isen::PySolver::setVerbosity)
        .def("print_", &Isen::PySolver::print)
        .def("topo", &Isen::PySolver::topo)
        .def("zhtold", &Isen::PySolver::zhtold)
        .def("zhtnow", &Isen::PySolver::zhtnow)
        .def("uold", &Isen::PySolver::uold)
        .def("unow", &Isen::PySolver::unow)
        .def("unew", &Isen::PySolver::unew)
        .def("sold", &Isen::PySolver::sold)
        .def("snow", &Isen::PySolver::snow)
        .def("snew", &Isen::PySolver::snew)
        .def("mtg", &Isen::PySolver::mtg)
        .def("mtgnew", &Isen::PySolver::mtgnew)
        .def("exn", &Isen::PySolver::exn)
        .def("prs", &Isen::PySolver::prs)
        .def("tau", &Isen::PySolver::tau)
        .def("prec", &Isen::PySolver::prec)
        .def("tot_prec", &Isen::PySolver::tot_prec)
        .def("qvold", &Isen::PySolver::qvold)
        .def("qvnow", &Isen::PySolver::qvnow)
        .def("qvnew", &Isen::PySolver::qvnew)
        .def("qcold", &Isen::PySolver::qcold)
        .def("qcnow", &Isen::PySolver::qcnow)
        .def("qcnew", &Isen::PySolver::qcnew)
        .def("qrold", &Isen::PySolver::qrold)
        .def("qrnow", &Isen::PySolver::qrnow)
        .def("qrnew", &Isen::PySolver::qrnew)
        .def("nrold", &Isen::PySolver::nrold)
        .def("nrnow", &Isen::PySolver::nrnow)
        .def("nrnew", &Isen::PySolver::nrnew)
        .def("ncold", &Isen::PySolver::ncold)
        .def("ncnow", &Isen::PySolver::ncnow)
        .def("ncnew", &Isen::PySolver::ncnew)
        .def("dthetadt", &Isen::PySolver::dthetadt)
        .def("sbnd1", &Isen::PySolver::sbnd1)
        .def("sbnd2", &Isen::PySolver::sbnd2)
        .def("ubnd1", &Isen::PySolver::ubnd1)
        .def("ubnd2", &Isen::PySolver::ubnd2)
        .def("qvbnd1", &Isen::PySolver::qvbnd1)
        .def("qvbnd2", &Isen::PySolver::qvbnd2)
        .def("qcbnd1", &Isen::PySolver::qcbnd1)
        .def("qcbnd2", &Isen::PySolver::qcbnd2)
        .def("qrbnd1", &Isen::PySolver::qrbnd1)
        .def("qrbnd2", &Isen::PySolver::qrbnd2)
        .def("dthetadtbnd1", &Isen::PySolver::dthetadtbnd1)
        .def("dthetadtbnd2", &Isen::PySolver::dthetadtbnd2)
        .def("nrbnd1", &Isen::PySolver::nrbnd1)
        .def("nrbnd2", &Isen::PySolver::nrbnd2)
        .def("ncbnd1", &Isen::PySolver::ncbnd1)
        .def("ncbnd2", &Isen::PySolver::ncbnd2);
}
