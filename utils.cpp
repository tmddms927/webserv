#include "utils.hpp"

std::string const ft_itoa(int i) {
    std::stringstream s;
	s << i;
	return s.str();
}
