#include "utils.hpp"

namespace paddock
{
namespace midi
{
namespace alsa
{

ClientId makeClientId(snd_seq_client_info_t* info)
{
    snd_seq_client_info_t* copy;
    if (snd_seq_client_info_malloc(&copy) == -1)
        throw std::bad_alloc();
    snd_seq_client_info_copy(copy, info);
    return std::shared_ptr<void>(copy, snd_seq_client_info_free);
}

}
}
}
