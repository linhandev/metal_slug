#include "GameFrame.h"
#include <cairomm/context.h>
#include <giomm/resource.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <glibmm/fileutils.h>
#include <iostream>
#include <gtkmm.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>


using namespace std;


/**************** GameMap *****************/
int GameMap::curr_map_x = 0;
vector<Line> GameMap::grounds;

GameMap::GameMap()
{
    try
    {
      background = Gdk::Pixbuf::create_from_resource("/resources/util/map.png");
    }
    catch(const Gio::ResourceError& ex)
    {
      std::cerr << "ResourceError: " << ex.what() << std::endl;
    }
    catch(const Gdk::PixbufError& ex)
    {
      std::cerr << "PixbufError: " << ex.what() << std::endl;
    }
}

void GameMap::draw(Glib::RefPtr<Gdk::Pixbuf> &disp_buf)
{
    int dispx = GameFrame::display_width,dispy = GameFrame::display_height;
    background->copy_area(curr_map_x, 0, dispx, dispy, disp_buf, 0, 0);
}


bool GameMap::adjust_map(DP point)
{
    int x = int(point.get_x());
    // cout<<"curr dp pointx "<< x <<" "<< curr_map_x<<endl;
    // cout<<x - curr_map_x<<" "<< 10544 - GameFrame::display_width - 10<<endl;
    if(x - curr_map_x >= 500 && curr_map_x <= 10544 - GameFrame::display_width - 10)
        curr_map_x += 10;
    if(x - curr_map_x < 200 && curr_map_x >= 10)
        curr_map_x -= 10;
    return true;
}

int GameMap::on_ground(DP pos)
{
    int px = int(pos.get_x()), py = int(pos.get_y());
    if(py >= 600)
        return 600;

    for(int i=0;i<grounds.size();i++)
        if(px >= grounds[i].get_min_x() && px<=grounds[i].get_max_x())
            if(py - grounds[i].get_y() <= 60)
                if(py <= grounds[i].get_y())
                    return int(grounds[i].get_y());
    return 0; // 如果在天上返回 0
}

/**************** Player ******************/
bool Player::expire()
{
    if(after_effect_count <= 0)
        return true;
    return false;
}

Player::Player()
{
    lr_pos.set_pos(100, 600);

    state_table.push_back("idle");
    next_state["idle"] = "idle";
    frame_per_image["idle"] = 3;

    state_table.push_back("turn");
    next_state["turn"] = "idle";
    frame_per_image["turn"] = 1;

    state_table.push_back("putup");
    next_state["putup"] = "aimup";
    frame_per_image["putup"] = 2;

    state_table.push_back("aimup");
    next_state["aimup"] = "aimup";
    frame_per_image["aimup"] = 2;

    state_table.push_back("putdown");
    next_state["putdown"] = "idle";
    frame_per_image["putdown"] = 2;


    state_table.push_back("shoot");
    next_state["shoot"] = "idle";
    frame_per_image["shoot"] = 1;

    state_table.push_back("grenade");
    next_state["grenade"] = "idle";
    frame_per_image["grenade"] = 2;

    state_table.push_back("walk");
    next_state["walk"] = "walk";
    frame_per_image["walk"] = 2;

    state_table.push_back("crouch");
    next_state["crouch"] = "crouch";
    frame_per_image["crouch"] = 2;

    state_table.push_back("crouch_shoot");
    next_state["crouch_shoot"] = "crouch";
    frame_per_image["crouch_shoot"] = 2;

    state_table.push_back("jump");
    next_state["jump"] = "fall";
    frame_per_image["jump"] = 2;

    state_table.push_back("fall");
    next_state["fall"] = "idle";
    frame_per_image["fall"] = 2;

    state_table.push_back("dying");
    next_state["dying"] = "dead";
    frame_per_image["dying"] = 4;

    state_table.push_back("dead");
    next_state["dead"] = "dead";
    frame_per_image["dead"] = 10;

    state_table.push_back("win");
    next_state["win"] = "win";
    frame_per_image["win"] = 4;

// grenade
// crouch grenade
}


