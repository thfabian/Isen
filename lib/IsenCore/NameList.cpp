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

#include <Isen/NameList.h>
#include <Isen/Terminal.h>
#include <iostream>
#include <sstream>
#include <string>

ISEN_NAMESPACE_BEGIN

void NameList::update()
{
    dth = thl / nz;
    nts = static_cast<int>(std::round(time / dt));
    nout = nts / iout + iiniout;
    nx1 = nx + 1;
    nz1 = nz + 1;
    nxb = nx + 2 * nb;
    nxb1 = nx1 + 2 * nb;
}

namespace internal
{

inline std::string printHelper(const std::string& name, int value)
{
    return (boost::format(" %-13s = %i\n") % name % value).str();
}

inline std::string printHelper(const std::string& name, bool value)
{
    return (boost::format(" %-13s = %s\n") % name % (value ? "true" : "false")).str();
}

inline std::string printHelper(const std::string& name, Float value)
{
    return (boost::format(" %-13s = %.4f\n") % name % value).str();
}

inline std::string printHelper(const std::string& name, std::string value)
{
    return (boost::format(" %-13s = %s\n") % name % value).str();
}

inline void header(std::string str)
{
    static int width = Terminal::getWidth();
    int halfSize = int(width - str.size() + 1) / 2 - 1;

    Terminal::Color white(Terminal::Color::getFileColor());
    std::cout << std::string(halfSize, '-');
    std::cout << " " << str << " ";
    std::cout << std::string(halfSize - (width % 2 ^ str.size() % 2) - 1, '-') << "\n";
}

} // namespace internal

void NameList::setByName(const std::string& name, const int& value)
{
    if(name.empty())
        return;
    else if(name == "iout")
    {
        this->iout = value;
    }
    else if(name == "xl")
    {
        this->xl = value;
    }
    else if(name == "nx")
    {
        this->nx = value;
    }
    else if(name == "nz")
    {
        this->nz = value;
    }
    else if(name == "topomx")
    {
        this->topomx = value;
    }
    else if(name == "topowd")
    {
        this->topowd = value;
    }
    else if(name == "topotim")
    {
        this->topotim = value;
    }
    else if(name == "k_shl")
    {
        this->k_shl = value;
    }
    else if(name == "k_sht")
    {
        this->k_sht = value;
    }
    else if(name == "nab")
    {
        this->nab = value;
    }
    else if(name == "nb")
    {
        this->nb = value;
    }
    else if(name == "imicrophys")
    {
        this->imicrophys = value;
    }
    else
    {
        throw IsenException("variable '%s' is not part of Namelist", name);
    }
}


void NameList::setByName(const std::string& name, const Float& value)
{
    if(name.empty())
        return;
    else if(name == "dx")
    {
        this->dx = value;
    }
    else if(name == "thl")
    {
        this->thl = value;
    }
    else if(name == "time")
    {
        this->time = value;
    }
    else if(name == "dt")
    {
        this->dt = value;
    }
    else if(name == "diff")
    {
        this->diff = value;
    }
    else if(name == "u00")
    {
        this->u00 = value;
    }
    else if(name == "bv00")
    {
        this->bv00 = value;
    }
    else if(name == "th00")
    {
        this->th00 = value;
    }
    else if(name == "u00_sh")
    {
        this->u00_sh = value;
    }
    else if(name == "diffabs")
    {
        this->diffabs = value;
    }
    else if(name == "vt_mult")
    {
        this->vt_mult = value;
    }
    else if(name == "autoconv_th")
    {
        this->autoconv_th = value;
    }
    else if(name == "autoconv_mult")
    {
        this->autoconv_mult = value;
    }
    else
    {
        throw IsenException("variable '%s' is not part of Namelist", name);
    }
}

void NameList::setByName(const std::string& name, const bool& value)
{
    if(name.empty())
        return;
    else if(name == "iiniout")
    {
        this->iiniout = value;
    }
    else if(name == "ishear")
    {
        this->ishear = value;
    }
    else if(name == "irelax")
    {
        this->irelax = value;
    }
    else if(name == "idbg")
    {
        this->idbg = value;
    }
    else if(name == "iprtcfl")
    {
        this->iprtcfl = value;
    }
    else if(name == "itime")
    {
        this->itime = value;
    }
    else if(name == "imoist")
    {
        this->imoist = value;
    }
    else if(name == "imoist_diff")
    {
        this->imoist_diff = value;
    }
    else if(name == "idthdt")
    {
        this->idthdt = value;
    }
    else if(name == "iern")
    {
        this->iern = value;
    }
    else if(name == "sediment_on")
    {
        this->sediment_on = value;
    }
    else
    {
        throw IsenException("variable '%s' is not part of Namelist", name);
    }
}

