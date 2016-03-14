###################################################################
#
# windowless OpenGL rendering example 
#
# freeman.justin@gmail.com
#
##################################################################


OBJ=	main.o 

# Include flags
INC=   

# Libraries
LFLAGS= -framework AppKit \
	-framework OpenGL 

CFLAGS=	-O3 -g -Wall -Wno-deprecated-declarations

CC=	gcc $(CFLAGS) $(INC) 


# Executable

EXEC=	./test	

$(EXEC):$(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(LFLAGS)

clean:
	rm $(OBJ)
	rm $(EXEC)
