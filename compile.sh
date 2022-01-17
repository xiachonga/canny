clang++ -c -o Canny.o Canny.cpp -O3 -v --target=aarch64-linux-gnu -march=armv8.2-a+sve  -ffast-math -Rpass=loop -Rpass-analysis=loop
clang++ -o main main.cpp Canny.o -O3 -lm -lpthread -L/usr/X11R6/lib -lm -lpthread -lX11 -I/opt/X11/include
