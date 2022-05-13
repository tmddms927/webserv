OUT		=	cgi
CC		=	clang++
CFLAGS	=	-fsanitize=address -g -I../HTTP/ -Wall -Wextra -Werror -fsanitize=address
SRCS	=	onli_cgi-server.cpp\
			../HTTP/HTTPHeaderField.hpp\
			../CGIInterface/CGIInterface.cpp\
			../HTTP/HTTP_req.cpp\
			../HTTP/HTTP_res.cpp\
			../HTTP/HTTP.cpp\
			../HTTP/HTTP_Chunk.cpp\
			../HTTP/HTTP_CGI.cpp\
			../utils.cpp\

OBJS	=	onli_cgi-server.o\
			../CGIInterface/CGIInterface.o\
			../HTTP/HTTP_CGI.o\
			../HTTP/HTTP_req.o\
			../HTTP/HTTP_res.o\
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
