#include <string>
#include <vector>
#include <iostream>
#include <map>

void    ft_split(std::vector<std::string> &dest, std::string const &src, std::string const &mark) {
    size_t start_pos = 0;

    for (int debug_count = 0;;debug_count++) {
        size_t found;

        if (start_pos >= src.size()) {
            std::cout << "pass max" << std::endl;
            break;
        }
        found = src.find(mark, start_pos);
        if (found == std::string::npos) {
            std::cout << "none pos" << std::endl;
            std::string s = src.substr(start_pos);
            dest.push_back(s);
            break;
        }
        dest.push_back(src.substr(start_pos, found - start_pos));
        start_pos = found + mark.size();
    }
}

// how to check invalid situation -> check syntax error and value validation each line
// if line is invalid, parser ignore it
// invalid syntax : none ':', none key or none value
// case 1-1(exist ':') -> [key:value] normal
// case 1-2(exist ':') -> [key:] ?
// case 1-3(exist ':') -> [:value] 400 bad request
// case 2(not exist ':') -> [key] ?
void        reqHeaderFieldParse(std::string const &req_header_field, std::map<std::string, std::string> & buf) {
    std::vector<std::string> req_header_field_splited;
    (void)buf;

    ft_split(req_header_field_splited, req_header_field, "\r\n");
    for (size_t i = 0; i < req_header_field_splited.size(); i++) {
        std::vector<std::string> node;
        ft_split(node , req_header_field_splited[i], ":");
        
        // line syntax error
        // line invalid argument error
        // parse key, value
    }
}