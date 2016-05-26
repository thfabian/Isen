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
#ifndef ISEN_PARSE_H
#define ISEN_PARSE_H

#include <Isen/Common.h>
#include <Isen/NameList.h>
#include <boost/any.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

ISEN_NAMESPACE_BEGIN

namespace internal
{

/// Pretty-print a detailed diagnostic message to std::cout.
void printDiagnostic(int row,
                     int col,
                     const std::string& filename,
                     const std::string& explanation,
                     bool isError = true,
                     std::string line = "") noexcept;
}

/// @brief Handle a parsing error by assembling a diagnostic string
class ParserException : public std::exception
{
public:
    /// Row and col are 0-based and converted and stored 1-based
    ParserException(
        int row, int col, const std::string& filename, const std::string& explanation, const std::string& line = "");

    virtual ~ParserException() {}

    /// Print a detailed diagnostic message to std::cout.
    void print() const noexcept;

    /// Returns the explanatory string.
    virtual const char* what() const noexcept { return explanation_.c_str(); }

    int getRow() const noexcept { return row_; }
    int getCol() const noexcept { return col_; }
    const char* getFilename() const noexcept { return filename_.c_str(); }

private:
    int row_, col_;           ///< Row and col of the error (1-based)
    std::string filename_;    ///< File currently being parsed
    std::string explanation_; ///< Explanatory string
    std::string line_;        ///< Line currently being parsed
};

/// @brief Tokenize a string
class Tokenizer
{
public:
    /// Initialze the tokenizer
    /// @param droppedDelims    Characters which are used to split the string
    /// @param keptDelims       Characters which are kept as a result
    /// @param dropEmptyTokens  Are empty tokens included?
    Tokenizer(const std::string& droppedDelims, const std::string& keptDelims = "", bool dropEmptyTokens = true)
    {
        separator_ = boost::char_separator<char>(droppedDelims.c_str(), keptDelims.c_str(),
                                                 dropEmptyTokens ? boost::drop_empty_tokens : boost::keep_empty_tokens);
    }

    /// Tokenize a string and store the result in a vector
    std::vector<std::string> tokenize(const std::string& str)
    {
        boost::tokenizer<boost::char_separator<char>> tokens(str, separator_);
        std::vector<std::string> result;
        for(auto it = tokens.begin(); it != tokens.end(); ++it)
            result.push_back(*it);
        return result;
    }

private:
    boost::char_separator<char> separator_;
};

/// @brief Parse the input file and return a NameList of the parsed variables.
///
/// The parser can read MATLAB and Python files. It will skip everything that is not an assignment, hence missing the @c
/// = character. In case of an error, the Parser will print a diagnostic message to std::cout before throwing an
/// IsenException.
///
/// @code{.cpp}
///     Parser p;
///     try {
///         std::shared_ptr<NameList> result = p.parse(file);
///     } catch(const IsenException& e) {
///         // Handle exception
///     }
/// @endcode
///
/// The parse function can be invoked multiple times on diffrent files and will return each time a newly constructed
/// NameList.
///
/// The parser is able to parse simple arithmetic expression like
/// @code{.cpp}
///     a = 5 * 5 // == 25
/// @endcode
/// aswell as more complex ones, including refrencing other variables,
/// @code{.cpp}
///     b = 5
///     a = 5 * b // == 25
/// @endcode
/// Caution: the parser simply applies the operation from right to left and hence struggles with the order of operations
/// (it cannot parse expressions which would require an expression tree), like
/// @code{.cpp}
///     a = -5 * 5 + 5 * 5 // == -150 != 0
/// @endcode
class Parser
{
public:
    using VariableMap = std::map<std::string, boost::any>;
    using IgnoreSet = std::set<std::string>;

    enum Style
    {
        UNKNOWN = 0,
        PYTHON = 1,
        MATLAB = 2
    };

    /// @brief Initialize the parser with the parsing style
    ///
    /// If no style is provided, the parser will guess the style based on file extension. In addition the option
    /// prettyPrintErrors allows to disable diagnostic messages.
    Parser(Style style = Style::UNKNOWN, bool prettyPrintErrors = true);
    explicit Parser(bool prettyPrintErrors);

    /// @brief Parse the file given by @c filename.
    ///
    /// This operation will allocate a new NameList and override the default values while they are being parsed.
    std::shared_ptr<NameList> parse(const std::string& filename);

    /// Get parsing style
    Style getStyle() const { return style_; }

    /// Set parsing style
    void setStyle(Style style) { style_ = style; }

    /// Initialize the internals of the parser (automatically called by
    /// Parser::parse)
    void init(const std::string& filename);

    /// Check whether the given string is a valid MATLAB or Python identifier
    bool isIdentifier(const std::string& str) const;

    /// Check whether the given string is empty or contains only spaces
    bool isEmptyOrSpaceOnly(const std::string& str) const;

    /// Check whether the given string is an operator {+,-,*,/}
    bool isOperator(const std::string& str) const;

    /// Check whether the given string is a number
    bool isNumber(const std::string& str) const;

    /// Extract a string surrounder by string escape characters
    std::string extractString(const std::string& str) const;

    /// Convert string to boolean
    bool stringToBoolean(const std::string& str) const;

    /// Internal parsing function to parse statements like 'lhs = rhs'
    void parseAssignment(std::shared_ptr<NameList>& nameList);

    /// Internal evaluation function to parse expressions like '5 * 5'
    std::string evalExpression(std::vector<std::string>::iterator first,
                               std::vector<std::string>::iterator end,
                               const std::string& line);

    /// Add a new variable to be recognized by the parser.
    template <typename T>
    void addVariable(const std::string& name)
    {
        auto r = variableMap_.insert(VariableMap::value_type(name, boost::any(T())));
        if(!r.second)
            throw IsenException("internal parser error: variable '%s' got registred twice", name);
    }

    /// Add a variable which will be ignored by the parser.
    void addIgnoreVariable(const std::string& name)
    {
        auto r = variableIgnoreSet_.insert(name);
        if(!r.second)
            throw IsenException("internal parser error: variable '%s' got registred twice", name);
    }

private:
    // Parsing
    bool prettyPrintErrors_; ///< Print diagnostic string if an error occurs?
    std::string filename_;   ///< File currently being parsed
    Style style_;            ///< Style of parsing
    char commentEscape_;     ///< Escape character for a Comment e.g '#'
    std::string lineEnd_;    ///< Signal end of line e.g ';' or '#'
    Tokenizer tokeninzer_;   ///< Tokenizer

    // Running variables
    std::string line_; ///< Line currently being parsed
    int row_;          ///< Row number

    // Regex
    boost::regex patIdentifier_; ///< Pattern to match identifiers
    boost::regex patOnlySpace_;  ///< Pattern to match only whitespaces
    boost::regex patOperator_;   ///< Pattern to match operators
    boost::regex patNumber_;     ///< Pattern to match numbers
    boost::regex patExtractStr_; ///< Pattern to extract a string

    // Result
    VariableMap variableMap_;     ///< Known variables
    VariableMap parsedMap_;       ///< Parsed/encountered variables
    IgnoreSet variableIgnoreSet_; ///< Ignored variables
};

ISEN_NAMESPACE_END

#endif
