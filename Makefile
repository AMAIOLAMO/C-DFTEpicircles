RM = "rm"

run: main
	./main

main: main.c
	cc main.c ./include/** ./private/** -L ./lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o main

clean:
	$(RM) main
