CC=gcc
RM=rm
MV=mv
CD=cd
CP=cp
AR=ar

INC= -I../inc


ifdef DEBUG
CFLAGS   = -g -O0
TARGET_DIR=$(BUILD_DIR)/debug
else
CFLAGS   = -O2
TARGET_DIR=$(BUILD_DIR)/release
endif

CFLAGS += -Wall -Werror $(INC)

#Out file path
OPath=$(TARGET_DIR)/obj/libx

LIBTARGET= $(TARGET_DIR)/libx.a

# all object files.
OBJ=$(OPath)/x_string.o $(OPath)/x_conf.o $(OPath)/x_malloc.o \
	$(OPath)/x_misc.o $(OPath)/x_module.o $(OPath)/x_ftp.o \
	 $(OPath)/x_connection.o  $(OPath)/x_tcp_port.o $(OPath)/x_log.o

sub_dirs=

LIBS=$(shell libs=;for subdir in $(sub_dirs);do libs=$$libs"$$subdir/.obj/*.o ";done;echo $$libs;)

all:start $(TARGET_DIR) $(OPath) $(OBJ) sub-dirs libx install end

libx:
	@$(AR) -crs $(LIBTARGET) $(OBJ) $(LIBS)
install:

start:
	@echo -e "\033[44;37;5m Make start  \033[0m"
	@make clean_obj

end:
	@echo -e "\033[44;37;5m Make finish \033[0m"

$(TARGET_DIR):
	@mkdir -p $(TARGET_DIR) 2> /dev/null

$(OPath):
	@mkdir -p $(OPath) 2> /dev/null

$(OPath)/%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<

sub-dirs:
	@for subdir in $(sub_dirs);\
	do\
		echo -e "\033[44;37;5m Make Dir [$$subdir]  \033[0m";\
		make -C $$subdir DEBUG=$(DEBUG);\
		ret=$$?;\
		if [ ! $$ret -eq "0" ];then \
			echo -e "\033[44;37;5m Make Error [$$ret]  \033[0m";\
			exit;\
		fi \
	done

clean:
	@$(RM) -rf $(OPath) > /dev/null 2>&1
	@$(RM) -f  $(LIBTARGET) > /dev/null

	@for subdir in $(sub_dirs);\
	do\
		echo  -e "\033[44;37;5m Clean [$$subdir]  \033[0m";\
		make -C $$subdir clean > /dev/null;\
	done

clean_obj:
	@$(RM) -f $(OBJ) > /dev/null

	@for subdir in $(sub_dirs);\
	do\
		echo -e "\033[44;37;5m Clean Obj [$$subdir]  \033[0m";\
		make -C $$subdir clean > /dev/null;\
	done
