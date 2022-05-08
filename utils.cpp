#include "utils.hpp" 

std::string const ft_itoa(int i) {
    std::stringstream s;
	s << i;
	return s.str();
}

void    ft_split(std::vector<std::string> &dest, std::string const &src, std::string const &mark) {
    size_t start_pos = 0;

    for (int debug_count = 0;;debug_count++) {
        size_t found;

        if (start_pos >= src.size()) {
            break;
        }
        found = src.find(mark, start_pos);
        if (found == std::string::npos) {
            std::string s = src.substr(start_pos);
            dest.push_back(s);
            break;
        }
        dest.push_back(src.substr(start_pos, found - start_pos));
        start_pos = found + mark.size();
    }
}