void NameList::setByName(const std::string& name, const std::string& value)
{
    if(name.empty())
        return;
    else if(name == "run_name")
    {
        this->run_name = value;
    }
    else
    {
        throw IsenException("variable '%s' is not part of Namelist", name);
    }
}


#define PRINT(Name) std::cout << internal::printHelper(#Name, this->Name)

void NameList::print() const
{
    internal::header("Output control");
    std::cout << internal::printHelper("run_name", this->run_name);
    std::cout << internal::printHelper("iout", this->iout);
    std::cout << internal::printHelper("iiniout", this->iiniout);

    internal::header("Domain size");
    std::cout << internal::printHelper("xl", this->xl);
    std::cout << internal::printHelper("nx", this->nx);
    std::cout << internal::printHelper("dx", this->dx);
    std::cout << internal::printHelper("thl", this->thl);
    std::cout << internal::printHelper("nz", this->nz);
    std::cout << internal::printHelper("time", this->time);
    std::cout << internal::printHelper("dt", this->dt);
    std::cout << internal::printHelper("diff", this->diff);

    internal::header("Topography");
    std::cout << internal::printHelper("topomx", this->topomx);
    std::cout << internal::printHelper("topowd", this->topowd);
    std::cout << internal::printHelper("topotim", this->topotim);

    internal::header("Initial atmosphere");
    std::cout << internal::printHelper("u00", this->u00);
    std::cout << internal::printHelper("bv00", this->bv00);
    std::cout << internal::printHelper("th00", this->th00);
    std::cout << internal::printHelper("ishear", this->ishear);
    std::cout << internal::printHelper("k_shl", this->k_shl);
    std::cout << internal::printHelper("k_sht", this->k_sht);
    std::cout << internal::printHelper("u00_sh", this->u00_sh);

    internal::header("Boundaries");
    std::cout << internal::printHelper("nab", this->nab);
    std::cout << internal::printHelper("diffabs", this->diffabs);
    std::cout << internal::printHelper("irelax", this->irelax);
    std::cout << internal::printHelper("nb", this->nb);

    internal::header("Print options");
    std::cout << internal::printHelper("idbg", this->idbg);
    std::cout << internal::printHelper("iprtcfl", this->iprtcfl);
    std::cout << internal::printHelper("itime", this->itime);

    internal::header("Physics: Moisture");
    std::cout << internal::printHelper("imoist", this->imoist);
    std::cout << internal::printHelper("imoist_diff", this->imoist_diff);
    std::cout << internal::printHelper("imicrophys", this->imicrophys);
    std::cout << internal::printHelper("idthdt", this->idthdt);
    std::cout << internal::printHelper("iern", this->iern);

    internal::header("Options for Kessler scheme");
    std::cout << internal::printHelper("vt_mult", this->vt_mult);
    std::cout << internal::printHelper("autoconv_th", this->autoconv_th);
    std::cout << internal::printHelper("autoconv_mult", this->autoconv_mult);
    std::cout << internal::printHelper("sediment_on", this->sediment_on);

    internal::header("Computed input parameters");
    std::cout << internal::printHelper("dth", this->dth);
    std::cout << internal::printHelper("nts", this->nts);
    std::cout << internal::printHelper("nout", this->nout);
    std::cout << internal::printHelper("nx1", this->nx1);
    std::cout << internal::printHelper("nz1", this->nz1);
    std::cout << internal::printHelper("nxb", this->nxb);
    std::cout << internal::printHelper("nxb1", this->nxb1);

    internal::header("Physical constants");
    std::cout << internal::printHelper("g", this->g);
    std::cout << internal::printHelper("cp", this->cp);
    std::cout << internal::printHelper("r", this->r);
    std::cout << internal::printHelper("r_v", this->r_v);
    std::cout << internal::printHelper("rdcp", this->rdcp);
    std::cout << internal::printHelper("cpdr", this->cpdr);
    std::cout << internal::printHelper("pref", this->pref);
    std::cout << internal::printHelper("z00", this->z00);
    std::cout << internal::printHelper("prs00", this->prs00);
    std::cout << internal::printHelper("exn00", this->exn00);
}

#undef PRINT

ISEN_NAMESPACE_END
