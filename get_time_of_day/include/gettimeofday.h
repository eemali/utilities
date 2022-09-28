#ifndef GET_TIME_OF_DAY
#define GET_TIME_OF_DAY
#ifdef _WIN32
#include <time.h>
#include <windows.h>
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
#ifdef _MSC_VER
struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};
#endif // _MSC_VER
int gettimeofday(struct timeval *tv, struct timezone *tz);
#else
#include <sys/time.h>
#endif // _WIN32
#endif // !GET_TIME_OF_DAY
