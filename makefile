NAME= codegen
OBJS= main.o codegen.o memctl.o assembler.o test.o codegen_arith.o

all: $(NAME)

clean:
	rm -rf *.o

$(NAME): $(OBJS)
	@echo Compiling $(NAME).
	gcc -o $(NAME) $(OBJS)

main.o: main.c
	gcc -c main.c

test.o: test.c
	gcc -c test.c

assembler.o: assembler.c
	gcc -c assembler.c

codegen.o: codegen.c
	gcc -c codegen.c

codegen_arith.o: codegen_arith.c
	gcc -c codegen_arith.c

memctl.o: memctl.c
	gcc -c memctl.c