#pragma once

namespace paddock
{
namespace midi
{
class ClientInfo;

class KorgPadKontrol
{
public:
    static bool matches(const ClientInfo& client);

    KorgPadKontrol();
    ~KorgPadKontrol();

    KorgPadKontrol(const KorgPadKontrol& other);
    KorgPadKontrol(KorgPadKontrol&& other);

    KorgPadKontrol& operator=(const KorgPadKontrol& other) = delete;
    KorgPadKontrol& operator=(KorgPadKontrol&& other) = delete;
};

} // namespace midi
} // namespace paddock
