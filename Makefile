PROJ = rad

CC = cc
CFLAGS = -g -I. -I/usr/include/
CPPFLAGS = -std=c++11 $(CFLAGS)

BASIC_LNFLAGS = -lm 
GL_LNFLAGS =  -lglut -lGL 
LNFLAGS = $(GL_LNFLAGS) $(BASIC_LNFLAGS) 

OBJS = room.o draw.o rad.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

%.o: %.c++ $(DEPS)
	$(CC) $(CPPFLAGS) -o $@ -c $< 

%.o: %.cpp $(DEPS)
	$(CC) $(CPPFLAGS) -o $@ -c $< 

$(PROJ) : $(OBJS)
	$(CC) $(OBJS) $(LNFLAGS) -o $(PROJ)

clean:
	rm $(OBJS)

