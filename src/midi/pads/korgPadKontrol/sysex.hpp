#pragma once

#include "midi/sysex.hpp"

#include "utils/byte.hpp"

#include <array>

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{
namespace sysex
{
constexpr size_t maxMessageSize = 128;

using midi::sysex::START;
using midi::sysex::END;
constexpr auto NON_REALTIME_MESSAGE = 0x7E_b;
constexpr auto GENERAL_INFORMATION = 0x06_b;

constexpr auto KORG = 0x42_b;
constexpr auto SW_PROJECT = 0x6E_b;
constexpr auto PADKONTROL = 0x08_b;

#define SYSEX_HEADER START, KORG, 0x40_b, SW_PROJECT, PADKONTROL

// clang-format off

// Values for 6th bytes
// 6th Byte = cd : 0dvmmmmm  d (0: Host->Controller, 1: Controller->Host)
//                           v (0: 2 Bytes Data Format, 1: Variable)
//                           mmmmm (Command Number)
// * = only in native mode

// Host commands
constexpr auto NATIVE_MODE_REQ = 0x00_b; // 000 00000
constexpr auto DISPLAY_LED     = 0x01_b; // 000 00001 *
constexpr auto DISPLAY_LCD     = 0x22_b; // 001 00010 *
constexpr auto PORT_DETECT_REQ = 0x1E_b; // 000 11110
constexpr auto DATA_DUMP_REQ   = 0x1F_b; // 000 11111
constexpr auto PACKET_COMM_REQ = 0x3F_b; // 001 11111

constexpr auto IDENTITY_REQ    = 0x01_b;

// Controller replies
constexpr auto NATIVE_MODE     = 0x40_b; // 010 00000
constexpr auto ENCODER_OUTPUT  = 0x43_b; // 010 00011 *
constexpr auto PAD_OUTPUT      = 0x45_b; // 010 00101 *
constexpr auto PEDAL_OUTPUT    = 0x47_b; // 010 00111 *
constexpr auto SW_OUTPUT       = 0x48_b; // 010 01000 *
constexpr auto KNOB_OUTPUT     = 0x49_b; // 010 01001 *
constexpr auto XY_OUTPUT       = 0x4B_b; // 010 01011 *
constexpr auto PORT_DETECT     = 0x7E_b; // 011 11110
constexpr auto PACKET_COMM     = 0x5F_b; // 010 11111
constexpr auto DATA_DUMP       = 0x7F_b; // 011 11111

constexpr auto IDENTITY        = 0x02_b;

// Function codes for requests and replies
constexpr auto SCENE_CHANGE_REQ       = 0x14_b;
constexpr auto SCENE_CHANGE           = 0x4F_b;
constexpr auto CURRENT_SCENE_DUMP_REQ = 0x10_b;
constexpr auto CURRENT_SCENE_DUMP     = 0x40_b;
constexpr auto GLOBAL_DATA_DUMP_REQ   = 0x0E_b;
constexpr auto GLOBAL_DATA_DUMP       = 0x51_b;
constexpr auto SCENE_WRITE_REQ        = 0x11_b;
constexpr auto DATA_FORMAT_ERROR      = 0x26_b;
constexpr auto DATA_LOAD_COMPLETED    = 0x23_b;
constexpr auto DATA_LOAD_ERROR        = 0x24_b;
constexpr auto WRITE_COMPLETED        = 0x21_b;
constexpr auto WRITE_ERROR            = 0x22_b;
// Bytes 00-03 are reserved for Native KORG mode Dump Data

#define PACKET_COMM_HOST_DATA_TYPE_1 PACKET_COMM_REQ, 0x2A_b, 0x00_b
#define PACKET_COMM_HOST_DATA_TYPE_2 PACKET_COMM_REQ, 0x0A_b, 0x01_b
#define PACKET_COMM_CONTROLLER_DATA_TYPE_1 PACKET_COMM, 0x00_b
#define PACKET_COMM_CONTROLLER_DATA_TYPE_2 PACKET_COMM, 0x01_b


constexpr auto nativeModeOffReq = std::to_array({
    SYSEX_HEADER, NATIVE_MODE_REQ, 0x00_b, 0x00_b, END});

constexpr auto nativeModeOffReply = std::to_array({
    SYSEX_HEADER, 0x40_b, 0x00_b, 0x02_b, END});

constexpr auto nativeModeOnReq = std::to_array({
    SYSEX_HEADER, NATIVE_MODE_REQ, 0x00_b, 0x01_b, END});

constexpr auto nativeModeOnReply = std::to_array({
    SYSEX_HEADER, 0x40_b, 0x00_b, 0x03_b, END});

constexpr auto nativeModeInit = std::to_array({
    SYSEX_HEADER, PACKET_COMM_HOST_DATA_TYPE_2,
    0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x29_b, 0x29_b, 0x29_b,
    END});

constexpr auto nativeModeTest = std::to_array({
    SYSEX_HEADER,
    DISPLAY_LCD, 0x04_b, 0x00_b, 0x59_b, 0x45_b, 0x53_b, END});

constexpr auto nativeEnableOutput = std::to_array({
    SYSEX_HEADER, PACKET_COMM_HOST_DATA_TYPE_1,
    //0       1       2       3       4       5       6       7
    0x00_b, 0x05_b, 0x05_b, 0x05_b, 0x7F_b, 0x7E_b, 0x7F_b, 0x7F_b,
    //8       9       10      11      12      13      14      15
    0x03_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b,
    //16      17      18      19      20      21      22      23
    0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b,
    //24      25      26      27      28      29      30      31
    0x0A_b, 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b, 0x07_b,
    //32      33      34      35      36      37      38      39
    0x08_b, 0x09_b, 0x0A_b, 0x0B_b, 0x0C_b, 0x0d_b, 0x0E_b, 0x0F_b,
    //40
    0x10_b, END});

constexpr auto inquiryMessageRequest = std::to_array({
    START, NON_REALTIME_MESSAGE, 0x7F_b, 0x06_b, IDENTITY_REQ, END});

constexpr auto currentSceneDataDumpReq = std::to_array({
    SYSEX_HEADER, DATA_DUMP_REQ, CURRENT_SCENE_DUMP_REQ, 0x00_b, END});

constexpr auto globalDataDumpReq = std::to_array({
    SYSEX_HEADER, DATA_DUMP_REQ, GLOBAL_DATA_DUMP_REQ, 0x00_b, END});

constexpr auto dataFormatError =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, DATA_FORMAT_ERROR, 0x00_b, END});

