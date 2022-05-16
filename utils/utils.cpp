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

void    ft_remove_space(std::string & str) {
		str.erase(remove(str.begin(), str.end(), ' '), str.end());
}

void    ft_trim_space(std::string & str) {
	ft_ltrim_space(str);
	ft_rtrim_space(str);
}

void    ft_ltrim_space(std::string & str) {
	for (std::string::iterator it = str.begin(); it != str.end(); it++) {
		if (!std::isspace(*it)) {
			str.erase(str.begin(), it);
			return ;
		}
	}
	str = "";
}

void    ft_rtrim_space(std::string & str) {
	for (std::string::reverse_iterator rit = str.rbegin(); rit != str.rend(); rit++) {
		if (!std::isspace(*rit)) {
			str.erase(rit.base(), str.end());
			return ;
		}
	}
	str = "";
}

std::pair<std::string, std::string> ft_slice_str(std::string const & src, size_t pos) {
	std::pair<std::string, std::string> p;
	if (src.size() <= pos)
		throw std::out_of_range("out of range");
	p.first = src.substr(0, pos);
	if (src.size() <= pos + 1)
		p.second = "";
	else
		p.second = src.substr(pos + 1);
	return p;
}

/*
** 현재 시간을 반환
** 단위 : second
*/
unsigned long long get_time() {
	struct timespec	time;

	clock_gettime(CLOCK_REALTIME, &time);
	return ((time.tv_sec) + (time.tv_nsec / 1000 / 1000));
}