// 只有向左走的时候需要设置方向，没有方向的或者向右的都不需要写
bool Player::set_state(string new_state, bool is_left)
{
    // 1.判断拒绝转换
    // 1.1 拒绝连点，一个状态必须完成之后才能重新执行
    if(curr_state == new_state)
        return false;

    // 1.2 状态转换写的是目标状态的名字，如果正在去往这个目标的过渡上，拒绝这个转换
    if(curr_state == "turn" && new_state == "walk")
        return false;
    if(curr_state == "putup" && new_state == "aimup")
        return false;

    // 1.3 转向状态不可打断
    if(curr_state == "turn")
        return false;

    // 1.4 跳跃一般都只点一下
    if( (curr_state == "jump" || curr_state == "fall") && new_state == "idle")
        return false;

    // 1.5 向上射击没有特别的动画，直接打就完了
    if( (curr_state == "aimup" || curr_state == "putup") && new_state == "shoot")
        return false;

    // 1.6 不合法的状态
    bool flag = true;
    for(int i=0; i<state_table.size();i++)
        if(state_table[i] == new_state)
            flag = false;
    if(flag)
    {
        cout<< "[ERROR]设置了一个不支持的curr_state" + new_state<<endl;;
        return false;
    }

    // 3. 修改当前参数
    if(curr_state == "idle" && new_state == "walk")
    {
        if(is_left != face_right)
            new_state = "walk";
        else
            new_state = "turn";

        if(is_left)
            face_right = false;
        else
            face_right = true;
    }
    if(curr_state == "idle" && new_state == "aimup")
        new_state = "putup";


    if(curr_state == "aimup" && new_state == "idle")
        new_state = "putdown";

    if((curr_state == "crouch" || curr_state == "crouch_shoot") && new_state == "shoot")
        new_state = "crouch_shoot";

    // new_state 确定
    return switch_state(new_state);
}

void Player::refresh()
{
    GameMap::curr_map_x=0;
    lr_pos.set_pos(100,600);
    speed_x = 0, speed_y = 0;
    acc_x = 0, acc_y = 10;
    walk_speed = 10;
    face_right = true;
    alive = true;
    string curr_state = "idle"; // 当前player状态
    curr_frame_count = 0; // 当前这张图片显示了多少帧
    curr_pic_num = 0; // 当前正在展示第几张图片
    health = 100;
    after_effect_count = 100;
}
bool Player::switch_state(string new_state)
{
    cout<<curr_state<<" "<<new_state<<endl;
    if( (curr_state == "idle" || curr_state == "turn") && new_state == "walk")
        add_speed(walk_speed, 0);

    if(new_state == "idle" && (speed_x !=0 || speed_y != 0))
        set_speed(0, 0);


    if(new_state == "jump")
        add_speed(0, 80);
    // 更新 curr_state
    curr_state = new_state, curr_frame_count = 0, curr_pic_num = 0;
    return true;
}


void Player::draw(Glib::RefPtr<Gdk::Pixbuf> &disp_buf)
{
    if(curr_state != "dying" && curr_state != "dead" && ! is_alive() )
        switch_state("dying");
    if(curr_state == "dead" || curr_state == "win")
        after_effect_count --, cout<<after_effect_count<<endl;

    string curr_path = "/resources/player/" + curr_state + "/" + to_string(curr_pic_num) + ".png";
    // cout<<curr_path<<endl;
    try
    {
      curr_pic = Gdk::Pixbuf::create_from_resource(curr_path);
    }
    catch(const Gio::ResourceError& ex)
    {
      // std::cerr << "ResourceError: " << ex.what() << std::endl;
      // cout<<"resource"<< curr_state<<" "<<next_state[curr_state]<<endl;
      switch_state(next_state[curr_state]);

      string curr_path = "/resources/player/" + curr_state + "/" + to_string(curr_pic_num) + ".png";
      curr_pic = Gdk::Pixbuf::create_from_resource(curr_path);
    }
    catch(const Gdk::PixbufError& ex)
    {
      std::cerr << "PixbufError: " << ex.what() << std::endl;
      return;
    }

    if(!face_right && curr_state != "turnleft") //如果当前人物向左，除了左转动画图片都要翻转
    {
        // cout<<"flipping"<<endl;
        curr_pic = curr_pic -> flip(true); //水平翻转
    }
    int posx = lr_pos.get_x() - curr_pic->get_width() * scale - GameMap::curr_map_x;
    int posy = lr_pos.get_y() - curr_pic->get_height() * scale;
    curr_pic -> composite(disp_buf, posx, posy, curr_pic->get_width() * scale, curr_pic->get_height() * scale, posx, posy, scale, scale, Gdk::InterpType::INTERP_NEAREST, 255);

    curr_frame_count++;
    // cout<<"当前帧计数"<<curr_frame_count<<endl;
    // cout<<"frame_per_image"<<frame_per_image[curr_state]<<endl;
    if(curr_frame_count == frame_per_image[curr_state])
    {
        // cout<<"下一张"<<endl;
        curr_frame_count = 0, curr_pic_num++;
    }
}

