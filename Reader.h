//
// Created by mikayel on 10/23/21.
//

#ifndef SIMPLE_DB_READER_H
#define SIMPLE_DB_READER_H
namespace simple_db
{

/// ConsoleReader class - manages reading operation from console. Is to be "feed" to ReadCommand function.
class ConsoleReader
{
public:
    [[nodiscard]]
    static std::string Read()
    {
        std::string commandHolder;
        std::getline(std::cin, commandHolder);
        return commandHolder;
    }
};


/// ReadCommand - reads command via specific Reader class and return.
/// \tparam Reader - reader class traits.
/// \param reader - instance of reader class which may have state and reading attributes.
/// \return Bytecode which was read from reader.
/// \usage - typical usage of this function:
///             const auto&& opcodes = ReadCommand(ImplementedReader());
template <typename Reader>
[[nodiscard]]
std::string ReadCommand(Reader&& reader)
{
    return reader.Read();
}

} // namespace simple_db
#endif //SIMPLE_DB_READER_H
