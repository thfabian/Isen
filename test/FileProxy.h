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
#ifndef ISEN_TEST_FILE_PROXY_H
#define ISEN_TEST_FILE_PROXY_H

#include <Isen/Common.h>
#include <boost/filesystem.hpp>
#include <fstream>
#include <initializer_list>
#include <string>

ISEN_NAMESPACE_BEGIN

// Create a temporary file and remove it aferwards
class ProxyFile
{
public:
    enum FileType
    {
        PYTHON = 0,
        MATLAB
    };

    ProxyFile()
    {
        boost::filesystem::path tempFile = boost::filesystem::unique_path();
        filename_ = tempFile.string();
    }

    ProxyFile(FileType fileType, std::initializer_list<std::string> lines) : ProxyFile()
    {
        char commentEscape = '#';
        switch(fileType)
        {
            case FileType::PYTHON:
                filename_ += ".py";
                commentEscape = '#';
                break;
            case FileType::MATLAB:
                filename_ += ".m";
                commentEscape = '%';
                break;
        }

        std::ofstream fout(filename_);
        int l = 0;
        for(const auto& line : lines)
            fout << line << commentEscape << " line " << (l++) << '\n';
        fout.close();
    }

    ~ProxyFile() { boost::filesystem::remove(filename_); }

    std::string getFilename() const { return filename_; }

private:
    std::string filename_;
};

ISEN_NAMESPACE_END

#endif