// 向前进方向和上添加这个速度
bool Player::add_speed(int sx, int sy)
{
    // cout<<face_right<<endl;
    if(face_right)
    {
        speed_x += sx, speed_y -= sy;
         g_print("add speed pos");
    }
    else
    {
        speed_x -= sx, speed_y -= sy;
        g_print("add speed neg");
    }

    return true;
}

bool Player::set_speed(int sx, int sy)
{
    speed_x = sx, speed_y = sy;
    return true;
}
// 根据当前的速度和加速度 todo 在这里或者map的adjust进行走到边界的检测
bool Player::move()
{
    // cout<<"player_moving"<<speed_x<<endl;
    lr_pos.add_x_y(speed_x, speed_y);
    int onground = GameMap::on_ground(lr_pos);
    if(onground == 0)
        speed_y += acc_y;
    else
        speed_y = 0, set_y(onground);
    return true;
}

// bool Player::on_ground()
// {
//     if(lr_pos.get_y() >= 600)
//         return true;
//     return false;
// }

void Player::shoot()
{
    string direction;
    if(face_right)
        direction = "right";
    else
        direction = "left";
    if(curr_state == "aimup" || curr_state == "putup")
        direction = "up";

    // cout<<"bullet x"<<get_gun().get_x()<<endl;
    // cout<<"player x"<<lr_pos.get_x()<<endl;
    GameEngine::projectiles.push_back(Projectile(get_gun(), "bullet", direction, "player"));
}

void Player::grenade()
{

}
DP Player::get_gun()
{
    int x,y;

    if(face_right)
        x = int(lr_pos.get_x() - 5);
    else
        x = int(lr_pos.get_x() - 25);

    if(curr_state == "aimup" || curr_state == "putup")
    {
        y = lr_pos.get_y() - 70;
        if(face_right)
            x = int(lr_pos.get_x() - 25);
        else
            x = int(lr_pos.get_x() - 5);
    }
    else
        y = lr_pos.get_y() - 30;
    return DP(x,y);
}

Box Player::get_box()
{
    Point ul = Point(int(lr_pos.get_x() - curr_pic->get_width()), int(lr_pos.get_y() - curr_pic->get_height()) );
    Point lr = Point(int(lr_pos.get_x()), int(lr_pos.get_y()) );
    // cout<<ul.get_x()<<" "<<ul.get_y()<<" "<<lr.get_x()<<" "<<lr.get_y()<<endl;
    return Box(ul, lr);
}
void Player::take_damage(int amount)
{
    health -= amount;
    if(health <0)
        health = 0, alive = false;
}
/*************** Enemy *********************/
Enemy::Enemy(DP pos, string t):lr_pos(pos)
{
    type = t;
    walk_speed = rand() % 5;
    if(type == "ufo")
        flyer = true;
    if(flyer)
        dist = rand() % 10 + 10;
    else
        dist = rand() % 40 + 180;
    try
    {
        pic = Gdk::Pixbuf::create_from_resource("/resources/enemy/"+ type + "/idle.png");
    }
    catch(const Gio::ResourceError& ex)
    {
        std::cerr << "ResourceError: " << ex.what() << std::endl;
    }
    catch(const Gdk::PixbufError& ex)
    {
        std::cerr << "PixbufError: " << ex.what() << std::endl;
    }
    flipped_pic = pic -> flip(true);
}


