#include "HTTP_Chunk.hpp"
#include <sstream>

void Chunk::setLength(std::string const & len_str) {
    std::stringstream stream;

    stream << len_str;
    stream >> std::hex >> length;
}

long Chunk::appendContent(std::string const & content_part) {
    content += content_part;
    return content_part.size();
}

long const & Chunk::getLength() const {
    return length;
}
std::string const & Chunk::getContent() const {
    return content;
}

bool    Chunk::isEndChunk() {
    return isEnd;
}

void    Chunk::setChunkEnd() {
    isEnd = true;
}

/*
 * chunk struct 초기화 하는 함수
 * length = -1, content = ""
 */
void Chunk::initChunk() {
    isEnd = false;
    length = -1;
    content = "";
}
