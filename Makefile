OUT		=	webserv
CC		=	clang++
CFLAGS	=	-fsanitize=address -g -I./HTTP -I./config
#-Wall -Wextra -Werror -fsanitize=address -g
SRCS	=	main.cpp\
			HTTP/HeaderFieldInfo.cpp\
			HTTP/HTTP.cpp\
			HTTP/HTTP_Chunk.cpp\
			HTTP/HTTP_req.cpp\
			HTTP/HTTP_req2.cpp\
			HTTP/HTTP_res.cpp\
			Server_socket.cpp\
			Server_kqueue.cpp\
			Server_method.cpp\
			Server.cpp\
			config/Config.cpp\
			config/ServerBlock.cpp\
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

.PHONY	:	all clean fclean re