void Enemy::draw(Glib::RefPtr<Gdk::Pixbuf> &disp_buf)
{
    // 如果死了在屏幕上倒计时，到0清除
    if(dead)
        clear_frame_count--;

    int ulx = int(lr_pos.get_x() - pic->get_width() * scale - GameMap::curr_map_x), uly = int(lr_pos.get_y() - pic->get_height() * scale);
    if(ulx <= 0 || ulx >= GameFrame::display_width)
        return;
    if(face_left)
        pic -> composite(disp_buf, ulx, uly, pic->get_width() * scale, pic->get_height() * scale, ulx, uly, scale, scale, Gdk::InterpType::INTERP_NEAREST, 255);
    else
        flipped_pic -> composite(disp_buf, ulx, uly, pic->get_width() * scale, pic->get_height() * scale, ulx, uly, scale, scale, Gdk::InterpType::INTERP_NEAREST, 255);

}

void Enemy::take_damage(int amount)
{
    health -= amount;
    if(health <= 0)
        die();
}


void Enemy::die()
{
    try
    {
        pic = Gdk::Pixbuf::create_from_resource("/resources/enemy/"+ type + "/dead.png");
    }
    catch(const Gio::ResourceError& ex)
    {
        std::cerr << "ResourceError: " << ex.what() << std::endl;
    }
    catch(const Gdk::PixbufError& ex)
    {
        std::cerr << "PixbufError: " << ex.what() << std::endl;
    }
    dead = true;
}


void Enemy::move(DP player_pos)
{
    int px = int(player_pos.get_x()), ex = int(lr_pos.get_x());

    // 如果被惊动了，就要去找打，只在x方向走，y方向交给地图判断下落
    if(alerted)
        if(flyer) //帖脸扔东西的怪
        {
            speed_x = 0;
            if(px <= ex - dist) // 向左走
                speed_x = -walk_speed;
            if(ex + dist <= px  ) // 向右走
                speed_x = walk_speed, face_left=false;
        }
        else
        {
            speed_x = 0;
            if(px < ex && ex - px >=dist) // 向左走
                speed_x = -walk_speed;
            if(ex < px && px - ex >= dist) // 向右走
                speed_x = walk_speed, face_left=false;
        }

    int onground = GameMap::on_ground(lr_pos);
    if(onground == 0)
        speed_y += acc_y;
    else
        speed_y = 0, lr_pos.set_y(onground);
    if(flyer)
        speed_y = 0;
    lr_pos.add_x_y(speed_x, speed_y);
}

void Enemy::decide_attack(int px)
{
    int ex = int(lr_pos.get_x());
    // cout<<ex<<" "<<px<<endl;
    if(attack_count != attack_interval)
    {
        attack_count++;
        return;
    }

    if(flyer)
    {
        if(abs(ex - px) <= 100)
            attack(px);
    }
    else
    {
        if(abs(ex - px) <= 300)
            attack(px);
    }
}

void Enemy::attack(int px)
{
    // cout<<"attacking"<<endl;
    attack_count=0;
    int ex = int(lr_pos.get_x());
    if(flyer)
        GameEngine::projectiles.push_back(Projectile(get_gun(),"bomb", "down", "enemy") );
    else
    {
        if(px<ex)
            face_left = true, GameEngine::projectiles.push_back(Projectile(get_gun(),"bullet", "left", "enemy"));
        else
            face_left = false, GameEngine::projectiles.push_back(Projectile(get_gun(),"bullet", "right", "enemy"));
    }

}
bool Enemy::expire()
{
    if(clear_frame_count <= 0)
        return true;
    return false;
}

// todo 这个box应该不太准
Box Enemy::get_box()
{
    Point ul = Point(int(lr_pos.get_x() - pic->get_width()), int(lr_pos.get_y() - pic->get_height()) );
    Point lr = Point(int(lr_pos.get_x()), int(lr_pos.get_y()) );
    return Box(ul, lr);
}
void Enemy::alert(int player_x)
{
    if(alerted)
        return;
    if( abs(lr_pos.get_x() - player_x) <= 1000 )
        alerted = true;
}

DP Enemy::get_gun()
{
    int lrx = int(lr_pos.get_x()), lry = int(lr_pos.get_y());
    int x,y;
    y = lry - 30;
    if(face_left)
        x = lrx - 10;
    else
        x = lrx;
    if(flyer)
        x = lrx - 30;
    return DP(x, y);
}

