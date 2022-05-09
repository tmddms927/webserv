OUT		=	webserv
CC		=	clang++
CFLAGS	=	-fsanitize=address -g -I /Server/
#-Wall -Wextra -Werror -fsanitize=address -g
SRCS	=	main.cpp\
			HeaderFieldInfo.cpp\
			HTTP.cpp\
			HTTP_req.cpp\
			HTTP_res.cpp\
			Server/Server_socket.cpp\
			Server/Server_kqueue.cpp\
			Server/Server_method.cpp\
			Server/ContentType/ContentType.cpp\
			Server/Server.cpp\
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

test	:	re
		@ ./webserv test.conf

.PHONY	:	all clean fclean re
