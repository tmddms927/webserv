#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <map>

#define REQ_HEADER_FIELD_BUF_SIZE   1024
void        reqHeaderFieldParse(std::string const &req_header_field, std::map<std::string, std::string> & buf);
    /*  request_test_header format
        line\r\n
        line\r\n
        ...
        line\r\n
        \r\n
    */

int main(int argc, char **argv) {
    std::string request_test_header;
    int fd;

    char req_header_field_buf[REQ_HEADER_FIELD_BUF_SIZE];
    memset(req_header_field_buf, 0, REQ_HEADER_FIELD_BUF_SIZE);

    if (argc != 2)
        std::cout << "use : ./test filename" << std::endl;

    fd = open(argv[1], O_RDONLY);

    int read_size;
    read_size = read(fd, req_header_field_buf, REQ_HEADER_FIELD_BUF_SIZE - 1);


    for (int i = 0; req_header_field_buf[i];)
    {
        if (req_header_field_buf[i] == '*') {
            req_header_field_buf[i++] = '\r';
            req_header_field_buf[i] = '\n';
        }
        i++;
    }

    std::map<std::string, std::string> buf;
    reqHeaderFieldParse(req_header_field_buf, buf);

    return 0;
}