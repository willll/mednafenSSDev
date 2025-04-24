#if defined(WIN32)


#include <winsock2.h>

#define MSG_NOSIGNAL 0

inline auto usleep(unsigned int us) -> int
{
    if (us != 0)
    {
        Sleep(us / 1000);
    }

    return 0;
}

#endif