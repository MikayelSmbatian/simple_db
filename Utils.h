//
// Created by mikayel on 10/23/21.
//

#ifndef SIMPLE_DB_UTILS_H
#define SIMPLE_DB_UTILS_H
#include <string>

namespace simple_db::constants
{

static constexpr int c_pageSize = 4096;

using namespace std::string_literals;
/// \brief Statement types.
/// \{
const auto& c_insert = "insert"s;
const auto& c_select = "select"s;
/// \}

/// \brief Metacommand types.
/// \{
constexpr const auto& c_exit = ".exit";
/// \}

} // namespace simple_db::constants

namespace simple_db::utils
{

inline bool IsMetaCommand(const std::string& commandHolder)
{
    return commandHolder.starts_with('.');
}

} // namespace simple_db::utils

#endif //SIMPLE_DB_UTILS_H
