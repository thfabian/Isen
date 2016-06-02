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

#pragma once
#ifndef ISEN_PYTHON_NAME_LIST_H
#define ISEN_PYTHON_NAME_LIST_H

#include <Isen/Common.h>
#include <Isen/NameList.h>

ISEN_NAMESPACE_BEGIN

/// @brief Expose the Solver interface to Python
class PyNameList
{
public:
    /// @brief Parse the file and set the NameList struct accordingly
    ///
    /// @param file     File containing the definition of the NameList struct. The file must either be a valid MATLAB or
    ///                 Python file. In file is empty, default parameters are being used.
    PyNameList(const char* file = "");

    /// Initialize with already existing NameList struct
    PyNameList(std::shared_ptr<NameList> namelist);

    /// Implement pythons __str__ operator
    static std::string __str__(const PyNameList& self) noexcept;

    /// Get nameList
    std::shared_ptr<NameList> getNameList() const noexcept { return namelist_; }

private:
    std::shared_ptr<NameList> namelist_;

public:
    //-------------------------------------------------
    // Floating point getter/setters
    //-------------------------------------------------

    void set_dx(double value) const noexcept { namelist_->dx = value; }
    double get_dx() const noexcept { return namelist_->dx; }

    void set_thl(double value) const noexcept { namelist_->thl = value; }
    double get_thl() const noexcept { return namelist_->thl; }

    void set_time(double value) const noexcept { namelist_->time = value; }
    double get_time() const noexcept { return namelist_->time; }

    void set_dt(double value) const noexcept { namelist_->dt = value; }
    double get_dt() const noexcept { return namelist_->dt; }

    void set_diff(double value) const noexcept { namelist_->diff = value; }
    double get_diff() const noexcept { return namelist_->diff; }

    void set_u00(double value) const noexcept { namelist_->u00 = value; }
    double get_u00() const noexcept { return namelist_->u00; }

    void set_bv00(double value) const noexcept { namelist_->bv00 = value; }
    double get_bv00() const noexcept { return namelist_->bv00; }

    void set_th00(double value) const noexcept { namelist_->th00 = value; }
    double get_th00() const noexcept { return namelist_->th00; }

    void set_u00_sh(double value) const noexcept { namelist_->u00_sh = value; }
    double get_u00_sh() const noexcept { return namelist_->u00_sh; }

    void set_diffabs(double value) const noexcept { namelist_->diffabs = value; }
    double get_diffabs() const noexcept { return namelist_->diffabs; }

    void set_vt_mult(double value) const noexcept { namelist_->vt_mult = value; }
    double get_vt_mult() const noexcept { return namelist_->vt_mult; }

    void set_autoconv_th(double value) const noexcept { namelist_->autoconv_th = value; }
    double get_autoconv_th() const noexcept { return namelist_->autoconv_th; }

    void set_autoconv_mult(double value) const noexcept { namelist_->autoconv_mult = value; }
    double get_autoconv_mult() const noexcept { return namelist_->autoconv_mult; }

    //-------------------------------------------------
    // Integer point getter/setters
    //-------------------------------------------------
    void set_iout(int value) const noexcept { namelist_->iout = value; }
    int get_iout() const noexcept { return namelist_->iout; }

    void set_xl(int value) const noexcept { namelist_->xl = value; }
    int get_xl() const noexcept { return namelist_->xl; }

    void set_nx(int value) const noexcept { namelist_->nx = value; }
    int get_nx() const noexcept { return namelist_->nx; }

    void set_nz(int value) const noexcept { namelist_->nz = value; }
    int get_nz() const noexcept { return namelist_->nz; }

    void set_topomx(int value) const noexcept { namelist_->topomx = value; }
    int get_topomx() const noexcept { return namelist_->topomx; }

    void set_topowd(int value) const noexcept { namelist_->topowd = value; }
    int get_topowd() const noexcept { return namelist_->topowd; }

    void set_topotim(int value) const noexcept { namelist_->topotim = value; }
    int get_topotim() const noexcept { return namelist_->topotim; }

    void set_k_shl(int value) const noexcept { namelist_->k_shl = value; }
    int get_k_shl() const noexcept { return namelist_->k_shl; }

    void set_k_sht(int value) const noexcept { namelist_->k_sht = value; }
    int get_k_sht() const noexcept { return namelist_->k_sht; }

    void set_nab(int value) const noexcept { namelist_->nab = value; }
    int get_nab() const noexcept { return namelist_->nab; }

    void set_nb(int value) const noexcept { namelist_->nb = value; }
    int get_nb() const noexcept { return namelist_->nb; }

    void set_imicrophys(int value) const noexcept { namelist_->imicrophys = value; }
    int get_imicrophys() const noexcept { return namelist_->imicrophys; }

    //-------------------------------------------------
    // Boolean point getter/setters
    //-------------------------------------------------
    void set_iiniout(bool value) const noexcept { namelist_->iiniout = value; }
    bool get_iiniout() const noexcept { return namelist_->iiniout; }

    void set_ishear(bool value) const noexcept { namelist_->ishear = value; }
    bool get_ishear() const noexcept { return namelist_->ishear; }

    void set_irelax(bool value) const noexcept { namelist_->irelax = value; }
    bool get_irelax() const noexcept { return namelist_->irelax; }

    void set_iprtcfl(bool value) const noexcept { namelist_->iprtcfl = value; }
    bool get_iprtcfl() const noexcept { return namelist_->iprtcfl; }

    void set_itime(bool value) const noexcept { namelist_->itime = value; }
    bool get_itime() const noexcept { return namelist_->itime; }

    void set_imoist(bool value) const noexcept { namelist_->imoist = value; }
    bool get_imoist() const noexcept { return namelist_->imoist; }

    void set_imoist_diff(bool value) const noexcept { namelist_->imoist_diff = value; }
    bool get_imoist_diff() const noexcept { return namelist_->imoist_diff; }

    void set_idthdt(bool value) const noexcept { namelist_->idthdt = value; }
    bool get_idthdt() const noexcept { return namelist_->idthdt; }

    void set_iern(bool value) const noexcept { namelist_->iern = value; }
    bool get_iern() const noexcept { return namelist_->iern; }

    void set_sediment_on(bool value) const noexcept { namelist_->sediment_on = value; }
    bool get_sediment_on() const noexcept { return namelist_->sediment_on; }

    //-------------------------------------------------
    // String point getter/setters
    //-------------------------------------------------
    void set_run_name(std::string value) const noexcept { namelist_->run_name = value; }
    std::string get_run_name() const noexcept { return namelist_->run_name; }
};

ISEN_NAMESPACE_END

#endif