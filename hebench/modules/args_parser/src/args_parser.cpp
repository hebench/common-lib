// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "../include/args_parser.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <limits>
#include <string_view>

using namespace hebench;

namespace args_parser {
constexpr const char *BlankTrim = " \t\n\r\f\v";

std::vector<std::string_view> tokenize(std::string_view s, const std::string_view &delim)
{
    std::vector<std::string_view> retval;

    while (!s.empty())
    {
        auto pos = s.find(delim);
        if (pos == std::string_view::npos)
            pos = s.size();
        retval.emplace_back(s.substr(0, pos));
        s.remove_prefix(pos);
        if (!s.empty())
            s.remove_prefix(delim.size());
    }

    return retval;
}
} // namespace args_parser

ArgsParser::ArgsParser(bool bshow_help,
                       const std::string &description,
                       bool buse_exit,
                       std::size_t margin_size,
                       std::size_t line_size) :
    ArgsParser(bshow_help, description, std::string(), std::string(), buse_exit, margin_size, line_size)
{
}

ArgsParser::ArgsParser(bool bshow_help,
                       const std::string &description,
                       const std::string &epilogue,
                       bool buse_exit,
                       std::size_t margin_size,
                       std::size_t line_size) :
    ArgsParser(bshow_help, description, epilogue, std::string(), buse_exit, margin_size, line_size)
{
}

ArgsParser::ArgsParser(bool bshow_help,
                       const std::string &description,
                       const std::string &epilogue,
                       const std::string &program_name,
                       bool buse_exit,
                       std::size_t margin_size,
                       std::size_t line_size) :
    m_buse_exit(buse_exit),
    m_margin_size(margin_size),
    m_line_size(line_size),
    m_help_id(std::numeric_limits<std::size_t>::max()),
    m_program_name(program_name),
    m_description(description),
    m_epilogue(epilogue)
{
    if (!m_description.empty())
        m_description = ArgsParser::fixHelpText(m_description, 0, m_line_size);
    if (!m_epilogue.empty())
        m_epilogue = ArgsParser::fixHelpText(m_epilogue, 0, m_line_size);

    if (bshow_help)
    {
        std::string help_text = (m_margin_size <= 0 ? std::string(4, ' ') : std::string()) + "Shows this help.";
        addArgument({ "-h", "/h", "\\h", "--help", "/help", "\\help" }, 0,
                    std::string(), help_text);
        m_help_id = findArgID("-h");
    }
}

std::size_t ArgsParser::addPositionalArgument(const std::string &arg_name, const std::string &help_text)
{
    m_positional_args.emplace_back(arg_name, fixHelpText(help_text));
    return m_positional_args.size() - 1;
}

void ArgsParser::addArgument(const std::string &arg, std::size_t n, const std::string &params_help, const std::string &help_text)
{
    add({ arg }, n, params_help, help_text);
}

void ArgsParser::addArgument(const std::string &arg0, const std::string &arg1, std::size_t n, const std::string &params_help,
                             const std::string &help_text)
{
    add({ arg0, arg1 }, n, params_help, help_text);
}

void ArgsParser::addArgument(const std::string &arg0, const std::string &arg1, const std::string &arg2, std::size_t n,
                             const std::string &params_help, const std::string &help_text)
{
    add({ arg0, arg1, arg2 }, n, params_help, help_text);
}

void ArgsParser::addArgument(const std::initializer_list<std::string> &args, std::size_t n, const std::string &params_help,
                             const std::string &help_text)
{
    add(args, n, params_help, help_text);
}

void ArgsParser::addArgument(const std::vector<std::string> &args, std::size_t n, const std::string &params_help,
                             const std::string &help_text)
{
    add(args, n, params_help, help_text);
}

