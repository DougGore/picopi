CC = g++
TARGET = testtts
LIBS = -lexpat -lsvoxpico
# CFLAGS = -fPIC -Wall -Wextra -O2 -g # C flags
# #LDFLAGS = -shared  # linking flags
# RM = rm -f  # rm command
# TARGET_LIB = testtts # target lib
 
# #SRCS = main.c src1.c src2.c # source files
# SRCS := strdup16to8.c \
# 		strdup8to16.c \
# 		svox_ssml_parser.cpp \
# 		com_svox_picottsengine.cpp \
# 		main.cpp

# OBJS = $(SRCS:.c=.o)
# #OBJS += $(SRCS:.cpp=.o)
 
# .PHONY: all
# all: ${TARGET_LIB}
 
# $(TARGET_LIB): $(OBJS)
# 	$(CC) ${LDFLAGS} -o $@ $^
 
# $(SRCS:.c=.d):%.d:%.c
# 	$(CC) $(CFLAGS) -MM $< >$@
 
# include $(SRCS:.c=.d)
 
# .PHONY: clean
# clean:
# 	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)

all:
	$(CC) -L../lib/ strdup16to8.c strdup8to16.c svox_ssml_parser.cpp com_svox_picottsengine.cpp main.cpp $(LIBS) -I../compat/include/ -I../lib/ -fpermissive -o $(TARGET)
