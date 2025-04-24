#include "tcp-server.h"

auto TinyTcpServer::sendText(const std::string &text) -> void
{
    sendData((const u8 *)text.data(), text.size());
}

auto TinyTcpServer::onData(const std::vector<u8> &data) -> void
{
    std::string_view dataStr((const char *)data.data(), (u32)data.size());

    if (!hadHandshake)
    {
        hadHandshake = true;

        // This is a security check for browsers.
        // Any website can request localhost via JS or HTML, while it can't see the result,
        // GDB will receive the data and commands could be injected (true for all GDB-servers).
        // Since all HTTP requests start with headers, we can simply block anything that doesn't start like a GDB client.
        if (dataStr[0] != '+')
        {
            printf("Non-GDB client detected (message: %s), disconnect client\n", dataStr.data());
            disconnectClient();
            return;
        }

        onConnect();
    }

    onText(dataStr);
}