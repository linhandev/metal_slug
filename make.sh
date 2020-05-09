g++ -c util.cpp
g++ -c resources.c `pkg-config gtkmm-3.0 --cflags --libs`
g++ -c GameFrame.cpp  `pkg-config gtkmm-3.0 --cflags --libs`
g++ -c GameWindow.cpp `pkg-config gtkmm-3.0 --cflags --libs`

g++ -o util.o resources.o GameFrame.o GameWindow.o `pkg-config gtkmm-3.0 --cflags --libs` -lsfml-audio
