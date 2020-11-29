#include "Engine.hpp"

#include <alsa/asoundlib.h>

#include <map>

namespace paddock
{
namespace midi
{
namespace alsa
{
namespace
{
ClientInfo::Type getClientType(const snd_seq_client_info_t* info)
{
    auto type = snd_seq_client_info_get_type(info);
    switch (type)
    {
    case SND_SEQ_USER_CLIENT:
        return ClientInfo::Type::user;
    case SND_SEQ_KERNEL_CLIENT:
        return ClientInfo::Type::system;
    default:
        // This shouldn't happen unless something wrong was done.
        throw std::logic_error("Unknown client type");
    }
}

std::optional<PortInfo::Direction> getPortDirection(
    const snd_seq_port_info_t* info)
{
    int capability = snd_seq_port_info_get_capability(info);
    if (capability & SND_SEQ_PORT_CAP_DUPLEX)
        return PortInfo::Direction::duplex;
    if (capability & SND_SEQ_PORT_CAP_READ)
        return PortInfo::Direction::read;
    if (capability & SND_SEQ_PORT_CAP_WRITE)
        return PortInfo::Direction::write;
    return {};
}

PortInfo::Type getPortType(const snd_seq_port_info_t* info)
{
    int type = snd_seq_port_info_get_type(info);
    if (type | SND_SEQ_PORT_TYPE_HARDWARE)
        return PortInfo::Type::hardware;
    return PortInfo::Type::software;
}

std::optional<PortInfo> getPortInfo(snd_seq_t* handle, int client, int port)
{
    snd_seq_port_info_t* info;
    snd_seq_port_info_alloca(&info);

    if (snd_seq_get_any_port_info(handle, client, port, info) == -1)
        return {};

    PortInfo portInfo;
    portInfo.name = snd_seq_port_info_get_name(info);
    portInfo.number = port;
    auto direction = getPortDirection(info);
    if (!direction)
        return {};
    portInfo.direction = *direction;
    portInfo.type = getPortType(info);
    return portInfo;
}

std::shared_ptr<void> makeClientId(snd_seq_client_info_t* info)
{
    snd_seq_client_info_t* copy;
    if (snd_seq_client_info_malloc(&copy) == -1)
        throw std::bad_alloc();
    snd_seq_client_info_copy(copy, info);
    return std::shared_ptr<void>(copy, snd_seq_client_info_free);
}

struct HwPortInfo
{
    std::string deviceId;
    PortInfo::Direction direction;
};

using PortNameToDeviceMap = std::map<std::string, HwPortInfo>;

static PortNameToDeviceMap getMidiDevices(snd_ctl_t* ctl, int card, int device)
{
    // Based on amidi.c
    snd_rawmidi_info_t* info;

    snd_rawmidi_info_alloca(&info);
    snd_rawmidi_info_set_device(info, device);

    snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
    int inSubdevices = snd_ctl_rawmidi_info(ctl, info) == -1
                           ? 0
                           : snd_rawmidi_info_get_subdevices_count(info);

    snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
    int outSubdevices = snd_ctl_rawmidi_info(ctl, info) == -1
                            ? 0
                            : snd_rawmidi_info_get_subdevices_count(info);

    int subdevices = std::max(inSubdevices, outSubdevices);
    if (!subdevices)
        return {};

    PortNameToDeviceMap portToDevice;

    for (int i = 0; i < subdevices; ++i)
    {
        snd_rawmidi_info_set_stream(info, i < inSubdevices
                                              ? SND_RAWMIDI_STREAM_INPUT
                                              : SND_RAWMIDI_STREAM_OUTPUT);
        snd_rawmidi_info_set_subdevice(info, i);

        if (snd_ctl_rawmidi_info(ctl, info) == -1)
            continue;

        auto name = snd_rawmidi_info_get_name(info);
        auto subdeviceName = snd_rawmidi_info_get_subdevice_name(info);

        auto getDirection = [inSubdevices, outSubdevices](int index) {
            if (index < inSubdevices && index < outSubdevices)
                return PortInfo::Direction::duplex;
            else if (index < inSubdevices)
                return PortInfo::Direction::read;
            else
                return PortInfo::Direction::write;
        };

        const bool hasSubdevice = i != 0 || subdeviceName[0] != '\0';

        char deviceId[128];
        if (hasSubdevice)
            snprintf(deviceId, 128, "hw:%d,%d,%d", card, device, i);
        else
            snprintf(deviceId, 128, "hw:%d,%d", card, device);

        portToDevice.insert(std::make_pair(
            hasSubdevice ? subdeviceName : name,
            HwPortInfo{.deviceId = deviceId, .direction = getDirection(i)}));
    }

    return portToDevice;
}

PortNameToDeviceMap getHwMidiDevices()
{
    PortNameToDeviceMap portToDevice;

    int card = -1;
    // If any error happens we return whatever we have
    while (snd_card_next(&card) == 0 && card >= 0)
    {
        char name[32];
        snprintf(name, 32, "hw:%d", card);

        snd_ctl_t* ctl;
        if (snd_ctl_open(&ctl, name, 0) < 0)
            continue;

        int device = -1;
        while (snd_ctl_rawmidi_next_device(ctl, &device) == 0 && device >= 0)
        {
            portToDevice.merge(getMidiDevices(ctl, card, device));
        }

        snd_ctl_close(ctl);
    }

    return portToDevice;
}
} // namespace

Expected<Sequencer> Engine::openClient(const char* name)
{
    return Sequencer::open(name);
}

std::vector<ClientInfo> Engine::clientInfos()
{
    const auto portToDevice = getHwMidiDevices();

    snd_seq_t* handle;
    if (snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0) == -1)
        return {};

    snd_seq_client_info_t* info;
    snd_seq_client_info_alloca(&info);

    snd_seq_get_client_info(handle, info);
    auto thisId = snd_seq_client_info_get_client(info);

    snd_seq_client_info_set_client(info, -1);

    std::vector<ClientInfo> clientInfos;

    while (snd_seq_query_next_client(handle, info) >= 0)
    {
        if (snd_seq_client_info_get_client(info) == thisId)
            continue;

        ClientInfo clientInfo;
        clientInfo.name = snd_seq_client_info_get_name(info);
        clientInfo.type = getClientType(info);
        clientInfo.id = makeClientId(info);

        auto clientId = snd_seq_client_info_get_client(info);
        auto numPorts = snd_seq_client_info_get_num_ports(info);

        for (int i = 0; i < numPorts; ++i)
        {
            auto portInfo = getPortInfo(handle, clientId, i);

            if (!portInfo)
                continue;

            portInfo->clientId = clientInfo.id;
            if (auto iter = portToDevice.find(portInfo->name);
                iter != portToDevice.end() &&
                iter->second.direction == portInfo->direction)
            {
                portInfo->hwDeviceId = iter->second.deviceId;
            }

            switch (portInfo->direction)
            {
            case PortInfo::Direction::read:
                clientInfo.outputs.push_back(*portInfo);
                break;
            case PortInfo::Direction::write:
                clientInfo.inputs.push_back(*portInfo);
                break;
            case PortInfo::Direction::duplex:
                clientInfo.outputs.push_back(*portInfo);
                clientInfo.inputs.push_back(*portInfo);
                break;
            default:;
            }
        }

        clientInfos.push_back(std::move(clientInfo));
    }

    snd_seq_close(handle);

    return clientInfos;
}

} // namespace alsa
} // namespace midi
} // namespace paddock
