filename = BrokenPianoSimulator

win:
	i686-w64-mingw32-g++ -w\
	  -Iinclude \
	  src/*.cpp thirdparty/load_obj/*.cpp \
	  include/glad/glad.c \
	  -o $(filename).exe \
	  -Linclude \
	  -lglfw3dll \
	  -static-libstdc++ \
	  -static-libgcc \
	  -static
