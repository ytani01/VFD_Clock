// Game1.h
// (c) 2018 FabLab Kannai
//
#ifndef GAME1_H
#define GAME1_H
#include <Arduino.h>
#include "VFD.h"

typedef uint8_t	mode_t;

//=====================================================================
class Bullet {
  public:
    static const uint8_t       VAL_NULL = 0xff;
    
    Bullet();

    void		init(uint8_t val, unsigned long interval);

    uint8_t		val();
    uint8_t		x();
    unsigned long	time();
    unsigned long	interval();

    boolean       	move();

    void		print(String prefix) const;

  private:
    uint8_t		_val;
    uint8_t		_x;
    unsigned long	_time;
    unsigned long	_interval;
};

class Player {
  public:
    Player();
    void init(uint8_t val);

    uint8_t	val();
    Bullet 	*bullet();
    
    void	up();
    void	down();
    void	shoot(unsigned long bullet_interval);
    void	bullet_delete();
    
  private:
    uint8_t	_val = 0;
    Bullet	_bullet;
};

//=====================================================================
class Enemy {
  public:
    Enemy();
    Enemy(unsigned long interval);
    
    void		init(unsigned long interval);

    uint8_t		val(size_t i);
    uint8_t		x();
    uint8_t		size();
    unsigned long	time();
    unsigned long	interval();
    void		set_interval(unsigned long interval);

    boolean		generate();
    void		hit();

    void		print(String prefix);

  private:
    uint8_t		_val[6];
    uint8_t		_x;
    uint8_t		_size   = 0;
    unsigned long	_time;
    unsigned long	_interval;
};

//=====================================================================
class Game1 {
 public:
  static const unsigned long	ENEMY_INTERVAL	= 3000; // msec
  static const unsigned long	BULLET_INTERVAL	=  250; // msec

  static const mode_t	MODE_PLAY	= 0x01;
  static const mode_t	MODE_END	= 0x02;

  Game1();

  void init(VFD *vfd);

  void loop();
  
  void displayGame();
  void displayScore();
  void display();

  mode_t	mode();
  unsigned long	score();
  Player	*p1();
  Enemy		*e1();
  
  void		set_mode(mode_t mode);
  
 private:
  VFD 		*_vfd;
  Player	_p1;
  Enemy		_e1;
  
  mode_t	_mode	= MODE_PLAY;

  unsigned long	_score	= 0;
};
#endif
