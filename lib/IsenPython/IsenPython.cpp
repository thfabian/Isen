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
#include <Isen/Python/PyLogger.h>
#include <Isen/Python/PyNameList.h>
#include <Isen/Python/PyOutput.h>
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

    // PyLogger
    class_<Isen::PyLogger>("Logger").def("enable", &Isen::PyLogger::enable).def("disable", &Isen::PyLogger::disable);

     // PyNameList
    class_<Isen::PyNameList>("NameList", init<boost::python::optional<const char*>>())
        .def("__str__", &Isen::PyNameList::__str__)
        // Floating point getter/setters
        .add_property("dx", &Isen::PyNameList::get_dx, &Isen::PyNameList::set_dx)
        .add_property("thl", &Isen::PyNameList::get_thl, &Isen::PyNameList::set_thl)
        .add_property("time", &Isen::PyNameList::get_time, &Isen::PyNameList::set_time)
        .add_property("dt", &Isen::PyNameList::get_dt, &Isen::PyNameList::set_dt)
        .add_property("diff", &Isen::PyNameList::get_diff, &Isen::PyNameList::set_diff)
        .add_property("u00", &Isen::PyNameList::get_u00, &Isen::PyNameList::set_u00)
        .add_property("bv00", &Isen::PyNameList::get_bv00, &Isen::PyNameList::set_bv00)
        .add_property("th00", &Isen::PyNameList::get_th00, &Isen::PyNameList::set_th00)
        .add_property("u00_sh", &Isen::PyNameList::get_u00_sh, &Isen::PyNameList::set_u00_sh)
        .add_property("diffabs", &Isen::PyNameList::get_diffabs, &Isen::PyNameList::set_diffabs)
        .add_property("vt_mult", &Isen::PyNameList::get_vt_mult, &Isen::PyNameList::set_vt_mult)
        .add_property("autoconv_th", &Isen::PyNameList::get_autoconv_th, &Isen::PyNameList::set_autoconv_th)
        .add_property("autoconv_mult", &Isen::PyNameList::get_autoconv_mult, &Isen::PyNameList::set_autoconv_mult)
        // Integer point getter/setters
        .add_property("iout", &Isen::PyNameList::get_iout, &Isen::PyNameList::set_iout)
        .add_property("xl", &Isen::PyNameList::get_xl, &Isen::PyNameList::set_xl)
        .add_property("nx", &Isen::PyNameList::get_nx, &Isen::PyNameList::set_nx)
        .add_property("nz", &Isen::PyNameList::get_nz, &Isen::PyNameList::set_nz)
        .add_property("topomx", &Isen::PyNameList::get_topomx, &Isen::PyNameList::set_topomx)
        .add_property("topowd", &Isen::PyNameList::get_topowd, &Isen::PyNameList::set_topowd)
        .add_property("topotim", &Isen::PyNameList::get_topotim, &Isen::PyNameList::set_topotim)
        .add_property("k_shl", &Isen::PyNameList::get_k_shl, &Isen::PyNameList::set_k_shl)
        .add_property("k_sht", &Isen::PyNameList::get_k_sht, &Isen::PyNameList::set_k_sht)
        .add_property("nab", &Isen::PyNameList::get_nab, &Isen::PyNameList::set_nab)
        .add_property("nb", &Isen::PyNameList::get_nb, &Isen::PyNameList::set_nb)
        .add_property("imicrophys", &Isen::PyNameList::get_imicrophys, &Isen::PyNameList::set_imicrophys)
        // Boolean point getter/setters
        .add_property("iiniout", &Isen::PyNameList::get_iiniout, &Isen::PyNameList::set_iiniout)
        .add_property("ishear", &Isen::PyNameList::get_ishear, &Isen::PyNameList::set_ishear)
        .add_property("irelax", &Isen::PyNameList::get_irelax, &Isen::PyNameList::set_irelax)
        .add_property("iprtcfl", &Isen::PyNameList::get_iprtcfl, &Isen::PyNameList::set_iprtcfl)
        .add_property("itime", &Isen::PyNameList::get_itime, &Isen::PyNameList::set_itime)
        .add_property("imoist", &Isen::PyNameList::get_imoist, &Isen::PyNameList::set_imoist)
        .add_property("imoist_diff", &Isen::PyNameList::get_imoist_diff, &Isen::PyNameList::set_imoist_diff)
        .add_property("idthdt", &Isen::PyNameList::get_idthdt, &Isen::PyNameList::set_idthdt)
        .add_property("iern", &Isen::PyNameList::get_iern, &Isen::PyNameList::set_iern)
        .add_property("sediment_on", &Isen::PyNameList::get_sediment_on, &Isen::PyNameList::set_sediment_on)
        // String point getter/setters
        .add_property("run_name", &Isen::PyNameList::get_run_name, &Isen::PyNameList::set_run_name);

    // PySolver
    class_<Isen::PySolver>("Solver")
        .def(init<boost::python::optional<const char*>>())
        .def("init", &Isen::PySolver::initWithFile, PySolver_overload_init())
        .def("init", &Isen::PySolver::initWithNameList)
        .def("run", &Isen::PySolver::run)
        .def("getField", &Isen::PySolver::getField)
        .def("getNameList", &Isen::PySolver::getNameList)
        .def("write", &Isen::PySolver::write, PySolver_overload_write());
}
