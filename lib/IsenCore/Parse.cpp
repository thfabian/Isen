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

#include <Isen/Logger.h>
#include <Isen/Parse.h>
#include <Isen/Terminal.h>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

ISEN_NAMESPACE_BEGIN

using namespace Terminal;

void internal::printDiagnostic(int row,
                               int col,
                               const std::string& filename,
                               const std::string& explanation,
                               bool isError,
                               std::string line) noexcept
{
    std::size_t terminalWidth = Terminal::getWidth();
    const std::size_t Indent = 5;

    // Location
    std::cout << Color::getFileColor() << filename << ":" << row << ":" << col << ": ";

    col--; // 0 based again

    // Explanation
    if(isError)
        std::cout << Color(Color::BoldRed) << "error: ";
    else
        std::cout << Color(Color::BoldMagenta) << "warning: ";

    std::cout << Color::getFileColor() << explanation << "\n";
    terminalWidth -= Indent;

    // Give up if the terminal is too small
    if(line.size() == 0 || terminalWidth < 10)
        return;

    // Format too long lines by truncating parts of it
    if(line.size() > terminalWidth)
    {
        std::stringstream lineStream;
        int lowerBound = col - terminalWidth / 2;
        int upperBound = col + terminalWidth / 2;

        if(lowerBound < 0)
        {
            // only print the beginning of the line
            lineStream << line.substr(0, terminalWidth - 4 - 1);
            lineStream << " ...";
        }
        else if(upperBound > line.size())
        {
            // only print the end of the line
            lineStream << "... ";
            lineStream << line.substr(line.size() - (terminalWidth - 4 - 1));
            col = col - (line.size() - (terminalWidth - 1));
        }
        else
        {
            lineStream << "... ";
            lineStream << line.substr(lowerBound, (terminalWidth - 8 - 1));
            lineStream << " ...";
            col = col - (lowerBound - 4);
        }
        line = lineStream.str();
    }

    std::cout << std::string(Indent, ' ') << line;
    if(line.back() != '\n')
        std::cout << "\n";

    // Position hint
    std::cout << std::string(col + Indent, ' ');
    std::cout << Color(Color::BoldGreen) << "^" << std::endl;
}

ParserException::ParserException(
    int row, int col, const std::string& filename, const std::string& explanation, const std::string& line)
    : row_(row + 1), col_(col + 1), filename_(filename), explanation_(explanation), line_(line)
{
}

void ParserException::print() const noexcept
{
    internal::printDiagnostic(row_, col_, filename_, explanation_, true, line_);
}


Parser::Parser(bool printErrors) : Parser(Style::UNKNOWN, printErrors)
{
}

Parser::Parser(Style style, bool prettyPrintErrors)
    : prettyPrintErrors_(prettyPrintErrors), style_(style), tokeninzer_(" ", "=+-*/")
{

    // Initialize all the known variables
    #define ADD_KNOWN_VARIABLE(name) addVariable<decltype(NameList().name)>(#name)

    ADD_KNOWN_VARIABLE(run_name);
    ADD_KNOWN_VARIABLE(iout);
    ADD_KNOWN_VARIABLE(iiniout);
    ADD_KNOWN_VARIABLE(xl);
    ADD_KNOWN_VARIABLE(nx);
    ADD_KNOWN_VARIABLE(dx);
    ADD_KNOWN_VARIABLE(thl);
    ADD_KNOWN_VARIABLE(nz);
    ADD_KNOWN_VARIABLE(time);
    ADD_KNOWN_VARIABLE(dt);
    ADD_KNOWN_VARIABLE(diff);
    ADD_KNOWN_VARIABLE(topomx);
    ADD_KNOWN_VARIABLE(topowd);
    ADD_KNOWN_VARIABLE(topotim);
    ADD_KNOWN_VARIABLE(u00);
    ADD_KNOWN_VARIABLE(bv00);
    ADD_KNOWN_VARIABLE(th00);
    ADD_KNOWN_VARIABLE(ishear);
    ADD_KNOWN_VARIABLE(k_shl);
    ADD_KNOWN_VARIABLE(k_sht);
    ADD_KNOWN_VARIABLE(u00_sh);
    ADD_KNOWN_VARIABLE(nab);
    ADD_KNOWN_VARIABLE(diffabs);
    ADD_KNOWN_VARIABLE(irelax);
    ADD_KNOWN_VARIABLE(nb);
    ADD_KNOWN_VARIABLE(idbg);
    ADD_KNOWN_VARIABLE(iprtcfl);
    ADD_KNOWN_VARIABLE(itime);
    ADD_KNOWN_VARIABLE(imoist);
    ADD_KNOWN_VARIABLE(imoist_diff);
    ADD_KNOWN_VARIABLE(imicrophys);
    ADD_KNOWN_VARIABLE(idthdt);
    ADD_KNOWN_VARIABLE(iern);
    ADD_KNOWN_VARIABLE(vt_mult);
    ADD_KNOWN_VARIABLE(autoconv_th);
    ADD_KNOWN_VARIABLE(autoconv_mult);
    ADD_KNOWN_VARIABLE(sediment_on);

    #undef ADD_KNOWN_VARIABLE

    // Initialze the ignored variables (like constants and computed variables)
    #define ADD_IGNORE_VARIABLE(name) addIgnoreVariable(#name)

    ADD_IGNORE_VARIABLE(dth);
    ADD_IGNORE_VARIABLE(nts);
    ADD_IGNORE_VARIABLE(nout);
    ADD_IGNORE_VARIABLE(nx1);
    ADD_IGNORE_VARIABLE(nz1);
    ADD_IGNORE_VARIABLE(nxb);
    ADD_IGNORE_VARIABLE(nxb1);
    ADD_IGNORE_VARIABLE(g);
    ADD_IGNORE_VARIABLE(cp);
    ADD_IGNORE_VARIABLE(r);
    ADD_IGNORE_VARIABLE(r_v);
    ADD_IGNORE_VARIABLE(rdcp);
    ADD_IGNORE_VARIABLE(cpdr);
    ADD_IGNORE_VARIABLE(pref);
    ADD_IGNORE_VARIABLE(z00);
    ADD_IGNORE_VARIABLE(prs00);
    ADD_IGNORE_VARIABLE(exn00);

    #undef ADD_IGNORE_VARIABLE
}

