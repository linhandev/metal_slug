// #include "GameFrame.h"
#include "GameWindow.h"
#include <gtkmm/application.h>
#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    GameWindow win;
    win.set_title("合金弹头");
    win.set_default_size(GameFrame::display_width, GameFrame::display_height);

    // GameFrame gf;
    // win.add(gf);
    // gf.show();

    return app->run(win);
}
