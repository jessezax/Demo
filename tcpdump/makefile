src = $(wildcard ./*.c)  
obj = $(patsubst %.c, %.o, $(src))  
#obj = $(src:%.c=%.o)  
target = app  
CC = g++ -g
$(target): $(obj)  
	    $(CC) $(obj) -o $(target)  
%.o: %.c  
	    $(CC) -c $< -o $@  

.PHONY: clean  
clean:  
	rm -rf $(obj) $(target)
