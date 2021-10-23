//
// Created by mikayel on 10/23/21.
//
#ifndef SIMPLE_DB_EXECUTABLE_H
#define SIMPLE_DB_EXECUTABLE_H
#include <iostream>
#include <cassert>
#include "Utils.h"
namespace simple_db
{

enum StatementType
{
    ST_UNKNOWN,
    ST_INSERT,
    ST_SELECT
};

class Statement
{
    std::string m_commandHolder;
    StatementType m_type;

public:
    Statement(std::string commandHolder)
    : m_commandHolder(std::move(commandHolder))
    , m_type(ST_UNKNOWN)
    {
        if(m_commandHolder.compare(0, constants::c_insert.size(), constants::c_insert))
        {
            m_type = ST_INSERT;
        }
        else if (m_commandHolder.compare(0, constants::c_select.size(), constants::c_select))
        {
            m_type = ST_SELECT;
        }
    }

    bool Execute()
    {
        switch (m_type)
        {
            case ST_SELECT:
                std::cout << "This is where we would do a select.\n";
                break;
            case ST_INSERT:
                std::cout << "This is where we would do an insert.\n";
                break;
            case ST_UNKNOWN:
            default:
                std::cout << "unknown statement.\n";
        }
        return false;
    }
};

enum MetaCommandResult
{
    MCR_UNRECOGNISED_COMMAND,
    MCR_SUCCESS
};

class MetaCommand
{
    std::string m_commandHolder;
    MetaCommandResult m_result{MCR_UNRECOGNISED_COMMAND};
public:
    MetaCommand(std::string value) noexcept
    : m_commandHolder(std::move(value))
    , m_result() {}

    MetaCommandResult Execute() noexcept
    {
        if(!utils::IsMetaCommand(m_commandHolder))
        {
            return MCR_UNRECOGNISED_COMMAND;
        }
        if(m_commandHolder == ".exit")
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            std::cerr << "Unrecognised command: " << m_commandHolder << "\n";
            return MCR_UNRECOGNISED_COMMAND;
        }
        assert(false && "unreachable code");
    }
};

template <typename CommandType>
bool Execute(CommandType&& command)
{
    return command.Execute();
}

} // namespace simple_db

#endif //SIMPLE_DB_EXECUTABLE_H