/*************** Projectile ***************/
Projectile::Projectile(DP pos, string t, string dir, string o)
{
    midend_pos = pos;
    direction = dir;
    type = t;
    owner = o;
    if(direction == "up")
        speed_y = -speed;
    if(direction == "right")
        speed_x = speed;
    if(direction == "left")
        speed_x = -speed;
    if(direction == "down")
        speed_y = speed;

    try
    {
        pic = Gdk::Pixbuf::create_from_resource("/resources/projectile/" + type + "/" + direction + ".png");
    }
    catch(const Gio::ResourceError& ex)
    {
        std::cerr << "ResourceError: " << ex.what() << std::endl;
    }
    catch(const Gdk::PixbufError& ex)
    {
        std::cerr << "PixbufError: " << ex.what() << std::endl;
    }
}

void Projectile::draw(Glib::RefPtr<Gdk::Pixbuf> &disp_buf)
{
    DP ul = get_ul();
    int ulx = int(ul.get_x()) - GameMap::curr_map_x;
    int uly = int(ul.get_y());
    if(ulx <= 0 || ulx > GameFrame::display_width)
        return;
    // cout<<ulx<<' '<<uly<<endl;
    pic -> composite(disp_buf, ulx, uly, pic->get_width() * scale, pic->get_height() * scale,ulx, uly, scale, scale, Gdk::InterpType::INTERP_NEAREST, 255);
}

DP Projectile::get_ul()
{
    return midend_pos;
}

void Projectile::move()
{
    midend_pos.add_x_y(speed_x, speed_y);
    if(speed_x != 0)
        dist += abs(speed_x);
    else
        dist += abs(speed_y);
    if(dist >= 800)
        stop();
}

void Projectile::stop()
{
    speed_x = 0, speed_y = 0;
}

bool Projectile::is_moving()
{
    if(speed_x != 0 || speed_y != 0)
        return true;
    return false;
}
bool Projectile::in_box(Box b)
{
    int curr_x = int(get_head().get_x()), curr_y = int(get_head().get_y());
    // cout<< curr_x << " " << curr_y<<endl;
    // cout<<b.get_x_low()<<" "<<b.get_x_high()<<" "<<b.get_y_low()<<" "<<b.get_y_high()<<endl;
    if(curr_x >= b.get_x_low() - 10 && curr_x <= b.get_x_high() + 10 && curr_y >= b.get_y_low() - 10 && curr_y <= b.get_y_high() + 10 )
        return true;
    return false;
}

// todo 实际写
DP Projectile::get_head()
{

    return midend_pos;
}

int Projectile::get_damage()
{
    return damage;
}
/*************** GameEngine **************/
vector<Projectile> GameEngine::projectiles;
vector<Enemy> GameEngine::enemies;

GameEngine::GameEngine()
{
    // projectiles.push_back(Projectile(DP(100,600), "right"));
    // projectiles.push_back(Projectile(DP(300,300),"bomb", "left"));
    // start_game();
}
void GameEngine::start_game()
{
    end_game();
    player.refresh();

    srand((unsigned)time(NULL));

    for(int i=0;i<100;i++)
    {
        int px = rand() % 9000 + 1000;
        cout<<px<<endl;
        bool flag = true;
        for(int j=0;j<enemies.size();j++)
            if(px - enemies[i].get_gun().get_x()<=10)
            {
                flag = false;
                continue;
            }
        if(flag)
            enemies.push_back(Enemy(DP(px, 500), "zombie"));
    }

    for(int i=0;i<50;i++)
    {
        int px = rand() % 9000 + 1000;
        bool flag = true;
        for(int j=0;j<enemies.size();j++)
            if(abs(px - enemies[i].get_gun().get_x()) <= 40)
            {
                flag = false;
                continue;
            }
        if(flag)
            enemies.push_back(Enemy(DP(px, 200), "ufo"));
    }
    cout<<"++++++++"<<"totaly "<<enemies.size()<<"enemies"<<endl;
}

void GameEngine::end_game()
{
    // 清空所有vector
    for(int i=0;i<projectiles.size();i++)
        projectiles.erase(projectiles.begin()+i);
    for(int i=0;i<enemies.size();i++)
        enemies.erase(enemies.begin() + i);
}
bool GameEngine::judge_end()
{
    // cout<<enemies.size() <<" "<< player.get_lr().get_x()<<endl;
    if(enemies.size() == 0 && player.get_lr().get_x() >= 200)
    {
        cout<<"++++++++++++++++++++++++"<<endl;
        end_by = "win";
        player.set_state("win");
    }
    if(player.is_alive() == false)
    {
        end_by = "lose";
    }
    if(player.expire())
    {
        if(end_by == "lose")
            GameFrame::status = "lose";
        else
            GameFrame::status = "choose";
        end_game();
        player.refresh();
        return true;
    }
    return false;
}

