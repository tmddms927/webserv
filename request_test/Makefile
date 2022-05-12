OUT		=	chunk_test
CC		=	clang++
CFLAGS	=	-fsanitize=address -g -I../HTTP/ -Wall -Wextra -Werror -fsanitize=address -g
SRCS	=	request_test.cpp\
			../HTTP/HTTPHeaderField.hpp\
			../HTTP/HTTP_req.cpp\
			../HTTP/HTTP.cpp\
			../HTTP/HTTP_Chunk.cpp\
			../utils.cpp\

OBJS	=	request_test.o\
			../HTTP/HTTP_req.o\
			../HTTP/HTTP.o\
			../HTTP/HTTP_Chunk.o\
			../utils.o\


all		:	${OUT} clean

.cpp.o	:
	@${CC} ${CFLAGS} -c ${<} -o ${<:.cpp=.o} 

${OUT}	:	${OBJS}
		@${CC} ${CFLAGS} -o ${OUT} ${OBJS}

clean	:
		@rm -rf ${OBJS}

fclean	:	clean
		@rm -rf ${OUT}

re		:	fclean all

.PHONY	:	all clean fclean re
