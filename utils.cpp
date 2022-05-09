#include "utils.hpp"

std::string const ft_itoa(int i) {
    std::stringstream s;
	s << i;
	return s.str();
}

/*
** 현재 시간을 반환
** 단위 : millisecond
*/
unsigned long const get_time() {
	struct timespec	time;

	clock_gettime(CLOCK_REALTIME, &time);
	return (time.tv_sec * 1000) + (time.tv_nsec / 1000);
}
