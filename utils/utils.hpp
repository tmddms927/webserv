#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <time.h>

std::string const ft_itoa(int i);

void    ft_split(std::vector<std::string> &dest, std::string const &src, std::string const &mark);
void    ft_remove_space(std::string & str);
void    ft_trim_space(std::string & str);
void    ft_ltrim_space(std::string & str);
void    ft_rtrim_space(std::string & str);
std::pair<std::string, std::string> ft_slice_str(std::string const & src, size_t pos);

unsigned long long get_time();


#endif
