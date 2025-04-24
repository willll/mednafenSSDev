#pragma once

#include "tcp-server.h"
#include "watchpoint.hpp"

enum class TinyGdbSignal : u8
{
  HANGUP = 1,
  INT = 2,
  QUIT = 3,
  ILL = 4,
  TRAP = 5,
  ABORT = 6,
  EMT = 7,
  FPE = 8,
  KILL = 9,
  BUS = 10,
  SEGV = 11,
  SYS = 12,
  PIPE = 13,
  ALRM = 14,
  TERM = 15,
  URG = 16,
  STOP = 17,
  TSTP = 18,
  CONT = 19,
  CHLD = 20,
  TTIN = 21,
  TTOU = 22,
  IO = 23,
  PROF = 27,
  WINCH = 28,
  LOST = 29,
};

/**
 * This implements a GDB server to handle remote debugging via a GDB client.
 * It is both independent of ares itself and any specific system.
 * Functionality is added by providing system-specific callbacks, as well as using the API inside a system.
 * (See the Readme.md file for more information.)
 *
 * NOTE:
 * Command handling and the overall logic was carefully designed to support as many IDEs and GDB versions as possible.
 * Things can break very easily (and the official documentation may lie), so be very sure of any changes made here.
 * If changes are necessary, please verify that the following gdb-versions / IDEs still work properly:
 *
 * GDB:
 * - gdb-multiarch        (the plain vanilla version exists in most package managers, supports a lot of arches)
 * - mips64-ultra-elf-gdb (special MIPS build of gdb-multiarch, i do NOT recommend it, behaves strangely)
 * - mingw-w64-x86_64-gdb (vanilla build for Windows/MSYS)
 *
 * IDEs/Tools:
 * - GDB's CLI
 * - VSCode
 * - CLion (with bundled gdb-multiarch)
 *
 * For testing, please also check both linux and windows (WSL2).
 * With WSL2, windows-ares is started from within WSL, while the debugger runs in linux.
 * This can be easily tested with VSCode and it's debugger.
 */
class TinyGDBServer : public TinyTcpServer
{
public:
  auto reset() -> void;

  struct
  {
    // Memory
    std::function<std::string(u64 address, u32 byteCount)> read{};
    std::function<void(u64 address, std::vector<u8> value)> write{};
    std::function<u64(u64 address)> normalizeAddress{};

    // Registers
    std::function<std::string()> regReadGeneral{};
    std::function<void(const std::string &regData)> regWriteGeneral{};
    std::function<std::string(u32 regIdx)> regRead{};
    std::function<bool(u32 regIdx, u64 regValue)> regWrite{};

    // Emulator
    std::function<void(u64 address)> emuCacheInvalidate{};
    std::function<std::string()> targetXML{};

  } hooks{};

  // Exception
  auto reportSignal(TinyGdbSignal sig, u64 originPC) -> bool;

  // PC / Memory State Updates
  auto reportPC(u64 pc) -> bool;
  auto reportMemRead(u64 address, u32 size) -> void;
  auto reportMemWrite(u64 address, u32 size) -> void;

  // Breakpoints / Watchpoints
  auto isHalted() const { return forceHalt && haltSignalSent; }
  auto hasBreakpoints() const
  {
    return (!breakpoints.empty()) || singleStepActive || (!watchpointRead.empty()) || (!watchpointWrite.empty());
  }

  auto getPcOverride() const { return pcOverride; };

  auto updateLoop() -> void;
  auto getStatusText(u32 port, bool useIPv4) -> std::string;

protected:
  auto onText(std::string_view text) -> void override;
  auto onConnect() -> void override;
  auto onDisconnect() -> void override;

private:
  bool insideCommand{false};
  std::string cmdBuffer{""};

  bool haltSignalSent{false}; // marks if a signal as been sent for new halts (force-halt and breakpoints)
  bool forceHalt{false};      // forces a halt despite no breakpoints being hit
  bool singleStepActive{false};

  bool noAckMode{false};         // gets set if lldb prefers no acknowledgements
  bool nonStopMode{false};       // (NOTE: Not working for now), gets set if gdb wants to switch over to async-messaging
  bool handshakeDone{false};     // set to true after a few handshake commands, used to prevent exception-reporting until client is ready
  bool requestDisconnect{false}; // set to true if the client decides it wants to disconnect

  bool hasActiveClient{false};
  u32 messageCount{0};    // message count per update loop
  s32 currentThreadC{-1}; // selected thread for the next 'c' command

  u64 currentPC{0};
  std::optional<u64> pcOverride{0}; // temporary override to handle edge-cases for exceptions/watchpoints

  // client-state:
  std::vector<u64> breakpoints{};
  std::vector<Watchpoint> watchpointRead{};
  std::vector<Watchpoint> watchpointWrite{};

  auto processCommand(const std::string &cmd, bool &shouldReply) -> std::string;
  auto resetClientData() -> void;

  auto reportWatchpoint(const Watchpoint &wp, u64 address) -> void;

  auto sendPayload(const std::string &payload) -> void;
  auto sendSignal(TinyGdbSignal code) -> void;
  auto sendSignal(TinyGdbSignal code, const std::string &reason) -> void;

  auto haltProgram() -> void;
  auto resumeProgram() -> void;
};

extern TinyGDBServer server;

