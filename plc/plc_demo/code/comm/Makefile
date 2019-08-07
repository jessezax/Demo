CC=gcc
RM=rm
MV=mv
CD=cd
CP=cp
AR=ar

INC= -I../inc

ifdef DEBUG
CFLAGS   = -g -O0 -lcrypto
TARGET_DIR=$(BUILD_DIR)/debug
TARGET  = $(TARGET_DIR)/comm_core_debug
else
CFLAGS   = -O2 -lcrypto
TARGET_DIR=$(BUILD_DIR)/release
TARGET  = $(TARGET_DIR)/comm_core

endif

LIBX    = $(TARGET_DIR)/libx.a
LDFLAGS = -lpthread -lrt -L/usr/lib64/mysql -lmysqlclient -lmxml -lcurl -lcrypto

CFLAGS += -Wall -Werror $(INC)

#Out file path
OPath=$(TARGET_DIR)/obj/comm

# all object files.
OBJ=$(OPath)/comm_conf.o $(OPath)/comm_core.o $(OPath)/comm_port.o $(OPath)/pm_msg_handle.o \
	$(OPath)/comm_connection.o $(OPath)/comm_msg.o $(OPath)/comm_mysql.o $(OPath)/pm_hash.o \
	$(OPath)/pm_rule_id_flag_init.o $(OPath)/pm_pro_app_xml_init.o $(OPath)/comm_base64.o\
	$(OPath)/comm_timer_task.o $(OPath)/main.o $(OPath)/comm_rule_file.o $(OPath)/comm_rule_version.o $(OPath)/pm_msg_ack.o\
	$(OPath)/comm_plc_msg.o $(OPath)/comm_plc_port.o $(OPath)/comm_plc_msg_handle.o $(OPath)/comm_plc_notifier.o \
	$(OPath)/pm_msg_response.o $(OPath)/pm_plc_log.o $(OPath)/pm_msg_user.o\
	$(OPath)/pm_plc_mysql_del.o $(OPath)/pm_plc_mysql_insert.o $(OPath)/pm_plc_mysql_handle.o \
	$(OPath)/pm_msg_handle_account.o \
	$(OPath)/pm_rule_generate_account.o \
	$(OPath)/pm_rule_generate.o\

sub_dirs=

LIBS=$(shell libs=;for subdir in $(sub_dirs);do libs=$$libs"$$subdir/.obj/*.o ";done;echo $$libs;)

all:start $(TARGET_DIR) $(OPath) $(OBJ) sub-dirs comm_core install end

comm_core:
	@$(CC) -o $(TARGET) $(OBJ) $(LIBS) $(LIBX) $(LDFLAGS)
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
	@$(RM) -f $(TARGET) > /dev/null

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
