#include "NsmSession.hpp"

#include "Globals.hpp"
#include "Log.hpp"
#include "errors.hpp"

#include <lo/lo_cpp.h>

#include <unistd.h>

#include <iostream>

namespace paddock
{
namespace core
{
namespace
{
constexpr int NSM_API_VERSION_MAJOR{1};
constexpr int NSM_API_VERSION_MINOR{0};

enum NsmProtocolErrors
{
    ERR_OK = 0,
    ERR_GENERAL = -1,
    ERR_INCOMPATIBLE_API = -2,
    ERR_BLACKLISTED = -3,
    ERR_LAUNCH_FAILED = -4,
    ERR_NO_SUCH_FILE = -5,
    ERR_NO_SESSION_OPEN = -6,
    ERR_UNSAVED_CHANGES = -7,
    ERR_NOT_NOW = -8
};

class NsmSessionErrorCategory : public std::error_category
{
    const char* name() const noexcept override
    {
        return "paddock-nsm-session-error";
    }
    std::string message(int code) const override
    {
        switch (static_cast<NsmSession::Error>(code))
        {
        case NsmSession::Error::couldNotStartLoServer:
            return "OSC server could not be started";
        case NsmSession::Error::failedToAnnounceToServer:
            return "Failed to announce to NSM server";
        case NsmSession::Error::invalidServerAddress:
            return "Invalid NSM server address";
        default:
            throw std::logic_error("Unknown error code");
        }
    }
    bool equivalent(int code, const std::error_condition& condition) const
        noexcept override
    {
        return (condition == ErrorType::nsmSession);
    }
};

const NsmSessionErrorCategory nsmSessionErrorCategory{};

constexpr auto NSM_CLIENT_OPEN = "/nsm/client/open";
constexpr auto NSM_CLIENT_SAVE = "/nsm/client/save";
constexpr auto NSM_CLIENT_SESSION_IS_LOADED = "/nsm/client/session_is_loaded";
constexpr auto NSM_CLIENT_IS_DIRTY = "/nsm/client/is_dirty";
constexpr auto NSM_CLIENT_IS_CLEAN = "/nsm/client/is_clean";
constexpr auto NSM_CLIENT_MESSAGE = "/nsm/client/message";

constexpr auto NSM_SERVER_ANNOUNCE = "/nsm/server/announce";
constexpr auto NSM_SERVER_BROADCAST = "/nsm/server/broadcast";
} // namespace

std::error_code make_error_code(NsmSession::Error error)
{
    return std::error_code{static_cast<int>(error), nsmSessionErrorCategory};
}

int getProtoFromAddress(const char* url)
{
    lo_address addr = lo_address_new_from_url(url);
    int proto = lo_address_get_protocol(addr);
    lo_address_free(addr);
    return proto;
}

class NsmSession::_Impl
{
public:
    Callbacks callbacks;
    std::string nsmUrl;
    lo::Address nsm;
    lo::ServerThread server;
    bool active{false};
    std::string name;

    static Expected<std::unique_ptr<_Impl>> connect(const char* nsmUrl,
                                                    Callbacks callbacks)
    {
        auto impl =
            std::unique_ptr<_Impl>(new _Impl(nsmUrl, std::move(callbacks)));
        if (!impl->nsm.is_valid())
        {
            return tl::make_unexpected(Error::invalidServerAddress);
        }
        if (!impl->server.is_valid())
        {
            return tl::make_unexpected(Error::couldNotStartLoServer);
        }

        impl->server.start();

        impl->init();

        if (auto expected = impl->announce(); !expected)
        {
            return tl::unexpected(expected.error());
        }
        return impl;
    }

    void sendDirty(bool dirty)
    {
        if (!active)
            return;

        if (dirty)
            nsm.send(NSM_CLIENT_IS_DIRTY);
        else
            nsm.send(NSM_CLIENT_IS_CLEAN);
    }

private:
    struct Reply
    {
        Reply(const char* path, _Impl* impl)
            : _path(path)
            , _impl(impl)
        {
        }

        void sendOk()
        {
            lo::Message message;
            message.add("ss", _path, "OK");
            _impl->nsm.send("/reply", message);
        }

        void sendError(int code, const char* errorMessage = "")
        {
            lo::Message message;
            message.add("sis", _path, code, errorMessage);
            _impl->nsm.send("/error", message);
        }

    private:
        const char* _path;
        _Impl* _impl;
    };

    _Impl(const char* url, Callbacks callbacks)
        : callbacks(std::move(callbacks))
        , nsmUrl(url)
        , nsm(url)
        , server(nullptr, getProtoFromAddress(url), serverErrorCallback)
    {
    }

