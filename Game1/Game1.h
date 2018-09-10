// Game1.h
// (c) 2018 FabLab Kannai
//
#ifndef GAME1_H
#define GAME1_H
#include <Arduino.h>
#include "VFD.h"

class Bullet {
  public:
    static const uint8_t       VAL_NULL = 0xff;
    
    Bullet();
    Bullet(uint8_t val, unsigned long interval);
    void          init(uint8_t val, unsigned long interval);

    uint8_t       val();
    uint8_t       x();
    unsigned long time();
    unsigned long interval();

    boolean       move();

  private:
    uint8_t       _val;
    uint8_t       _x;
    unsigned long _time;
    unsigned long _interval;
};

class Player {
  public:
    Player();
    void init(uint8_t val);

    uint8_t val();
    Bullet bullet();
    
    void    up();
    void    down();
    void    shoot(unsigned long bullet_interval);
    boolean bullet_move();
    uint8_t bullet_val();
    uint8_t bullet_x();
    void    bullet_delete();
    
  private:
    uint8_t _val = 0;
    Bullet _bullet;
};

class Enemy {
  public:
    Enemy();
    Enemy(unsigned long interval);
    void          init(unsigned long interval);

    uint8_t       val(uint8_t i);
    uint8_t       x();
    uint8_t       size();
    unsigned long time();
    unsigned long interval();
    void          set_interval(unsigned long interval);

    boolean       generate();
    void          hit();

  private:
    uint8_t       _val[6];
    uint8_t       _x;
    uint8_t       _size   = 0;
    unsigned long _time;
    unsigned long _interval;
};

#endif
