#pragma once

#include "tcp-socket.h"

/**
 * Provides text-based TCP server on top of the Socket.
 * This handles incoming messages and can send data back to the client.
 */

class TinyTcpServer : public TinySocket
{
public:
    bool hadHandshake{false};

protected:
    auto onData(const std::vector<u8> &data) -> void override;

    auto sendText(const std::string &text) -> void;
    virtual auto onText(std::string_view text) -> void = 0;
};