    Reply prepareReply(const char* path) { return Reply(path, this); }

    void init()
    {
        server.add_method("/error", "sis", [this](lo_arg** argv, int argc) {
            this->notifyAnnounceError(&argv[0]->s, argv[1]->i, &argv[2]->s);
        });

        server.add_method("/reply", "ssss", [this](lo_arg** argv, int argc) {
            return this->notifyAnnounceReply(&argv[0]->s, &argv[1]->s,
                                             &argv[2]->s, &argv[3]->s);
        });

        server.add_method(
            NSM_CLIENT_OPEN, "sss", [this](lo_arg** argv, int argc) {
                this->notifyOpen(&argv[0]->s, &argv[1]->s, &argv[2]->s);
            });

        server.add_method(NSM_CLIENT_SAVE, "",
                          [this](lo_arg**, int) { this->notifySave(); });

        server.add_method(NSM_CLIENT_SESSION_IS_LOADED, "",
                          [this](lo_arg** argv, int argc) {
                              this->notifySessionLoaded();
                          });

        server.add_method(nullptr, nullptr, [](lo_arg** argv, int argc) {
            log() << "broadcast";
        });
    }

    tl::expected<_Impl*, std::error_code> announce()
    {
        lo::Message message;
        message.add("sssiii",
                    "Paddock",                   // Application name
                    ":dirty:message:",           // capabilities
                    Globals::instance().argv[0], // process name
                    NSM_API_VERSION_MAJOR, NSM_API_VERSION_MINOR,
                    (int)getpid());

        nsm.send(NSM_SERVER_ANNOUNCE, message);
        checkWait(2000);
        return this;
    }

    int notifyAnnounceError(const std::string& path, int code,
                            const std::string& errorMessage)
    {
        if (path != NSM_SERVER_ANNOUNCE)
            return -1;
        // TODO
        log() << code << errorMessage;
        return 0;
    }

    int notifyAnnounceReply(const std::string& path, const std::string&,
                            const std::string& managerName, const std::string&)
    {
        if (path != NSM_SERVER_ANNOUNCE)
            return -1;

        sendMessage(1, "Successfully registered on " + managerName);
        active = true;
        return 0;
    }

    void notifyOpen(const std::string& path, const std::string& sessionName,
                    const std::string& clientId)
    {
        if (!callbacks.openCallback)
            return;
        auto reply = prepareReply(NSM_CLIENT_OPEN);

        if (auto error = callbacks.openCallback(path, sessionName, clientId))
        {
            reply.sendError(ERR_GENERAL, error.message().c_str());
        }
        else
        {
            log() << "send message";
            sendMessage(1, "Opened program from " + path);
            reply.sendOk();
        }
    }

    void notifySessionLoaded() { log() << "session loaded"; }

    void notifySave()
    {
        if (!callbacks.saveCallback)
            return;
        auto reply = prepareReply(NSM_CLIENT_SAVE);

        if (auto error = callbacks.saveCallback())
        {
            reply.sendError(ERR_GENERAL, error.message().c_str());
        }
        else
            reply.sendOk();
    }

    void sendMessage(int priority, const std::string& text)
    {
        lo::Message message;
        message.add("is", priority, text.c_str());
        nsm.send(NSM_CLIENT_MESSAGE, message);
    }

    static void serverErrorCallback(int num, const char* msg, const char* where)
    {
#ifndef NDEBUG
        log() << "OSC server error " << num << ": " << msg
              << (where ? " (" + std::string(where) + ")" : "");
#endif
    }

    void checkWait(int timeout)
    {
        // Copied from the sample client from NSM.
        if (server.wait(timeout))
        {
            while (server.recv(0))
                ;
        }
    }
};

Expected<NsmSession> NsmSession::startNsmSession(const char* nsmUrl,
                                                 Callbacks callbacks)
{
    log() << "Trying to connect to NSM server:" << nsmUrl;

    auto impl = _Impl::connect(nsmUrl, std::move(callbacks));

    if (impl)
        return NsmSession(std::move(*impl));
    else
        return tl::make_unexpected(impl.error());
}

NsmSession::NsmSession(std::unique_ptr<_Impl> impl)
    : _impl(std::move(impl))
{
}

NsmSession::~NsmSession() = default;

NsmSession::NsmSession(NsmSession&& other) noexcept = default;
NsmSession& NsmSession::operator=(NsmSession&& other) noexcept = default;

void NsmSession::setDirty(bool dirty)
{
    _impl->sendDirty(dirty);
}

} // namespace core
} // namespace paddock