void GameEngine::render(Glib::RefPtr<Gdk::Pixbuf> &disp_buf)
{
    judge_end();
    // 1.绘制地图 注意一定放在最前面，会覆盖
    gamemap.adjust_map(player.get_lr());
    gamemap.draw(disp_buf);

    // 2. 更新玩家位置，绘制玩家
    player.move();
    player.draw(disp_buf);

    // 3. 更新敌人位置，绘制敌人
    for(int i=0;i<enemies.size();i++)
    {
        if(enemies[i].is_alive()) // 活着才干这些事
        {
            enemies[i].move(player.get_lr());
            enemies[i].alert( int( player.get_lr().get_x() ) ) ; // 尝试惊动
            enemies[i].decide_attack(int(player.get_lr().get_x()));
        }
        enemies[i].draw(disp_buf);
    }
    for(int i=0;i<enemies.size();i++)
        if(enemies[i].expire())
            enemies.erase(enemies.begin()+i);

    // 4. 更新子弹位置 ，检测是否击中，绘制子弹，超过距离范围删掉
    for(int i=0;i<projectiles.size();i++)
        projectiles[i].move(), projectiles[i].draw(disp_buf);

    // player的子弹打怪
    for(int i=0;i<projectiles.size();i++)
        for(int j=0;j<enemies.size();j++)
            if(enemies[j].is_alive() && projectiles[i].from_player())
                if(projectiles[i].in_box(enemies[j].get_box()))
                    enemies[j].take_damage(projectiles[i].get_damage()), projectiles[i].stop();

    for(int i=0;i<projectiles.size();i++)
        if(player.is_alive() && !projectiles[i].from_player())
            if(projectiles[i].in_box(player.get_box()))
                player.take_damage(30), projectiles[i].stop();


    for(int i=0;i<projectiles.size();i++)
        if(! projectiles[i].is_moving())
            projectiles.erase(projectiles.begin()+i);
}



/***************** GameFrame **********************/
string GameFrame:: status = "pending"; // pending choose man playing
Point GameFrame::map_ul = Point(0,20);
// todo 前面的几张图有比这个大的
int GameFrame::display_width = 1300;
int GameFrame::display_height = 700;
GameFrame::GameFrame()
{
    Glib::signal_timeout().connect( sigc::mem_fun(*this, &GameFrame::on_timeout), 50 );
    disp_buf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,true,8,GameFrame::display_width, GameFrame::display_height);

}
GameFrame::~GameFrame(){}

// 绘制函数，这个函数调用所有的绘制，刷新在ontimeout
bool GameFrame::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{

    if(status == "playing")
    {
        engine.render(disp_buf);
    }
    else
    {
        show_image();
    }

    Gdk::Cairo::set_source_pixbuf(cr, disp_buf, 0 ,0);
    cr->paint();
    return true;
}
void GameFrame::show_image()
{
    Glib::RefPtr<Gdk::Pixbuf> image;
    try
    {
      image = Gdk::Pixbuf::create_from_resource("/resources/util/"+ status +".png");
    }
    catch(const Gio::ResourceError& ex)
    {
      std::cerr << "ResourceError: " << ex.what() << std::endl;
    }
    catch(const Gdk::PixbufError& ex)
    {
      std::cerr << "PixbufError: " << ex.what() << std::endl;
    }
    int dx=GameFrame::display_width, dy=GameFrame::display_height;
    double sx=double(dx) / double(image->get_width());
    double sy=double(dy) / double(image->get_height());

    image->scale(disp_buf, 0,0,dx,dy,0,0,sx,sy,Gdk::InterpType::INTERP_NEAREST);
    // image->copy_area(0, 0, image->get_width(), image->get_height(), disp_buf, 0, 0);
}
bool GameFrame::on_timeout()
{
    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
        win->invalidate_rect(r, false); //重绘整个widget
    }
    return true;
}
