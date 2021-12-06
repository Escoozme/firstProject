#include "projectile.h"

void Projectile::update(float deltaTime) {
	 if(active) {
		  pos_X += (speed * xDir) * deltaTime;

		  posRect.x = (int)(pos_X + 0.5f);

		  if(posRect.x < (0 - posRect.w)) {
			   posRect.y = -1000;
			   pos_Y = posRect.y;
			   active = false;
		  }
	 }
}

void Projectile::draw(SDL_Renderer* renderer) {
	 SDL_RenderCopy(renderer, texture, NULL, &posRect);
}

void Projectile::reset() {
	 active = false;
	 
	 posRect.x = -1000;
	 posRect.y = -1000;

	 pos_X = posRect.x;
	 pos_Y = posRect.y;
}

//Constructor
Projectile::Projectile(SDL_Renderer* renderer, float x, float y) {
	 active = false;

	 speed = 800.0;

	 SDL_Surface* surface = IMG_Load("./Assets/baton.png");

	 texture = SDL_CreateTextureFromSurface(renderer, surface);

	 if(texture == NULL)
		  printf("Unable to load image %s! SDL_image Error: %s\n", "./Assets/baton.png", IMG_GetError());

	 SDL_FreeSurface(surface);

	 posRect.w = 50;
	 posRect.h = 37;
	 posRect.x = x;
	 posRect.y = y - (posRect.h / 2);

	 pos_X = x;
	 pos_Y = y;

	 xDir = 1;
	 yDir = 0;
}

Projectile::~Projectile() {
	 //SDL_DestroyTexture(texture);
}