void ArgsParser::parse(int argc, const char *const argv[], int start_index)
{
    if (start_index < 0)
        start_index = 0;
    if (argc < 0)
        argc = 0;
    if (argc < start_index)
        throw std::invalid_argument("Not enough arguments.");

    if (m_program_name.empty())
    {
        // get program name if available
        if (argc > 0 && start_index > 0)
            m_program_name = std::filesystem::path(argv[0]).filename();
    } // end if

    if (m_program_name.empty())
        m_program_name = DefaultProgramName;

    int i = start_index;
    while (i < argc)
    {
        std::string sarg = argv[i];
        try
        {
            // retrieve option argument (will throw if it is not)
            args_unique_id id = findArgID(sarg);
            // check if argument is help
            if (checkShowHelp(id))
            {
                i = argc;
            } // end if
            else
            {
                // parse argument

                m_set_args.insert(id); // argument passed
                // check if it requires values
                if (!m_map_values[id].empty())
                {
                    std::vector<std::string> &values = m_map_values[id];
                    if (i + static_cast<int>(values.size()) >= argc)
                        throw std::logic_error("Insufficient number of parameters for argument \"" + sarg + "\".");
                    for (std::size_t j = 0; j < values.size(); ++j)
                        values[j] = argv[i + j + 1];
                    i += static_cast<int>(values.size());
                } // end if
            } // end else
        }
        catch (InvalidArgument &)
        {
            if (m_positional_values.size() >= m_positional_args.size())
                // all positional arguments have been parsed,
                // and this is not a known option argument
                throw;
            // parse positional argument
            m_positional_values.emplace_back(std::move(sarg));
        }
        ++i;
    } // end while
}

bool ArgsParser::isArgumentValid(const std::string &arg) const
{
    return m_map_args.count(arg) > 0;
}

bool ArgsParser::hasArgument(const std::string &arg) const
{
    return hasArgument(findArgID(arg));
}

bool ArgsParser::hasArgument(args_unique_id id) const
{
    return m_set_args.count(id) > 0;
}

bool ArgsParser::hasValue(const std::string &arg) const
{
    args_unique_id id = findArgID(arg);
    return hasArgument(id) && !m_map_values.at(id).empty();
}

const std::vector<std::string> &ArgsParser::getValue(const std::string &arg) const
{
    return m_map_values.at(findArgID(arg));
}

const std::string &ArgsParser::getPositionalValue(std::size_t arg_position) const
{
    return m_positional_values.at(arg_position);
}

void ArgsParser::add(const std::vector<std::string> &args, std::size_t n, const std::string &params_help, const std::string &help_text)
{
    if (args.empty())
        throw std::invalid_argument("Invalid empty arguments.");
    args_unique_id id        = m_map_values.size();
    std::string help_text_id = args.front();
    std::vector<std::string> arr_help_text;
    arr_help_text.reserve(args.size() + 3);
    // map all parsable argument names for this argument to a single ID
    for (const std::string &s : args)
    {
        if (m_map_args.count(s) > 0)
            throw std::invalid_argument("Invalid duplicated argument: \"" + s + "\".");
        m_map_args[s] = id;
        arr_help_text.emplace_back(s);
    }
    arr_help_text.emplace_back();
    arr_help_text.emplace_back(params_help);
    arr_help_text.emplace_back(fixHelpText(help_text));
    m_map_help[help_text_id] = arr_help_text;

    m_map_values[id] = std::vector<std::string>(n);
}

ArgsParser::args_unique_id ArgsParser::findArgID(const std::string &arg) const
{
    if (!isArgumentValid(arg))
        throw InvalidArgument("Invalid argument: \"" + arg + "\".");
    return m_map_args.at(arg);
}

std::string ArgsParser::fixHelpText(const std::string &original) const
{
    return ArgsParser::fixHelpText(original, this->m_margin_size, this->m_line_size);
}

