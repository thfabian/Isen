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

inline std::string printHelper(const std::string& name, double value)
{
    return (boost::format(" %-13s = %.4f\n") % name % value).str();
}

inline std::string printHelper(const std::string& name, std::string value)
{
    return (boost::format(" %-13s = %s\n") % name % value).str();
}

inline void header(std::ostream& out, bool color, std::string str)
{
    static int width = Terminal::getWidth();
    int halfSize = int(width - str.size() + 1) / 2 - 1;

    if(color)
        Terminal::Color white(Terminal::Color::getFileColor());
    out << std::string(halfSize, '-');
    out << " " << str << " ";
    out << std::string(halfSize - (width % 2 ^ str.size() % 2) - 1, '-') << "\n";
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
    this->update();
}

void NameList::setByName(const std::string& name, const double& value)
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
    this->update();    
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
    this->update();    
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
    this->update();    
}

void NameList::print(std::stringstream& out) const
{
    this->print(out, false);
}

void NameList::print(std::ostream& out, bool color) const
{
    internal::header(out, color, "Output control");
    out << internal::printHelper("run_name", this->run_name);
    out << internal::printHelper("iout", this->iout);
    out << internal::printHelper("iiniout", this->iiniout);

    internal::header(out, color, "Domain size");
    out << internal::printHelper("xl", this->xl);
    out << internal::printHelper("nx", this->nx);
    out << internal::printHelper("dx", this->dx);
    out << internal::printHelper("thl", this->thl);
    out << internal::printHelper("nz", this->nz);
    out << internal::printHelper("time", this->time);
    out << internal::printHelper("dt", this->dt);
    out << internal::printHelper("diff", this->diff);

    internal::header(out, color, "Topography");
    out << internal::printHelper("topomx", this->topomx);
    out << internal::printHelper("topowd", this->topowd);
    out << internal::printHelper("topotim", this->topotim);

    internal::header(out, color, "Initial atmosphere");
    out << internal::printHelper("u00", this->u00);
    out << internal::printHelper("bv00", this->bv00);
    out << internal::printHelper("th00", this->th00);
    out << internal::printHelper("ishear", this->ishear);
    out << internal::printHelper("k_shl", this->k_shl);
    out << internal::printHelper("k_sht", this->k_sht);
    out << internal::printHelper("u00_sh", this->u00_sh);

    internal::header(out, color, "Boundaries");
    out << internal::printHelper("nab", this->nab);
    out << internal::printHelper("diffabs", this->diffabs);
    out << internal::printHelper("irelax", this->irelax);
    out << internal::printHelper("nb", this->nb);

    internal::header(out, color, "Print options");
    out << internal::printHelper("idbg", this->idbg);
    out << internal::printHelper("iprtcfl", this->iprtcfl);
    out << internal::printHelper("itime", this->itime);

    internal::header(out, color, "Physics: Moisture");
    out << internal::printHelper("imoist", this->imoist);
    out << internal::printHelper("imoist_diff", this->imoist_diff);
    out << internal::printHelper("imicrophys", this->imicrophys);
    out << internal::printHelper("idthdt", this->idthdt);
    out << internal::printHelper("iern", this->iern);

    internal::header(out, color, "Options for Kessler scheme");
    out << internal::printHelper("vt_mult", this->vt_mult);
    out << internal::printHelper("autoconv_th", this->autoconv_th);
    out << internal::printHelper("autoconv_mult", this->autoconv_mult);
    out << internal::printHelper("sediment_on", this->sediment_on);

    internal::header(out, color, "Computed input parameters");
    out << internal::printHelper("dth", this->dth);
    out << internal::printHelper("nts", this->nts);
    out << internal::printHelper("nout", this->nout);
    out << internal::printHelper("nx1", this->nx1);
    out << internal::printHelper("nz1", this->nz1);
    out << internal::printHelper("nxb", this->nxb);
    out << internal::printHelper("nxb1", this->nxb1);

    internal::header(out, color, "Physical constants");
    out << internal::printHelper("g", this->g);
    out << internal::printHelper("cp", this->cp);
    out << internal::printHelper("r", this->r);
    out << internal::printHelper("r_v", this->r_v);
    out << internal::printHelper("rdcp", this->rdcp);
    out << internal::printHelper("cpdr", this->cpdr);
    out << internal::printHelper("pref", this->pref);
    out << internal::printHelper("z00", this->z00);
    out << internal::printHelper("prs00", this->prs00);
    out << internal::printHelper("exn00", this->exn00);
}

ISEN_NAMESPACE_END
