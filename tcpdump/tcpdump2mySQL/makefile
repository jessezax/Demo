src = $(wildcard ./*.c)  
obj = $(patsubst %.c, %.o, $(src))  
#obj = $(src:%.c=%.o)  
target = connect_mysql  
CC = g++ -g -I/usr/include/mysql/  -L/usr/lib64/mysql/ -lmysqlclient -lz
$(target): $(obj)  
	    $(CC) $(obj) -o $(target)  
%.o: %.c  
	    $(CC) -c $< -o $@  

.PHONY: clean  
clean:  
	rm -rf $(obj) $(target)
