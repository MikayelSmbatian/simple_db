//
// Created by mikayel on 10/23/21.
//
#ifndef SIMPLE_DB_EXECUTABLE_H
#define SIMPLE_DB_EXECUTABLE_H
#include <iostream>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>
#include <cstring>
#include <sstream>
#include "Utils.h"
namespace simple_db
{

struct Row
{
    static constexpr const uint32_t c_columnUsernameSize = 32;
    static constexpr const uint32_t c_columnEmailSize = 255;

public:
    uint32_t m_id;
    char m_username[c_columnUsernameSize];
    char m_email[c_columnEmailSize];

public:
    friend std::istream& operator>>(std::istream& in, Row& row) noexcept
    {
        return in >> row.m_id >> row.m_username >> row.m_email;
    }

    friend std::ostream& operator<<(std::ostream& out, const Row& row) noexcept
    {
        return out << row.m_id << row.m_username << row.m_email;
    }

public:
    static constexpr const size_t c_idSize = sizeof(m_id);
    static constexpr const size_t c_usernameSize = sizeof(m_username);
    static constexpr const size_t c_emailSize = sizeof(m_email);

    static constexpr const uint32_t c_offsetID = 0;
    static constexpr const uint32_t c_offsetUsername = c_offsetID + c_idSize;
    static constexpr const uint32_t c_offsetEmail = c_offsetUsername + c_usernameSize;
    static constexpr const uint32_t c_totalRowSize = c_idSize + c_usernameSize + c_emailSize;
    static constexpr const uint32_t c_pageCapacity = constants::c_pageSize / c_totalRowSize;
};

class MemoryPage
{
public:
    char m_start[constants::c_pageSize];
public:
    MemoryPage() = default;
    MemoryPage(const MemoryPage&) = delete;
    MemoryPage(MemoryPage&&) = default;
    MemoryPage& operator=(MemoryPage&&) = default;
    MemoryPage& operator=(const MemoryPage&) = delete;
    ~MemoryPage() = default;
};

class Table
{
public:
    static constexpr const size_t c_tableMaxPages = 100;
    static constexpr const size_t c_rowsPerPage = constants::c_pageSize / Row::c_totalRowSize;
    static constexpr const size_t c_tableMaxRows = c_rowsPerPage * c_tableMaxPages;
private:
    size_t m_numRows{};
    std::vector<MemoryPage> m_pages;

public:

    [[nodiscard]]
    char* GetRowSlot(size_t rowIndex, MemoryPage* hint = nullptr) noexcept
    {
        if(hint == nullptr)
        {
            const auto pageNum = rowIndex / c_rowsPerPage;
            if(pageNum >= m_pages.size())
            {
                return nullptr;
            }
            hint = &m_pages[pageNum];
        }

        const auto rowOffset = rowIndex % c_rowsPerPage;
        const auto bytesOffset = rowOffset * Row::c_totalRowSize;
        return hint->m_start + bytesOffset;
    }

    void SerializeRow(const Row& row) noexcept
    {
        int numRowsInLastPage = static_cast<int>(m_numRows) - static_cast<int>(m_pages.size() - 1) * c_rowsPerPage;
        if(numRowsInLastPage > int(c_rowsPerPage) || m_pages.empty())
        {
            m_pages.emplace_back(); // Insert new empty page.
        }
        auto destination = GetRowSlot(m_numRows, &m_pages.back());
        memcpy(destination + Row::c_offsetID, reinterpret_cast<const void *>(&row.m_id), Row::c_idSize);
        memcpy(destination + Row::c_offsetUsername, reinterpret_cast<const void *>(&row.m_username), Row::c_usernameSize);
        memcpy(destination + Row::c_offsetEmail, reinterpret_cast<const void *>(&row.m_email), Row::c_emailSize);
        ++m_numRows;
    }

    void DeserializeRow(size_t index, Row& row) noexcept
    {
        auto source = GetRowSlot(index, nullptr);
        memcpy(&row.m_id, source + Row::c_offsetID, Row::c_idSize);
        memcpy(&row.m_username, source + Row::c_offsetUsername, Row::c_usernameSize);
        memcpy(&row.m_email, source + Row::c_offsetEmail, Row::c_emailSize);
    }

    [[nodiscard]] size_t GetNumRows() const { return m_numRows; }
};

enum StatementType
{
    ST_UNKNOWN,
    ST_INSERT,
    ST_SELECT
};

enum ExecuteResult
{
    ER_NULL,
    ER_SUCCESS,
    ER_FAILURE,
    ER_FULL_TABLE
};

class Statement
{
    std::string m_commandHolder;
    StatementType m_type;
    std::shared_ptr<Table> m_table;
    std::shared_ptr<Row> m_rowToInsert;

private:
    Statement(std::string commandHolder, StatementType type,
              std::shared_ptr<Table> table, std::shared_ptr<Row> rowToInsert)
    : m_commandHolder(std::move(commandHolder))
    , m_type(type)
    , m_table(std::move(table))
    , m_rowToInsert(std::move(rowToInsert)) {}

    ExecuteResult ExecuteInsert()
    {
        if(m_table->GetNumRows() > Table::c_tableMaxRows)
        {
            return ER_FULL_TABLE;
        }
        m_table->SerializeRow(*m_rowToInsert);
        return ER_SUCCESS;
    }

    ExecuteResult ExecuteSelect()
    {
        const auto numRows = m_table->GetNumRows();
        for(int i = 0; i < numRows; ++i)
        {
            Row row{};
            m_table->DeserializeRow(i, row);
            std::cout << row << "\n";
        }
        return ER_SUCCESS;
    }

    static std::unique_ptr<Statement> PrepareInsert(const std::string& commandHolder,
                                                    const std::shared_ptr<Table>& table)
    {
        auto row = std::make_shared<Row>();
        std::istringstream formatter{commandHolder};
        {
            std::string removeInsert;
            formatter >> removeInsert;
        }
        formatter >> *row;
        if(formatter.bad())
        {
            throw std::logic_error("Syntax error");
        }
        return std::unique_ptr<Statement>(
                new Statement(commandHolder, ST_INSERT, table, row));
    }

    static std::unique_ptr<Statement> PrepareSelect(const std::string& commandHolder,
                                                    const std::shared_ptr<Table>& table)
    {
        return std::unique_ptr<Statement>{
                new Statement{commandHolder, ST_SELECT, table, nullptr}};
    }
public:

    static std::unique_ptr<Statement> Prepare(const std::string& commandHolder,
                                              const std::shared_ptr<Table>& table = nullptr)
    {
        if(commandHolder.compare(0, constants::c_insert.size(), constants::c_insert) == 0)
        {
            return PrepareInsert(commandHolder, table);
        }
        else if (commandHolder.compare(0, constants::c_select.size(), constants::c_select) == 0)
        {
            return PrepareSelect(commandHolder, table);
        }
        return nullptr;
    }

    ExecuteResult Execute()
    {
        switch (m_type)
        {
            case ST_SELECT:
                return ExecuteSelect();
            case ST_INSERT:
                return ExecuteInsert();
            case ST_UNKNOWN:
            default:
                std::cout << "unknown statement.\n";
                return ER_FAILURE;
        }
        return ER_NULL;
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
    explicit MetaCommand(std::string value) noexcept
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
