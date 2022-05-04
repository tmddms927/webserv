OUT		=	webserv
CC		=	clang++
CFLAGS	=	-Wall -Wextra -Werror -fsanitize=address -g
SRCS	=	main.cpp\
			HeaderFieldInfo.cpp\
			HTTP.cpp\
			HTTP_req.cpp\
			HTTP_res.cpp\
			SocketController.cpp\
			SocketController_kqueue.cpp
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