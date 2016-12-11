#Will have to do until I write the Makefile
echo "Cleaning up!"
rm spacer.o
rm ddb_spacer.so
echo "Compiling!"
gcc -Wall -g -fPIC -std=c99 -D_GNU_SOURCE -I/opt/deadbeef/include `pkg-config --cflags gtk+-3.0` spacer.c -c -o spacer.o
echo "Linking!"
gcc -shared -g spacer.o `pkg-config --libs gtk+-3.0` -o ddb_spacer.so
