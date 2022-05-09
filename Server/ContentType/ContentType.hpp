#ifndef CONTENT_TYPE_HPP
# define CONTENT_TYPE_HPP

#include <iostream>

class ContentType {
private:
    const std::string file;
    std::string content_type;
public:
    ContentType(std::string const & f);
    std::string const & getContentType();
    void set_content_type();
};

#endif
