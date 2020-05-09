#ifndef GAMEFRAME
#define GAMEFRAME

#include "util.h"
#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>
#include <vector>
#include <string>
#include "SFML/Audio.hpp"

using namespace std;

/*
gameframe是游戏过程中的窗口，只负责render，gf只看到一个当前静态的局面按照局面画出来。
gameengine创建和跟踪游戏过程中所有的对象，进行碰撞检测之类的
所有的对象都有自己绘制方法

所有对象在位置以游戏背景左上角为原点，x是横向，y是纵向，记录在背景上的位置
所有绘制方法要以实际屏幕上一个点为基准点，屏幕上绘制范围外的元素避免绘制
*/

/*
地图，背景绘制，地平线存储，
*/
class GameMap
{
    static vector<Line>grounds; // 地平线
    Glib::RefPtr<Gdk::Pixbuf> background; // 游戏背景图 10544 × 700

public:
    static int curr_map_x; // 当前map从这个数开始画，之前的都已经走过了
    GameMap();
    void draw(Glib::RefPtr<Gdk::Pixbuf> &disp_buf);
    bool adjust_map(DP playerx);
    static int on_ground(DP pos);
};

/*
玩家
*/
class Player
{
    DP lr_pos; // 玩家左上角位置
    const int scale = 2; // 屏幕显示是素材4倍面积
    Glib::RefPtr<Gdk::Pixbuf> curr_pic; // 当前玩家图片
    int speed_x = 0, speed_y = 0; // 玩家当前移动速度
    int acc_x = 0, acc_y = 10; // 玩家的加速度，更新位置的时候会根据加速度更新速度，主要是实现跳的效果
    int walk_speed = 10;
    bool face_right = true;
    bool alive = true;
    string curr_state = "idle"; // 当前player状态
    vector<string> state_table; // 记录所有可能的状态
    map<string, string>next_state; // 默认下一个状态
    map<string, int>frame_per_image; // 每张图片显示多少帧
    int curr_frame_count = 0; // 当前这张图片显示了多少帧
    int curr_pic_num = 0; // 当前正在展示第几张图片
    int health = 100;
    int after_effect_count = 100;
    bool switch_state(string new_state);
public:
    Player();
    void draw(Glib::RefPtr<Gdk::Pixbuf> &disp_buf);
    void shoot();
    void grenade();
    bool set_state(string new_state, bool is_left=false);

    bool add_speed(int sx, int xy);
    // bool sub_speed(int sx, int sy);
    bool set_speed(int sx, int sy);
    bool move();
    // bool on_ground();
    bool is_alive(){return alive; };
    void take_damage(int amount);
    DP get_lr(){return lr_pos; };
    DP get_gun();
    Box get_box();
    void set_y(int y){lr_pos.set_y(y);};
    bool expire();
    void refresh();
};

/*
敌人基类，所有的敌人都有alert，寻路，攻击这些方法
*/
class Enemy
{
    DP lr_pos; // 玩家左上角位置
    int scale = 2; // 屏幕显示是素材4倍面积
    Glib::RefPtr<Gdk::Pixbuf> pic; // 当前玩家图片
    Glib::RefPtr<Gdk::Pixbuf> flipped_pic; // 当前玩家图片
    int speed_x = 0, speed_y = 0; // 玩家当前移动速度
    int acc_x = 0, acc_y = 5; // 玩家的加速度，更新位置的时候会根据加速度更新速度，主要是实现跳的效果
    int walk_speed = 3;
    bool face_left = true;
    int health = 100;
    string type;
    int clear_frame_count = 80;
    int attack_interval = 40;
    int attack_count = 0;
    bool dead = false;
    bool alerted = false;
    bool flyer = false;
    int dist;
public:
    Enemy(DP pos, string type);
    void draw(Glib::RefPtr<Gdk::Pixbuf> &disp_buf);
    // Point get_foot_point();
    DP get_gun();
    void decide_attack(int px);
    void attack(int px);
    void alert(int player_x);
    void take_damage(int amout);
    Box get_box();
    void die();
    void move(DP player_pos);
    bool expire();
    bool is_alerted() {return alerted; };
    bool is_alive() {return !dead; };
};


class Projectile
{
    DP midend_pos; //尾部中点
    int speed_x=0,speed_y=0;
    int acc_x=0, acc_y=0;
    int speed = 30;
    int scale = 1;
    int dist = 0;
    int damage = 30;
    string direction; // 飞行方向
    string owner = "player"; //玩家的子弹打不到自己
    Glib::RefPtr<Gdk::Pixbuf> pic; // 当前玩家图片
    string type;
public:
    Projectile(DP pos, string type, string dir, string o);
    DP get_head(); //返回头部中点，用这个判断碰撞
    void move();
    void draw(Glib::RefPtr<Gdk::Pixbuf> &disp_buf);
    DP get_ul();
    bool is_moving();
    void stop();
    bool in_box(Box);
    int get_damage();
    bool from_player(){return owner=="player"; }
};


/*
游戏引擎，创建所有对象，进行碰撞检测，接收键盘指令
*/
class GameEngine
{
public:
    GameMap gamemap;
    Player player;
    string end_by;
public:
    static vector<Enemy>enemies;
    static vector<Projectile>projectiles;
    GameEngine();
    void start_game();
    void end_game();
    bool judge_end_game();
    void render(Glib::RefPtr<Gdk::Pixbuf> &disp_buf);
    bool judge_end();
};

/*
游戏画面，负责绘制
*/
class GameFrame : public Gtk::DrawingArea
{
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    bool on_timeout();
    Glib::RefPtr<Gdk::Pixbuf> disp_buf;
    void show_image();
public:
    static string status;
    GameEngine engine;
    static Point map_ul;
    static int display_width;
    static int display_height;

    GameFrame();
    virtual ~GameFrame();
    sigc::connection timer;


};


#endif // GAMEFRAME
