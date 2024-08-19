PROGRAM	= sort

SRC			= sort.c

OBJS		= ${SRC:.c=.o}

all: ${PROGRAM}

${PROGRAM}: ${OBJS}
	gcc -g3 ${OBJS} -o ${PROGRAM}

%.o: %.c
	gcc -g3 -c $< -o $@

tui_gdb: all
	gdb --tui ${PROGRAM}

gdb: all
	gdb ${PROGRAM}

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${PROGRAM}

re: fclean all
