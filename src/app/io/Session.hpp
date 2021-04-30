#include "app/pads/models.hpp"

#include "io/Serializer.hpp"

#include "utils/Expected.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace paddock::io
{
struct Session
{
    ControllerModel::Model model;
    json program;
};

template <>
struct Serializer<Session>
{
    static Session deserialize(const json& in)
    {
        return Session{in["model"].get<ControllerModel::Model>(),
                       in["program"]};
    }

    static json serialize(const Session& session)
    {
        json j;
        j["model"] = session.model;
        return j;
    }
};

Expected<io::Session> readSession(const std::string& filePath);
std::error_code writeSession(const std::string& filePath, io::Session);

} // namespace paddock::io
