#pragma once

#include <atomic>
#include <mutex>
#include <vector>
#include <string>
#include <string_view>
#include <functional>
#include <optional>
#include <algorithm>

#include "gdb-types.h"
#include "helpers.h"

class TinySocket
{
public:
    auto open(u32 port, bool useIPv4) -> bool;
    auto close(bool notifyHandler = true) -> void;

    auto disconnectClient() -> void;

    auto isStarted() const -> bool { return serverRunning; }
    auto hasClient() const -> bool { return fdClient >= 0; }

    auto getURL(u32 port, bool useIPv4) const -> std::string;

    ~TinySocket() { close(false); }

protected:
    auto update() -> void;

    auto sendData(const u8 *data, u32 size) -> void;
    virtual auto onData(const std::vector<u8> &data) -> void = 0;

    virtual auto onConnect() -> void = 0;
    virtual auto onDisconnect() -> void = 0;

private:
    std::atomic<bool> stopServer{false};     // set to true to let the server-thread know to stop.
    std::atomic<bool> serverRunning{false};  // signals the current state of the server-thread
    std::atomic<bool> wantKickClient{false}; // set to true to let server know to disconnect the current client (if conn.)

    std::atomic<s32> fdServer{-1};
    std::atomic<s32> fdClient{-1};

    std::vector<u8> receiveBuffer{};
    std::mutex receiveBufferMutex{};

    std::vector<u8> sendBuffer{};
    std::mutex sendBufferMutex{};
};