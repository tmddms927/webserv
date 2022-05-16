OUT		=	webserv
CC		=	clang++
CFLAGS	=	-I./HTTP -I./config -Wall -Wextra -Werror
SRCS	=	main.cpp\
			\
			HTTP/HeaderFieldInfo.cpp\
			HTTP/HTTP.cpp\
			HTTP/HTTP_Chunk.cpp\
			HTTP/HTTP_req.cpp\
			HTTP/HTTP_res.cpp\
			HTTP/HTTP_CGI.cpp\
			\
			Server/Server.cpp\
			Server/Server_socket.cpp\
			Server/Server_kqueue.cpp\
			Server/Server_method.cpp\
			Server/Server_utils.cpp\
			Server/Server_cgi.cpp\
			Server/ContentType/ContentType.cpp\
			Server/URIParser/URIParser.cpp\
			\
			config/Config.cpp\
			config/ServerBlock.cpp\
			\
			autoindex/AutoIndex.cpp\
			\
			CGIInterface/CGIInterface.cpp\
			utils.cpp
OBJS	=	${SRCS:.cpp=.o}

all		:	${OUT}

.cpp.o	:
	@${CC} ${CFLAGS} -c ${<} -o ${<:.cpp=.o} 

${OUT}	:	${OBJS}
		@${CC} ${CFLAGS} -o ${OUT} ${OBJS}

clean	:
		@rm -rf ${OBJS}

fclean	:	clean
		@rm -rf ${OUT}

re		:	fclean all

test	:	all
		@ ./webserv test.conf

s	:	all
		@ ./webserv seungoh_test.conf

j	:	all
		@ ./webserv jeokim_test.conf

.PHONY	:	all clean fclean re
