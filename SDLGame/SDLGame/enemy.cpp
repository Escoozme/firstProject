#include "enemy.h"

void Enemy::update(float deltaTime) {
	 pos_X += (speed * xDir) * deltaTime;

	 posRect.x = (int)(pos_X - 0.5f);

	 if(posRect.x < (0 - posRect.w))
		  reset();
}

void Enemy::draw(SDL_Renderer* renderer) {
	 SDL_RenderCopy(renderer, texture, NULL, &posRect);
}

void Enemy::reset() {
	 speed = rand() % (5) + 1;
	 speed *= 100;

	 posRect.x = 1100 + posRect.w;
	 posRect.y = rand() % (768 - posRect.h) + 1;

	 pos_X = posRect.x;
	 pos_Y = posRect.y;
}

//Constructor
Enemy::Enemy(SDL_Renderer* renderer) {
	 SDL_Surface* surface = IMG_Load("./Assets/hurdle.png");

	 texture = SDL_CreateTextureFromSurface(renderer, surface);

	 SDL_FreeSurface(surface);

	 posRect.w = 100;
	 posRect.h = 100;

	 reset();

	 xDir = -1;
	 yDir = 0;
}

Enemy::~Enemy() {
	 //SDL_DestroyTexture(texture);
}
