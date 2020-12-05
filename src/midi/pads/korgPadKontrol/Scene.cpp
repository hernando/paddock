#include "Scene.hpp"

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{

Expected<std::vector<std::byte>> encodeScene(const Scene& scene)
{
    (void)scene;
    return {};
}

Expected<Scene> decodeScene(std::span<const std::byte> payload)
{
    for (auto byte : payload)
    {
        printf("%.2X ", int(byte));
    }
    printf("\n");
    return Scene{};
}

}
}
}
