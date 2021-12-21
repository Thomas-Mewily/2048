CC		 =	gcc
CFLAGS 	 = -lSDL -lSDLmain
LDFLAGS	 = -lm -ldl 
OBJFILES = CombineBloc.o Font.o SDL_Util.o Util.o  
TARGET	 = jeu

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) $(CFLAGS) $(LDFLAGS)