std::string ArgsParser::fixHelpText(const std::string &original,
                                    std::size_t margin_size,
                                    std::size_t line_size)
{
    // rearrange help text for an argument to fit the specified margin and line size
    std::stringstream retval;

    // split original text into its original lines
    std::vector<std::string_view> tokens =
        args_parser::tokenize(std::string_view(original.c_str(), original.size()), "\n");

    std::size_t actual_line_size = line_size > 0 ? line_size - margin_size : 0;
    for (std::size_t i = 0; i < tokens.size(); ++i)
    {
        // process each original line

        // split original line into multiple lines respecting specified
        // margin and line legth
        std::string_view token = tokens[i];
        std::vector<std::string_view> lines;
        if (i > 0)
            // next line
            retval << std::endl;
        bool b_first_line = true;
        while (!token.empty())
        {
            std::string_view line;
            // absolute cut of the line
            if (actual_line_size <= 0 || token.size() <= actual_line_size)
                line = token;
            else
            {
                line = token.substr(0, actual_line_size);
                // find first blank in the line
                auto blank_pos = line.find_last_of(args_parser::BlankTrim);
                if (blank_pos == std::string_view::npos)
                    // no blanks found in absolute line cut, so, split at the first blank
                    // in remaining original line
                    line = token.substr(0, token.find_first_of(args_parser::BlankTrim));
                else
                    line = line.substr(0, blank_pos);
            } // end else

            // line is now blank cut

            // remove the blank cut line from original
            token.remove_prefix(line.size());
            if (token.find_first_of(args_parser::BlankTrim) == 0)
                token.remove_prefix(1);

            // output the blank cut line
            if (b_first_line)
                b_first_line = false;
            else
                retval << std::endl;
            retval << std::string(margin_size, ' ') << line;
        } // end while
    } // end for

    return retval.str();
}

void ArgsParser::printUsage() const
{
    printUsage(std::cout);
}

void ArgsParser::printUsage(std::ostream &os) const
{
    os << "Usage:" << std::endl;
    if (m_margin_size <= 0)
        os << "    ";
    else
        os << std::string(m_margin_size, ' ');
    os << m_program_name;
    if (!m_map_help.empty())
        os << " OPTIONS";
    if (m_positional_args.size() > 0)
    {
        std::string s_margin(m_program_name.size() + 1
                                 + (m_margin_size <= 0 ? 4 : m_margin_size),
                             ' ');
        for (std::size_t i = 0; i < m_positional_args.size(); ++i)
            os << " \\" << std::endl
               << s_margin << m_positional_args[i].first;
    } // end if
    os << std::endl;
}

bool ArgsParser::checkShowHelp(args_unique_id id)
{
    bool retval = id == m_help_id;
    if (retval)
    {
        m_map_args.clear();
        m_map_values.clear();
        m_set_args.clear();
        showHelp();
    } // end if

    return retval;
}

void ArgsParser::showHelp() const
{
    showHelp(std::cout);
}

void ArgsParser::showHelp(std::ostream &os) const
{
    // build and display the help text
    if (!m_description.empty())
        os << m_description << std::endl
           << std::endl;
    printUsage(os);
    if (m_positional_args.size() > 0)
    {
        os << std::endl
           << "POSITIONAL ARGUMENTS: " << m_positional_args.size() << std::endl;
        for (std::size_t i = 0; i < m_positional_args.size(); ++i)
        {
            os << m_positional_args[i].first << std::endl
               << m_positional_args[i].second << std::endl
               << std::endl;
        } // end for
    } // end if
    if (!m_map_help.empty())
    {
        os << std::endl
           << "OPTIONS:" << std::endl;
        for (auto help_data : m_map_help)
        {
            const std::vector<std::string> &arr_help_data = help_data.second;
            if (!arr_help_data.back().empty())
            {
                int help_idx    = -1;
                bool bhelp_text = false;
                for (std::size_t i = 0; i < arr_help_data.size(); i++)
                {
                    // if true, then help text is comming next
                    if (!bhelp_text && arr_help_data[i].empty())
                    {
                        help_idx   = 0;
                        bhelp_text = true; // help text coming next
                    } // end if
                    else
                    {
                        if (help_idx < 0 && i > 0)
                            os << ", ";
                        else if (help_idx == 0)
                            os << " ";
                        os << arr_help_data[i];
                        if (help_idx == 0)
                        {
                            os << std::endl;
                            help_idx = 1;
                        } // end if
                    } // end else
                } // end for
                os << std::endl
                   << std::endl;
            } // end if
        } // end for
    } // end if

    if (!m_epilogue.empty())
        os << std::endl
           << m_epilogue
           << std::endl;

    if (m_buse_exit)
        std::exit(0);
    else
        throw HelpShown("Help requested.");
}
