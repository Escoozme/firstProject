#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

using namespace std;

class Projectile {
public:
	 void update(float);
	 void draw(SDL_Renderer*);
	 void reset();

	 Projectile(SDL_Renderer*, float, float);
	 ~Projectile();

	 bool active;
	 SDL_Texture* texture;
	 SDL_Rect posRect;
	 float xDir, yDir;
	 float speed;
	 float pos_X, pos_Y;
};