bool Parser::isIdentifier(const std::string& str) const
{
    return boost::regex_match(str, patIdentifier_);
}

bool Parser::isOperator(const std::string& str) const
{
    return boost::regex_match(str, patOperator_);
}

bool Parser::isNumber(const std::string& str) const
{
    return boost::regex_match(str, patNumber_);
}

bool Parser::isEmptyOrSpaceOnly(const std::string& str) const
{
    return (str.empty() || boost::regex_match(str, patOnlySpace_));
}

std::string Parser::extractString(const std::string& str) const
{
    boost::smatch match;
    if(boost::regex_search(str, match, patExtractStr_))
        if(match.size() > 1 && match[1].matched)
            return match[1];
    throw std::invalid_argument((boost::format("invalid string '%s'") % str).str());
}

bool Parser::stringToBoolean(const std::string& str) const
{
    if(str == "1" || str == "True" || str == "true")
        return true;
    if(str == "0" || str == "False" || str == "false")
        return false;
    throw std::invalid_argument((boost::format("invalid boolean value '%s'") % str).str());
}

void Parser::init(const std::string& filename, bool skipPathNormalization)
{
    // Extract filename and expand to absolute path
    if(!skipPathNormalization)
    {
        boost::filesystem::path p(filename);
        p = boost::filesystem::absolute(p);
        p = boost::filesystem::canonical(p);
        filename_ = p.string();
    }
    else
        filename_ = filename;

    // Detect parsing style
    if(style_ == Style::UNKNOWN)
    {
        std::string ext = boost::filesystem::path(filename).extension().string();

        if(ext == ".py")
            style_ = Style::PYTHON;
        else if(ext == ".m")
            style_ = Style::MATLAB;
        else
            throw IsenException("parser couldn't deduce parsing style from file extension: %s", filename);
    }

    if(style_ == Style::PYTHON)
    {
        commentEscape_ = '#';
        lineEnd_ = "#;";
        patExtractStr_ = boost::regex(R"(['"]{1}(.*)['"]{1})");
    }
    else // Style::MATLAB
    {
        commentEscape_ = '%';
        lineEnd_ = "%;";
        patExtractStr_ = boost::regex(R"([']{1}(.*)[']{1})");
    }

    patIdentifier_ = boost::regex(R"([_[:alpha:]]\w*)");
    patOnlySpace_ = boost::regex(R"(^\s*$)");
    patOperator_ = boost::regex(R"(\+|-|\*|/)");
    patNumber_ = boost::regex(R"(^-?\d+\.?\d*$)");

    // Reset running variables
    row_ = 0;
    line_.clear();
    parsedMap_.clear();
}

inline bool isBoolean(const boost::any& operand)
{
    return (operand.type() == typeid(bool));
}

inline bool isInteger(const boost::any& operand)
{
    return (operand.type() == typeid(int));
}

inline bool isFloat(const boost::any& operand)
{
    return (operand.type() == typeid(double));
}

inline bool isString(const boost::any& operand)
{
    return (operand.type() == typeid(std::string));
}

std::string Parser::evalExpression(std::vector<std::string>::iterator first,
                                   std::vector<std::string>::iterator end,
                                   const std::string& line)
{
    auto parserError = [&](int pos, const std::string& msg) { throw ParserException(row_, pos, filename_, msg, line); };

    auto getPositionInLine = [&](const std::string& str) -> std::size_t { return line.find(str); };

    std::string cur = *(end - 1);
    if(isOperator(cur))
        parserError(getPositionInLine(cur), (boost::format("expected variable after '%s'") % cur).str());

    // The evaluation will convert everything to double
    double result = 1.0f;

    std::string opLeft, opRight, opOperator;

    // Evaluate the expression 'opLeft opOperator opRight'
    auto evalExpr = [&](bool handleUnaryMinus) -> double {
        if(!isOperator(opOperator))
            parserError(getPositionInLine(opOperator),
                        (boost::format("invalid binary operator '%s'") % opOperator).str());

        auto assign = [&](const std::string& op) -> double {
            // String to double
            if(isNumber(op))
                return std::stof(op);
            // Is it a refrence to an already parsed variable?
            else if(parsedMap_.count(op))
            {
                boost::any var = parsedMap_[op];
                if(isFloat(var))
                    return boost::any_cast<double>(var);
                else if(isInteger(var))
                    return static_cast<double>(boost::any_cast<int>(var));
                else if(isBoolean(var))
                    return static_cast<double>(boost::any_cast<bool>(var));
                else
                    parserError(getPositionInLine(op), "strings are not supported in expressions");
            }
            // Variable is not known
            else
                parserError(getPositionInLine(op), (boost::format("'%s' was not declared in this scope") % op).str());
            return 0; // Make compiler happy
        };

        double left = assign(opLeft);
        double right = assign(opRight);

        if(handleUnaryMinus)
            left *= -1;

        switch(opOperator[0])
        {
            case '+':
                return (left + right);
            case '-':
                return (left - right);
            case '*':
                return (left * right);
            case '/':
                return (left / right);
        }
        assert(0 && "unreachale code");
        return 0;
    };

    auto len = std::distance(first, end);

    // Unary expression
    if(len == 2)
    {
        opLeft = *(first + 1);
        opOperator = "+";
        opRight = "0";

        if(*first == "*" || *first == "/")
            parserError(getPositionInLine(*first), (boost::format("invalid unary operator '%s'") % (*first)).str());

        result = evalExpr(*first == "-");
    }
    // Binary expression
    else if(len <= 4)
    {
        auto it = end;
        opRight = *--it;
        opOperator = *--it;
        opLeft = *--it;

        result = evalExpr(*first == "-");
    }
    // Nested expression
    else
    {
        auto it = end;
        opRight = *--it;
        opOperator = *--it;
        opLeft = *--it;
        result = evalExpr(false);

        while(it != first)
        {
            opRight = std::to_string(result);
            opOperator = *--it;
            opLeft = *--it;

            if((it - 1) == first)
            {
                --it;
                if(!isOperator(*it) || *it == "*" || *it == "/")
                    parserError(getPositionInLine(*it), (boost::format("invalid unary operator '%s'") % (*it)).str());
            }
            result = evalExpr(*first == "-");
        }
    }
    return std::to_string(result);
}

void Parser::parseAssignment(std::shared_ptr<NameList>& nameList)
{
    // Remove comments from line and the end of line token ';'
    std::string line = line_.substr(0, line_.find_first_of(lineEnd_));

    if(isEmptyOrSpaceOnly(line))
        return;

    auto getPositionInLine = [&](const std::string& str) -> std::size_t { return line.find(str); };

    auto parserError = [&](int pos, const std::string& msg) { throw ParserException(row_, pos, filename_, msg, line); };

    // Tokenize the line
    std::vector<std::string> tokens = tokeninzer_.tokenize(line);

    // Is it an assigment?
    auto itAssignment = std::find(tokens.begin(), tokens.end(), "=");

    // Not an assigment -> skip
    if(itAssignment == tokens.end())
        return;

    // Check if token '=' is at the correct position
    switch(std::distance(tokens.begin(), itAssignment))
    {
        case 0:
            parserError(getPositionInLine("="), "expected variable before '='");
        case 1:
            break;
        default:
            parserError(getPositionInLine(tokens[1]),
                        (boost::format("invalid token '%s' expected '='") % tokens[1]).str());
    }

    // Check if this is a valid variable
    std::string lhs = tokens[0];

    if(variableIgnoreSet_.count(lhs))
        return;

    if(!variableMap_.count(lhs))
        parserError(getPositionInLine(lhs), (boost::format("unknown identifier '%s'") % lhs).str());

    boost::any lhsValue = variableMap_[lhs];

    std::string rhs;
    if(tokens.size() < 3)
        parserError(getPositionInLine("="), "expected variable after '='");
    else if(tokens.size() == 3)
        rhs = tokens[2];
    // We have to deal with a calculated right-hand side e.g var = 5 * 5
    else
    {
        if(isBoolean(lhsValue))
            parserError(getPositionInLine(tokens[3]), "expressions are not supported on booleans");
        if(isString(lhsValue))
            parserError(getPositionInLine(tokens[3]), "expressions are not supported on strings");

        rhs = evalExpression(tokens.begin() + 2, tokens.end(), line);
    }
    // Perform conversion to the actual type and update the variable in the
    // namelist
    try
    {
        if(isInteger(lhsValue))
        {
            lhsValue = std::stoi(rhs);
            nameList->setByName(lhs, boost::any_cast<int>(lhsValue));
        }
        else if(isFloat(lhsValue))
        {
            lhsValue = std::stod(rhs);
            nameList->setByName(lhs, boost::any_cast<double>(lhsValue));
        }
        else if(isBoolean(lhsValue))
        {
            lhsValue = stringToBoolean(rhs);
            nameList->setByName(lhs, boost::any_cast<bool>(lhsValue));
        }
        else if(isString(lhsValue))
        {
            lhsValue = extractString(rhs);
            nameList->setByName(lhs, boost::any_cast<std::string>(lhsValue));
        }
        else
        {
            throw IsenException("internal parser error: variable '%s' has unknown type: %s", lhs,
                                lhsValue.type().name());
        }
    }
    catch(const std::invalid_argument& e)
    {
        // Try to get some meaningful error messages and rethrow
        std::string errMsg;
        if(isFloat(lhsValue))
            errMsg = (boost::format("invalid floating point value '%s'") % rhs).str();
        else if(isInteger(lhsValue))
            errMsg = (boost::format("invalid integer value '%s'") % rhs).str();
        else
            errMsg = (boost::format("conversion error: %s") % e.what()).str();

        parserError(getPositionInLine(rhs), errMsg);
    }

    auto r = parsedMap_.insert(VariableMap::value_type(lhs, lhsValue));

    // Warn if we already parsed this variable
    if(!r.second)
        internal::printDiagnostic(row_, getPositionInLine(lhs), filename_,
                                  (boost::format("redclaration of variable '%s'") % lhs).str(), false, line);
}

std::shared_ptr<NameList> Parser::parse(const std::string& filename)
{
    std::ifstream fin(filename);
    if(!fin.is_open())
        throw IsenException("no such file: %f", filename.c_str());

    // Set internals for parsing
    init(filename);

    std::shared_ptr<NameList> nameList(new NameList);

    Timer t;
    LOG() << "Parsing '" << filename << "' ... " << logger::flush;

    // Parse line-by-line
    try
    {
        while(!std::getline(fin, line_).eof())
        {
            parseAssignment(nameList);
            row_++;
        }
    }
    catch(const ParserException& pe)
    {
        LOG() << logger::failed;
        if(prettyPrintErrors_ && !LOG().isDisabled())
        {
            pe.print();
            throw IsenException("parser error");
        }
        else
            throw IsenException("syntax error in %s:%i:%i: %s", pe.getFilename(), pe.getRow(), pe.getCol(), pe.what());
    }

    nameList->update();
    style_ = Style::UNKNOWN; // Reset style
    LOG_SUCCESS(t);
    return nameList;
}

void Parser::parseSingleLine(std::shared_ptr<NameList>& nameList, const std::string& line)
{
    style_ = Style::MATLAB;
    init("command line option --namelist", true);
    line_ = line;

    try
    {
        parseAssignment(nameList);
        row_++;
    }
    catch(const ParserException& pe)
    {
        throw IsenException("syntax error in %s: %s", pe.getFilename(), pe.what());
    }

    nameList->update();
    style_ = Style::UNKNOWN;
}


ISEN_NAMESPACE_END
