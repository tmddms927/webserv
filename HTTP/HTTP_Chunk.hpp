#ifndef HTTP_CHUNK_HPP
#define HTTP_CHUNK_HPP

#include <string>

class  Chunk {
private:
	bool					isEnd;
	long                    length;
	std::string             content;

public:
	void				initChunk();

	bool				isEndChunk();
	void				setChunkEnd();

	long const &		getLength() const;
	std::string const & getContent() const;

	void				setLength(std::string const & len_str);
	long				appendContent(std::string const & content_part);
};

#endif