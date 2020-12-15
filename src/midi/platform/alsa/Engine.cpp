#include "Engine.hpp"

#include "midi/errors.hpp"

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
ClientType getClientType(const snd_seq_client_info_t* info)
{
    auto type = snd_seq_client_info_get_type(info);
    switch (type)
    {
    case SND_SEQ_USER_CLIENT:
        return ClientType::user;
    case SND_SEQ_KERNEL_CLIENT:
        return ClientType::system;
    default:
        // This shouldn't happen unless something wrong was done.
        throw std::logic_error("Unknown client type");
    }
}

std::optional<PortDirection> getPortDirection(const snd_seq_port_info_t* info)
{
    int capability = snd_seq_port_info_get_capability(info);
    if (capability & SND_SEQ_PORT_CAP_DUPLEX)
        return PortDirection::duplex;
    if (capability & SND_SEQ_PORT_CAP_READ)
        return PortDirection::read;
    if (capability & SND_SEQ_PORT_CAP_WRITE)
        return PortDirection::write;
    return {};
}

PortType getPortType(const snd_seq_port_info_t* info)
{
    int type = snd_seq_port_info_get_type(info);
    if (type | SND_SEQ_PORT_TYPE_HARDWARE)
        return PortType::hardware;
    return PortType::software;
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

ClientId makeClientId(snd_seq_client_info_t* info)
{
    snd_seq_client_info_t* copy;
    if (snd_seq_client_info_malloc(&copy) == -1)
        throw std::bad_alloc();
    snd_seq_client_info_copy(copy, info);
    return std::shared_ptr<void>(copy, snd_seq_client_info_free);
}

std::shared_ptr<snd_seq_client_info_t> getClientInfo(const ClientId& clientId)
{
    return std::static_pointer_cast<snd_seq_client_info_t>(clientId);
}

struct HwPortInfo
{
    std::string deviceId;
    PortDirection direction;
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
                return PortDirection::duplex;
            else if (index < inSubdevices)
                return PortDirection::read;
            else
                return PortDirection::write;
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

ClientInfo makeClientInfo(snd_seq_t* handle, ClientId clientId,
                          const PortNameToDeviceMap& portToDevice)
{
    ClientInfo result;
    auto info = getClientInfo(clientId);

    result.name = snd_seq_client_info_get_name(info.get());
    result.type = getClientType(info.get());
    result.id = std::move(clientId);

    auto alsaClientId = snd_seq_client_info_get_client(info.get());
    auto numPorts = snd_seq_client_info_get_num_ports(info.get());

    for (int i = 0; i < numPorts; ++i)
    {
        auto portInfo = getPortInfo(handle, alsaClientId, i);

        if (!portInfo)
            continue;

        portInfo->clientId = result.id;
        if (auto iter = portToDevice.find(portInfo->name);
            iter != portToDevice.end() &&
            iter->second.direction == portInfo->direction)
        {
            portInfo->hwDeviceId = iter->second.deviceId;
        }

        switch (portInfo->direction)
        {
        case PortDirection::read:
            result.outputs.push_back(*portInfo);
            break;
        case PortDirection::write:
            result.inputs.push_back(*portInfo);
            break;
        case PortDirection::duplex:
            result.outputs.push_back(*portInfo);
            result.inputs.push_back(*portInfo);
            break;
        default:;
        }
    }
    return result;
}

std::shared_ptr<void> getPollDescriptor(snd_seq_t* client)
{
    // This function return always 1 if events has either POLLIN or POLLOUT.
    // Let's assume that's the case, but throw if it's not.
    if (snd_seq_poll_descriptors_count(client, POLLIN) != 1)
        throw std::logic_error("Poll descriptors is not 1");

    auto fd = std::make_shared<pollfd>();
    // No need to check the return value, it must be one.
    snd_seq_poll_descriptors(client, fd.get(), 1, POLLIN);
    return std::static_pointer_cast<void>(fd);
}
} // namespace

Expected<Engine> Engine::create()
{
    snd_seq_t* handle;
    if (snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0) == -1)
        return tl::make_unexpected(EngineError::initializationFailed);

    return Engine(Handle(handle, snd_seq_close));
}

Engine::Engine(Handle handle)
    : _handle{std::move(handle)}
    , _pollHandle{getPollDescriptor(_handle.get())}
{
    snd_seq_client_info_t* info;
    snd_seq_client_info_alloca(&info);

    snd_seq_get_client_info(_handle.get(), info);
    auto thisId = snd_seq_client_info_get_client(info);

    snd_seq_client_info_set_client(info, -1);

    while (snd_seq_query_next_client(_handle.get(), info) >= 0)
    {
        auto id = snd_seq_client_info_get_client(info);
        if (id == thisId)
            continue;

        _clients.push_back(std::make_tuple(id, makeClientId(info)));
    }
}

Engine::Engine(Engine&& other) noexcept = default;
Engine& Engine::operator=(Engine&& other) noexcept = default;

Engine::~Engine() = default;

Expected<Sequencer> Engine::openClient(const char* name,
                                       PortDirection direction)
{
    return Sequencer::open(name, direction);
}

std::vector<ClientInfo> Engine::queryClientInfos() const
{
    const auto portToDevice = getHwMidiDevices();

    std::vector<ClientInfo> result;
    for (const auto& client : _clients)
    {
        result.push_back(makeClientInfo(
            _handle.get(), std::get<ClientId>(client), portToDevice));
    }
    return result;
}

std::optional<ClientInfo> Engine::queryClientInfo(const ClientId& id) const
{
    if (!id)
        return {};

    snd_seq_t* handle;
    if (snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0) == -1)
        return {};

    auto clientInfo = getClientInfo(id);
    // This function may return old data if the client has been disconnected
    // Since the ClientId pointer was created.
    return makeClientInfo(handle, std::move(clientInfo), getHwMidiDevices());
}

} // namespace alsa
} // namespace midi
} // namespace paddock
