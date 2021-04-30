#include "Session.hpp"

#include <cerrno>
#include <fstream>
#include <sstream>
#include <system_error>

#include <iostream>

namespace paddock::io
{
namespace
{
Expected<std::string> readFileContent(const std::string& filePath)
{
    std::ifstream in(filePath);
    if (in.fail())
    {
        return tl::make_unexpected(
            std::make_error_code(static_cast<std::errc>(errno)));
    }

    std::ostringstream stream;
    stream << in.rdbuf();

    if (in.fail())
    {
        return tl::make_unexpected(
            std::make_error_code(static_cast<std::errc>(errno)));
    }

    return stream.str();
}
} // namespace

Expected<io::Session> readSession(const std::string& filePath)
{
    return readFileContent(filePath).and_then(
        [](std::string input) -> Expected<io::Session> {
            json document{std::move(input)};
            return Serializer<io::Session>::deserialize(document);
        });
}

} // namespace paddock::io