constexpr auto dataLoadCompleted =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, DATA_LOAD_COMPLETED, 0x00_b, END});

constexpr auto dataLoadError =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, DATA_LOAD_ERROR, 0x00_b, END});

constexpr auto writeCompleted =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, WRITE_COMPLETED, 0x00_b, END});

constexpr auto writeError =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, WRITE_ERROR, 0x00_b, END});

constexpr auto sceneWriteReq(std::byte sceneNumber)
{
    return std::to_array({SYSEX_HEADER, DATA_DUMP_REQ, SCENE_WRITE_REQ,
                          sceneNumber, END});
}

constexpr auto sceneChangeReq(std::byte sceneNumber)
{
    return std::to_array({SYSEX_HEADER, DATA_DUMP_REQ, SCENE_CHANGE_REQ,
                          sceneNumber, END});
}

constexpr auto resetDefaultScene =  std::to_array({
    SYSEX_HEADER, 0x7F_b, 0x7F_b, 0x02_b, 0x0A_b, 0x02_b, CURRENT_SCENE_DUMP,
    0x00_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x00_b,
    0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x00_b, 0x19_b,
    0x19_b, 0x3D_b, 0x45_b, 0x41_b, 0x3F_b, 0x3C_b, 0x00_b, 0x3B_b, 0x39_b,
    0x37_b, 0x31_b, 0x33_b, 0x44_b, 0x38_b, 0x00_b, 0x30_b, 0x34_b, 0x36_b,
    0x3A_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    0x00_b, 0x00_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x00_b,
    0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x00_b, 0x7F_b,
    0x7F_b, 0x7F_b, 0x7F_b, 0x19_b, 0x2F_b, 0x7F_b, 0x40_b, 0x00_b, 0x7F_b,
    0x00_b, 0x20_b, 0x31_b, 0x14_b, 0x7F_b, 0x19_b, 0x7F_b, 0x31_b, 0x15_b,
    0x7F_b, 0x7F_b, 0x11_b, 0x06_b, 0x67_b, 0x7F_b, 0x7F_b, 0x00_b, 0x31_b,
    0x01_b, 0x7F_b, 0x7F_b, 0x14_b, 0x00_b, 0x32_b, 0x0C_b, 0x00_b, 0x02_b,
    0x3C_b, 0x7F_b, 0x40_b, 0x14_b, 0x7F_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    0x00_b, 0x00_b, 0x00_b, END});

// clang-format on
}
}
}
}
