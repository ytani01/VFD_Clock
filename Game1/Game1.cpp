// Player.cpp
// (c) 2018 FabLab Kannai
//
#include "Game1.h"

//===========================================
// Constractor
Bullet::Bullet() {
  Bullet::init(VAL_NULL, 0);
}
Bullet::Bullet(uint8_t val, unsigned long interval) {
  Bullet::init(val, interval);
}

// Public methods
void Bullet::init(uint8_t val, unsigned long interval) {
  _val = val;
  _x = 0;
  _time = millis();
  _interval = interval;
  //Serial.println("Bullet: val=" + String(_val) + ", x=" + String(_x) + ", time=" + String(_time) + ", interval=" + String(_interval)); 
}

uint8_t Bullet::val() {
  return _val;
}
uint8_t Bullet::x() {
  return _x;
}
unsigned long Bullet::time() {
  return _time;
}
unsigned long Bullet::interval() {
  return _interval;
}

boolean Bullet::move() {
  unsigned long cur_time = millis();

  if ( _val == VAL_NULL ) {
    return false;
  }
  if ( cur_time - _time < _interval ) {
    return false;
  }

  _time = cur_time;
  _x++;
  if ( _x >= 6 ) {
    _val = VAL_NULL;
  }
  return true;
}

//===========================================
// Constractor
Player::Player() {
  Player::init(0);
}

// Public methods
void Player::init(uint8_t val) {
  _val = val;
  _bullet = Bullet(Bullet::VAL_NULL, 0);
}

uint8_t Player::val() {
  return _val;
}
Bullet Player::bullet() {
  return _bullet;
}

void Player::up() {
  _val = ( _val + 1 ) % 10;
}
void Player::down() {
  if ( _val == 0 ) {
    _val = 9;
  } else {
    _val--;
  }
}

void Player::shoot(unsigned long bullet_interval) {
  if ( _bullet.val() != Bullet::VAL_NULL ) {
    return;
  }
  _bullet.init(_val, bullet_interval);
}

boolean Player::bullet_move() {
  return _bullet.move();
}
uint8_t Player::bullet_val() {
  return _bullet.val();
}

void Player::bullet_delete() {
  if ( _bullet.val() == Bullet::VAL_NULL ) {
    return;
  }
  _bullet.init(Bullet::VAL_NULL, 0);
}

uint8_t Player::bullet_x() {
  return _bullet.x();
}

//===========================================
// Constractor
Enemy::Enemy() {
  Enemy::init(0);
}
Enemy::Enemy(unsigned long interval) {
  Enemy::init(interval);
}

// Public methods
void Enemy::init(unsigned long interval) {
  _interval = interval;
  _time = 0;
  _size = 0;
}

uint8_t Enemy::val(uint8_t i) {
  return _val[i];
}
uint8_t Enemy::x() {
  return (6 - _size);
}
uint8_t Enemy::size() {
  return _size;
}
unsigned long Enemy::time() {
  return _time;
}
unsigned long Enemy::interval() {
  return _interval;
}
void Enemy::set_interval(unsigned long interval) {
  _interval = interval;
}

boolean Enemy::generate() {
  unsigned long cur_time = millis();

  if ( cur_time - _time < _interval ) {
    return false;
  }
  if ( _size >= 6 ) {
    return false;
  }

  _val[_size] = random(10);
  _size++;
  _time = cur_time;
  return true;
}

void Enemy::hit() {
  for (int i=1; i < _size; i++) {
    _val[i-1] = _val[i];
  }
  _size--;
}
