# metal_slug
A C++/GTKmm implementation of metal_slug, work in progress but can be a good start for further development. Contain some good visual and sound materials for player and enemies.

# Requirement
GTKmm is required, see [GTKmm Docs](https://developer.gnome.org/gtkmm-tutorial/stable/chapter-installation.html) for detailed installation guide. I have tried on Manjaro Linux and Mac
```shell
# manjaro 
sudo pacman -S gtkmm3

# mac
brew install gtkmm3
```
[Mac installation guide](https://medium.com/@ivyzhou/how-to-use-gtkmm-with-glade-in-xcode-69745c8401a9)

# Compile

Compile the resources first, this turns image files into c code, speed up reading while running and avoids missing resource problems.
```shell
glib-compile-resources --target=resources.c --generate-source image.gresource.xml
```
Compile the actual game, on Mac the `pkg-config gtkmm-3.0 --cflags --libs` command may not work in all directories. If so, run it and replace it with the output while compiling.
```shell
g++ resources.c  util.cpp GameFrame.cpp GameWindow.cpp metal_slug.cpp -o metal `pkg-config gtkmm-3.0 --cflags --libs`
```

U R ready to kill some zombies
```shell
./metal
```

# TODOs
- [ ] Code clean up
- [ ] State transactions is not smooth enough
- [ ] Adding sound effect
- [ ] Implement more enemies and projectiles
- [ ] Implement explode effects
- [ ] Add more ground to the map so player and enemies can jump onto
- [ ] Find more resource for enemies, currently it's just 3 pictures.

Should u have any problems, contact me at davidlindev@qq.com
