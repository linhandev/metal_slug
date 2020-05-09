#include "GameWindow.h"
#include <iostream>
using namespace std;
GameWindow::GameWindow()
{
  add_events(Gdk::KEY_PRESS_MASK);

  add(eb);
  eb.add(gf);
  eb.set_events(Gdk::BUTTON_PRESS_MASK);
  eb.signal_button_press_event().connect(
  sigc::mem_fun(*this, &GameWindow::on_eventbox_button_press) );
  show_all_children();
}

bool GameWindow::on_key_press_event(GdkEventKey* key_event)
{
  //GDK_MOD1_MASK -> the 'alt' key(mask)
  //GDK_KEY_1 -> the '1' key
  //GDK_KEY_2 -> the '2' key

  cout<< key_event->keyval<<endl;
  // A 97 向左走
  if(key_event->keyval == 97)
  {
      g_print("going left\n");
      gf.engine.player.set_state("walk", true);

      return true;
  }
  //d
  if(key_event->keyval == 100)
  {
      g_print("going right\n");
      gf.engine.player.set_state("walk");
      return true;
  }

  // w
  if(key_event->keyval == 119)
  {
      g_print("putting gun up\n");
      gf.engine.player.set_state("aimup");
      return true;
  }

  // s
  if(key_event->keyval == 115)
  {
      g_print("get down\n");
      gf.engine.player.set_state("crouch");
      return true;
  }

  //j
  if(key_event->keyval == 106)
  {
      g_print("shooting\n");
      gf.engine.player.set_state("shoot");
      gf.engine.player.shoot();
      return true;
  }
  //k 107 grenade
  if(key_event->keyval == 107)
  {
      g_print("gernade");
      return true;
  }

  //space 32
  if(key_event->keyval == 32)
  {
      g_print("jumping");
      gf.engine.player.set_state("jump");
      return true;
  }


  //if the event has not been handled, call the base class
  return Gtk::Window::on_key_press_event(key_event);
}

bool GameWindow::on_key_release_event(GdkEventKey* key_event)
{
    cout<<key_event->keyval<<"  释放"<<endl;
    int kval = key_event->keyval;
    // 射击，手雷和跳都不需要终止
    if(kval == 106 || kval == 107 || kval == 32)
        return true;
    gf.engine.player.set_state("idle");
    return true;

}
bool GameWindow::on_eventbox_button_press(GdkEventButton*)
{
    g_print("clicked");
    if(GameFrame::status == "pending")
    {
        GameFrame::status = "intro";
        return true;
    }
    if(GameFrame::status == "intro")
    {
        GameFrame::status = "choose";
        return true;
    }
    if(GameFrame::status == "choose")
    {
        GameFrame::status = "man";
        return true;
    }
    if(GameFrame::status == "man")
    {
        gf.engine.start_game();
        GameFrame::status = "playing";
        return true;
    }
    if(GameFrame::status == "lose")
    {
        GameFrame::status = "choose";
        return true;
    }
    


    return true;
}

GameWindow::~GameWindow()
{
}
