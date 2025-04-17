#if defined(PLATFORM_WINDOWS)

#include <winsock2.h>

inline auto poll(struct pollfd fds[], unsigned long nfds, int timeout) -> int { return WSAPoll(fds, nfds, timeout); }

inline auto usleep(unsigned int us) -> int
{
    if (us != 0)
    {
        Sleep(us / 1000);
    }

    return 0;
}

#endif