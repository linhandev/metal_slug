#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "GameFrame.h"
#include <gtkmm.h>

class GameWindow : public Gtk::Window
{
public:
  GameWindow();
  virtual ~GameWindow();

private:
  //Override default signal handler:
  GameFrame gf;
  Gtk::EventBox eb;
  bool on_key_press_event(GdkEventKey* event) override;
  bool on_key_release_event(GdkEventKey* event) override;
  bool on_eventbox_button_press(GdkEventButton* button_event);

};

#endif //GAMEWINDOW_H
