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

/// @brief Expose the NameList interface to Python
class PyNameList
{
public:
    /// @brief Parse the file and set the NameList struct accordingly
    ///
    /// @param file     File containing the definition of the NameList struct. The file must either be a valid MATLAB or
    ///                 Python file. In file is empty, default parameters are being used.
    explicit PyNameList(const char* file = "");

    /// Initialize with already existing NameList struct
    explicit PyNameList(std::shared_ptr<NameList> namelist);

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
    void set_thl(double value) const noexcept
    {
        namelist_->thl = value;
        namelist_->update();
    }
    double get_thl() const noexcept { return namelist_->thl; }

    void set_time(double value) const noexcept
    {
        namelist_->time = value;
        namelist_->update();
    }
    double get_time() const noexcept { return namelist_->time; }

    void set_dt(double value) const noexcept
    {
        namelist_->dt = value;
        namelist_->update();
    }
    double get_dt() const noexcept { return namelist_->dt; }

    void set_diff(double value) const noexcept
    {
        namelist_->diff = value;
        namelist_->update();
    }
    double get_diff() const noexcept { return namelist_->diff; }

    void set_u00(double value) const noexcept
    {
        namelist_->u00 = value;
        namelist_->update();
    }
    double get_u00() const noexcept { return namelist_->u00; }

    void set_bv00(double value) const noexcept
    {
        namelist_->bv00 = value;
        namelist_->update();
    }
    double get_bv00() const noexcept { return namelist_->bv00; }

    void set_th00(double value) const noexcept
    {
        namelist_->th00 = value;
        namelist_->update();
    }
    double get_th00() const noexcept { return namelist_->th00; }

    void set_u00_sh(double value) const noexcept
    {
        namelist_->u00_sh = value;
        namelist_->update();
    }
    double get_u00_sh() const noexcept { return namelist_->u00_sh; }

    void set_diffabs(double value) const noexcept
    {
        namelist_->diffabs = value;
        namelist_->update();
    }
    double get_diffabs() const noexcept { return namelist_->diffabs; }

    void set_vt_mult(double value) const noexcept
    {
        namelist_->vt_mult = value;
        namelist_->update();
    }
    double get_vt_mult() const noexcept { return namelist_->vt_mult; }

    void set_autoconv_th(double value) const noexcept
    {
        namelist_->autoconv_th = value;
        namelist_->update();
    }
    double get_autoconv_th() const noexcept { return namelist_->autoconv_th; }

    void set_autoconv_mult(double value) const noexcept
    {
        namelist_->autoconv_mult = value;
        namelist_->update();
    }
    double get_autoconv_mult() const noexcept { return namelist_->autoconv_mult; }

    //-------------------------------------------------
    // Integer point getter/setters
    //-------------------------------------------------
    void set_iout(int value) const noexcept
    {
        namelist_->iout = value;
        namelist_->update();
    }
    int get_iout() const noexcept { return namelist_->iout; }

    void set_xl(int value) const noexcept
    {
        namelist_->xl = value;
        namelist_->update();
    }
    int get_xl() const noexcept { return namelist_->xl; }

    void set_nx(int value) const noexcept
    {
        namelist_->nx = value;
        namelist_->update();
    }
    int get_nx() const noexcept { return namelist_->nx; }

    void set_nz(int value) const noexcept
    {
        namelist_->nz = value;
        namelist_->update();
    }
    int get_nz() const noexcept { return namelist_->nz; }

    void set_topomx(int value) const noexcept
    {
        namelist_->topomx = value;
        namelist_->update();
    }
    int get_topomx() const noexcept { return namelist_->topomx; }

    void set_topowd(int value) const noexcept
    {
        namelist_->topowd = value;
        namelist_->update();
    }
    int get_topowd() const noexcept { return namelist_->topowd; }

    void set_topotim(int value) const noexcept
    {
        namelist_->topotim = value;
        namelist_->update();
    }
    int get_topotim() const noexcept { return namelist_->topotim; }

    void set_k_shl(int value) const noexcept
    {
        namelist_->k_shl = value;
        namelist_->update();
    }
    int get_k_shl() const noexcept { return namelist_->k_shl; }

    void set_k_sht(int value) const noexcept
    {
        namelist_->k_sht = value;
        namelist_->update();
    }
    int get_k_sht() const noexcept { return namelist_->k_sht; }

    void set_nab(int value) const noexcept
    {
        namelist_->nab = value;
        namelist_->update();
    }
    int get_nab() const noexcept { return namelist_->nab; }

    void set_nb(int value) const noexcept
    {
        namelist_->nb = value;
        namelist_->update();
    }
    int get_nb() const noexcept { return namelist_->nb; }

    void set_imicrophys(int value) const noexcept
    {
        namelist_->imicrophys = value;
        namelist_->update();
    }
    int get_imicrophys() const noexcept { return namelist_->imicrophys; }

    //-------------------------------------------------
    // Boolean point getter/setters
    //-------------------------------------------------
    void set_iiniout(bool value) const noexcept
    {
        namelist_->iiniout = value;
        namelist_->update();
    }
    bool get_iiniout() const noexcept { return namelist_->iiniout; }

    void set_ishear(bool value) const noexcept
    {
        namelist_->ishear = value;
        namelist_->update();
    }
    bool get_ishear() const noexcept { return namelist_->ishear; }

    void set_irelax(bool value) const noexcept
    {
        namelist_->irelax = value;
        namelist_->update();
    }
    bool get_irelax() const noexcept { return namelist_->irelax; }

    void set_iprtcfl(bool value) const noexcept
    {
        namelist_->iprtcfl = value;
        namelist_->update();
    }
    bool get_iprtcfl() const noexcept { return namelist_->iprtcfl; }

    void set_itime(bool value) const noexcept
    {
        namelist_->itime = value;
        namelist_->update();
    }
    bool get_itime() const noexcept { return namelist_->itime; }

    void set_imoist(bool value) const noexcept
    {
        namelist_->imoist = value;
        namelist_->update();
    }
    bool get_imoist() const noexcept { return namelist_->imoist; }

    void set_imoist_diff(bool value) const noexcept
    {
        namelist_->imoist_diff = value;
        namelist_->update();
    }
    bool get_imoist_diff() const noexcept { return namelist_->imoist_diff; }

    void set_idthdt(bool value) const noexcept
    {
        namelist_->idthdt = value;
        namelist_->update();
    }
    bool get_idthdt() const noexcept { return namelist_->idthdt; }

    void set_iern(bool value) const noexcept
    {
        namelist_->iern = value;
        namelist_->update();
    }
    bool get_iern() const noexcept { return namelist_->iern; }

    void set_sediment_on(bool value) const noexcept
    {
        namelist_->sediment_on = value;
        namelist_->update();
    }
    bool get_sediment_on() const noexcept { return namelist_->sediment_on; }

    //-------------------------------------------------
    // String point getter/setters
    //-------------------------------------------------
    void set_run_name(std::string value) const noexcept
    {
        namelist_->run_name = value;
        namelist_->update();
    }
    std::string get_run_name() const noexcept { return namelist_->run_name; }
};

ISEN_NAMESPACE_END

#endif
