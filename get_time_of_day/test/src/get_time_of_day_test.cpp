#include "../../include/gettimeofday.h"
#include <iostream>

int main(int argc, char** argv)
{
    struct timeval time_val;
    struct timezone time_zone;

	// The `gettimeofday` function can get the time as well as timezone.
	gettimeofday(&time_val, &time_zone);

	// The use of the `timezone` structure is obsolete. The tz argument 
	// should normally be specified as NULL.
	gettimeofday(&time_val, NULL);
	
	std::cout << "seconds [time_val.tv_sec]: " << time_val.tv_sec << std::endl;
	std::cout << "microseconds [time_val.tv_usec]: " << time_val.tv_usec << std::endl;
	std::cout << "minutes west of Greenwich [time_zone.tz_minuteswest]: " << time_zone.tz_minuteswest << std::endl;
	std::cout << "type of dst correction [time_zone.tz_dsttime]:" << time_zone.tz_dsttime << std::endl;

	return 0;
}
