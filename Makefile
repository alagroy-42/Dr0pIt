NAME = DropIt

CC = gcc
CFLAGS = -Wall -Werror -Wextra

SRCS = dropper.c
HOST_SRCS = host.c

GREEN = \033[0;32m

all: $(NAME) host

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $<
	echo "${GREEN}[+] Dropper compiled !"

clean:
	$(RM) $(NAME)

host: $(HOST_SRCS)
	$(CC) $(CFLAGS) -o $@ $<
	echo "${GREEN}[+] Host compiled !"

re: clean all

.SILENT:
