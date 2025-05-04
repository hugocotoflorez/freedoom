filename = BrokenPianoSimulator

win:
	rm ./objs -rf
	mkdir -p objs
	i686-w64-mingw32-gcc -w thirdparty/vshcfp/src/*.c -c
	mv *.o objs

	i686-w64-mingw32-g++ -w\
	  -Iinclude \
	  src/*.cpp thirdparty/load_obj/*.cpp \
	  objs/*.o \
	  include/glad/glad.c \
	  -o $(filename).exe \
	  -Linclude \
	  -lglfw3dll \
	  -static-libstdc++ \
	  -static-libgcc \
	  -static
	rm ./objs -rf
