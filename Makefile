NAME = Dr0pIt

CC = gcc
CFLAGS = -Wall -Werror -Wextra
CFLAGS += $(addprefix -I , $(INCLUDES_DIR))

INCLUDES_DIR = ./includes/
OBJS_DIR = ./.objs/
SRCS_DIR = ./srcs/

SRC_FILES = dropit.c opt_parse.c download.c
OBJ_FILES = $(SRC_FILES:.c=.o)
OBJS = $(addprefix $(OBJS_DIR), $(OBJ_FILES))
HEADERS = $(INCLUDES_DIR)dropit.h

GREEN = \033[0;32m
RED = \033[0;31m
WHITE = \033[0;0m

HOST_SRCS = host.c

all: $(OBJS_DIR) $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	echo "${GREEN}[+] Dropper compiled !${WHITE}"

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) -o $@ -c $<
	echo "${GREEN}[+] $< compiled${WHITE}"

$(OBJS_DIR):
	mkdir -p $@
	echo "${GREEN}[+] Object file directory created"

host: $(HOST_SRCS)
	$(CC) $(CFLAGS) -o $@ $<
	echo "${GREEN}[+] Host compiled !${WHITE}"

clean:
	$(RM) -Rf $(OBJS_DIR)
	echo "${RED}[+] Object file directory removed${WHITE}"

fclean: clean
	$(RM) $(NAME)
	$(RM) host || true
	echo "${RED}[+] Binaries removed${WHITE}"

re: fclean all

.PHONY: clean re fclean all
.SILENT:
