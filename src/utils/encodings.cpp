#include "encodings.hpp"

#include "byte.hpp"

namespace paddock
{
namespace
{
void merge(std::byte& byte, unsigned char mask)
{
    byte = static_cast<std::byte>(static_cast<unsigned char>(byte) | mask);
}
} // namespace

std::vector<std::byte> to7bitEncoding(std::span<const std::byte> input)
{
    std::vector<std::byte> result((input.size() * 8 + 6) / 7, 0x00_b);

    auto in = input.begin();
    auto out = result.begin();

    while (in != input.end())
    {
        for (int i = 1; i != 8; ++i, ++in)
        {
            if (in == input.end())
                return result;

            const auto byte = static_cast<unsigned char>(*in);
            merge(*out++, byte >> i);
            merge(*out, (byte << (7 - i)) & '\x7F');
        }
        ++out;
    }
    return result;
}

std::vector<std::byte> from7bitEncoding(std::span<const std::byte> input)
{
    std::vector<std::byte> result((input.size() + 1) * 7 / 8, 0x00_b);

    auto in = input.begin();
    auto out = result.begin();
    while (in != input.end())
    {
        *out = static_cast<std::byte>((static_cast<unsigned char>(*in) << 1));
        ++in;
        for (int i = 1; i != 7; ++i, ++in)
        {
            if (in == input.end())
                return result;

            const auto byte = static_cast<unsigned char>(*in) & '\x7F';
            merge(*out++, byte >> (7 - i));
            merge(*out, byte << (i + 1));
        }
        if (in == input.end())
            return result;

        merge(*out++, static_cast<unsigned char>(*in++) & '\x7F');
    }
    return result;
}

} // namespace paddock
