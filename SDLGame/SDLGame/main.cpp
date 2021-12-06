#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "enemy.h"
#include "projectile.h"

using namespace std;

float deltaTime = 0.0;					//deltaTime init() - for frame rate independence
int thisTime = 0;
int lastTime = 0;

float playerSpeed = 500.0f;
float yDir;								//movement on the Y axis - up or down
float pos_X = 0, pos_Y = 0;				//control integer creep
SDL_Rect playerPos;						//create rectangles for player position
int playerScore, oldScore;
int playerLives, oldLives;
SDL_Point center;

TTF_Font* font;							//font stuff
SDL_Color colorP1 = {0,0,0,255};
SDL_Surface* scoreSurface, * livesSurface;
SDL_Texture* scoreTexture, * livesTexture;
SDL_Rect scorePos, livesPos;
string tempScore, tempLives;

vector<Enemy> enemyList;				//enemy stuff
vector<Projectile> projectileList;		//projectile stuff

Mix_Chunk* shoot;						//sounds
Mix_Chunk* enemyHit, *playerHit;
Mix_Music* bgm;

void CreateProjectile();				//definitions below main
void UpdateScore(SDL_Renderer*);
void UpdateLives(SDL_Renderer*);

int main(int argc, char* argv[]) {
	 bool quit = false;					//boolean to maintain program loop

	 SDL_Window* window;				//declare a pointer
	 SDL_Renderer* renderer = NULL;		//Create a renderer variable
	 SDL_Init(SDL_INIT_EVERYTHING);		//initialize SDL2

	 window = SDL_CreateWindow(
		  "GOTY",					//window title
		  SDL_WINDOWPOS_UNDEFINED,		//initial x pos
		  SDL_WINDOWPOS_UNDEFINED,		//initial y pos
		  1024,							//width in pixels
		  768,							//height in pixels
		  SDL_WINDOW_OPENGL				//flags
	 );

	 if(window == NULL) {
		  printf("Could not create window: %s\n", SDL_GetError());
		  return 1;
	 }

	 renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	 // BACKGROUND IMAGE -- CREATE
	 SDL_Surface* surface = IMG_Load("./Assets/bkgd.png");		 //create an SDL surface
	 SDL_Texture* bkgd;											 //create bkgd texture
	 bkgd = SDL_CreateTextureFromSurface(renderer, surface);	 //place surface into the texture
	 SDL_FreeSurface(surface);									 //free the surface

	 SDL_Rect bkgdPos;											 //create rectangles for the menu graphics
	 bkgdPos.x = 0;
	 bkgdPos.y = 0;
	 bkgdPos.w = 1024;
	 bkgdPos.h = 768;
	 // BACKGROUND IMAGE -- CREATE END

	 // PLAYER IMAGE -- CREATE
	 surface = IMG_Load("./Assets/player.png");
	 SDL_Texture* player;
	 player = SDL_CreateTextureFromSurface(renderer, surface);
	 SDL_FreeSurface(surface);

	 playerPos.x = 0;
	 playerPos.y = 400;
	 playerPos.w = 103;
	 playerPos.h = 150;
	 // PLAYER IMAGE -- CREATE END

	 SDL_Event event;										//SDL Event to handle input

	 Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);		//initialize audio playback
	 shoot = Mix_LoadWAV("./Assets/throw.wav");				//load shoot sound
	 enemyHit = Mix_LoadWAV("./Assets/enemyHit.wav");		//load enemy hit sound
	 playerHit = Mix_LoadWAV("./Assets/playerHit.wav");		//load player hit sound
	 bgm = Mix_LoadMUS("./Assets/backgroundloop.wav");		//load and loop background music

	 if(!Mix_PlayingMusic())								//play and loop music if not playing
		  Mix_PlayMusic(bgm, -1);

	 TTF_Init();
	 font = TTF_OpenFont("./Assets/Jamaica Aroma.ttf", 20);//initialize font
	 
	 scorePos.x = (bkgdPos.w / 2) - 100;
	 scorePos.y = 10;										//rectangles
	 livesPos.x = (bkgdPos.w / 2) - 100;
	 livesPos.y = 40;

	 UpdateScore(renderer);
	 UpdateLives(renderer);

	 // GAME STATES
	 enum GameState { GAME, WIN, LOSE };

	 GameState gameState = GAME;		//track state
	 bool game, win, lose;				//movement through individual states

	 while(!quit) {

		  switch(gameState) {
			   case GAME: {
					game = true;

					cout << "The Game State is GAME" << endl;

					surface = IMG_Load("./Assets/bkgd.png");
					bkgd = SDL_CreateTextureFromSurface(renderer, surface);
					SDL_FreeSurface(surface);

					projectileList.clear();
					for(int i = 0; i < 10; ++i) {									 //fill projectile list
						 Projectile tempProjectile(renderer, i + 5, i + 5);
						 projectileList.push_back(tempProjectile);
					}

					enemyList.clear();
					for(int i = 0; i < 10; ++i) {									 //fill enemy list
						 Enemy tempEnemy(renderer);
						 enemyList.push_back(tempEnemy);
					}

					oldScore = 0, playerScore = 0;									 //initialize score and lives
					oldLives = 0, playerLives = 5;

					while(game) {
						 thisTime = SDL_GetTicks();
						 deltaTime = (float)(thisTime - lastTime) / 1000;
						 lastTime = thisTime;

						 if(SDL_PollEvent(&event)) {
							  if(event.type == SDL_QUIT) {
								   quit = true;
								   game = false;
								   break;
							  }

							  switch(event.type) {
								   case SDL_KEYUP:
										switch(event.key.keysym.sym) {
											 case SDLK_SPACE:
												  CreateProjectile();
												  break;
											 default:
												  break;
										}
							  }
						 }
						 // PLAYER MOVEMENT -------------------------------------------------
						 const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

						 if(currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W])
							  yDir = -1;
						 else if(currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S])
							  yDir = 1;
						 else
							  yDir = 0;
						 // UPDATE ----------------------------------------------------------


						 pos_Y += (playerSpeed * yDir) * deltaTime;					 //get new position to move player
						 playerPos.y = (int)(pos_Y + 0.5f);							 //move player

						 if(playerPos.y < 0) {										 //keep player on screen
							  playerPos.y = 0;
							  pos_Y = 0;											 //moving up
						 }
						 if(playerPos.y > 768 - playerPos.h) {						 //moving down
							  playerPos.y = 768 - playerPos.h;
							  pos_Y = 768 - playerPos.h;
						 }

						 for(int i = 0; i < projectileList.size(); ++i) {			 //update projectile list
							  if(projectileList[i].active)
								   projectileList[i].update(deltaTime);
						 }

						 for(int i = 0; i < enemyList.size(); ++i)									 //update enemy list
							  enemyList[i].update(deltaTime);

						 for(int i = 0; i < projectileList.size(); ++i) {			 //check for collision with bullet and enemy
							  if(projectileList[i].active) {
								   for(int j = 0; j < enemyList.size(); ++j)
										if(SDL_HasIntersection(&projectileList[i].posRect, &enemyList[j].posRect)) {
											 Mix_PlayChannel(-1, enemyHit, 0);
											 enemyList[j].reset();
											 projectileList[i].reset();

											 playerScore += 10;
											 if(playerScore >= 500) {				 //check if player wins
												  game = false;
												  gameState = WIN;
												  playerScore = 0;
												  playerLives = 5;
											 }
										}
								   if(projectileList[i].posRect.x >= 1050)
										projectileList[i].reset();
							  }
						 }
						 
						 for(int i = 0; i < enemyList.size(); ++i) {				 //check for collision with player and enemy
							  if(SDL_HasIntersection(&playerPos, &enemyList[i].posRect)) {
								   Mix_PlayChannel(-1, playerHit, 0);
								   enemyList[i].reset();

								   --playerLives;
								   if(playerLives <= 0) {							 //check if player loses
										game = false;
										gameState = LOSE;
										playerScore = 0;
										playerLives = 5;
								   }
							  }
						 }

						 if(oldScore != playerScore)								 //update score and lives on screen text
							  UpdateScore(renderer);
						 if(oldLives != playerLives)
							  UpdateLives(renderer);
						 // UPDATE END ------------------------------------------------------
						 // DRAW ************************************************************
						 SDL_RenderClear(renderer);									 //clear old buffer (what was on screen prior)
						 SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);			 //draw bkgd
						 for(int i = 0; i < projectileList.size(); ++i) {			 //draw projectiles
							  if(projectileList[i].active)
								   projectileList[i].draw(renderer);
						 }

						 SDL_RenderCopy(renderer, player, NULL, &playerPos);		 //draw player
						 for(int i = 0; i < enemyList.size(); ++i) {								 //draw enemies
							  enemyList[i].draw(renderer);
						 }

						 SDL_RenderCopy(renderer, scoreTexture, NULL, &scorePos);	 //draw score
						 SDL_RenderCopy(renderer, livesTexture, NULL, &livesPos);	 //draw lives
						 SDL_RenderPresent(renderer);								 //draw new info to the screen
						 // DRAW END ********************************************************
					}//ends game screen
					break;
			   }
			   case WIN:
					win = true;

					cout << "The Game State is WIN" << endl;
					cout << "\"R\" to Replay Game" << endl;
					cout << "\"Q\" to Quit Game" << endl << endl;

					surface = IMG_Load("./Assets/win.png");
					bkgd = SDL_CreateTextureFromSurface(renderer, surface);
					SDL_FreeSurface(surface);

					while(win) {
						 thisTime = SDL_GetTicks();
						 deltaTime = (float)(thisTime - lastTime) / 1000;
						 lastTime = thisTime;

						 if(SDL_PollEvent(&event)) {
							  if(event.type == SDL_QUIT) {
								   quit = true;
								   win = false;
								   break;
							  }

							  switch(event.type) {
								   case SDL_KEYUP:
										switch(event.key.keysym.sym) {
											 case SDLK_r:
												  win = false;
												  gameState = GAME;
												  break;
											 case SDLK_q:
												  win = false;
												  quit = true;
												  break;
											 default:
												  break;
										}
							  }
						 }

						 pos_X += (playerSpeed * 1.5) * deltaTime;				//move player right
						 playerPos.x = (int)(pos_X + 0.5f);

						 SDL_RenderClear(renderer);								//clear old buffer
						 SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);		//draw bkgd
						 SDL_RenderCopy(renderer, player, NULL, &playerPos);
						 SDL_RenderPresent(renderer);							//draw new info to the screen
					}//ends win screen
					playerPos.x = 0;	//reset player's position after running right
					playerPos.y = 400;
					pos_X = 0;
					break;
			   case LOSE:
					lose = true;
					int k = 0;	   //control variable for visually colliding player and hurdle

					cout << "The Game State is LOSE" << endl;
					cout << "\"R\" to Replay Game" << endl;
					cout << "\"Q\" to Quit Game" << endl << endl;

					surface = IMG_Load("./Assets/lose.png");
					bkgd = SDL_CreateTextureFromSurface(renderer, surface);
					SDL_FreeSurface(surface);

					enemyList[0].posRect.x = rand() % 1100 + bkgdPos.h;
					enemyList[0].posRect.y = playerPos.y + 65;
					enemyList[0].pos_X = enemyList[0].posRect.x;
					enemyList[0].pos_Y = enemyList[0].posRect.y;

					while(lose) {
						 thisTime = SDL_GetTicks();
						 deltaTime = (float)(thisTime - lastTime) / 1000;
						 lastTime = thisTime;

						 if(SDL_PollEvent(&event)) {
							  if(event.type == SDL_QUIT) {
								   quit = true;
								   lose = false;
								   break;
							  }

							  switch(event.type) {
								   case SDL_KEYUP:
										switch(event.key.keysym.sym) {
											 case SDLK_r:
												  lose = false;
												  gameState = GAME;
												  break;
											 case SDLK_q:
												  lose = false;
												  quit = true;
												  break;
											 default:
												  break;
										}
							  }
						 }
						 
						 if(!SDL_HasIntersection(&playerPos, &enemyList[0].posRect)) {
							  pos_X += (playerSpeed * 1) * deltaTime;				 //move player right
							  playerPos.x = (int)(pos_X + 0.5f);

							  enemyList[0].pos_X += (playerSpeed * -1.25) * deltaTime;	 //move hurdle left
							  enemyList[0].posRect.x = (int)(enemyList[0].pos_X - 0.5f);
						 }
						 else {
							  if(k < 1000) {										 //makes sure they visually collide
								   pos_X += (playerSpeed * 1) * deltaTime;
								   playerPos.x = (int)(pos_X + 0.5f);

								   enemyList[0].pos_X += (playerSpeed * -1) * deltaTime;
								   enemyList[0].posRect.x = (int)(enemyList[0].pos_X - 0.5f);

								   ++k;
							  }

						 }

						 SDL_RenderClear(renderer);								//clear old buffer
						 SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);		//draw bkgd
						 if(k < 500) {											//draw player and hurdle
							  SDL_RenderCopy(renderer, player, NULL, &playerPos);
							  enemyList[0].draw(renderer);
						 }
						 else {													//draw them differently
							  if(k == 500)
								   Mix_PlayChannel(-1, playerHit, 0);
							  center.x = enemyList[0].posRect.w;
							  center.y = enemyList[0].posRect.h;
							  SDL_RenderCopyEx(renderer, enemyList[0].texture, NULL, &enemyList[0].posRect, 90, &center, SDL_FLIP_NONE);
							  center.x = playerPos.w;
							  center.y = playerPos.h;
							  SDL_RenderCopyEx(renderer, player, NULL, &playerPos, 90, &center, SDL_FLIP_NONE);
						 }
						 
						 SDL_RenderPresent(renderer);							//draw new info to the screen
					}//ends lose screen
					playerPos.x = 0;		 //reset player's position after running right
					playerPos.y = 400;
					pos_X = 0;
					k = 0;					 //reset control variable
					break;
		  }//ends switch for gameState
	 }//ends game loop

	 SDL_DestroyWindow(window);	   //close and destroy the window
	 SDL_Quit();				   //clean up

	 return 0;
}

void CreateProjectile() {
	 for(int i = 0; i < projectileList.size(); ++i)
		  if(!projectileList[i].active) {
			   Mix_PlayChannel(-1, shoot, 0);

			   projectileList[i].active = true;

			   projectileList[i].posRect.y = (pos_Y + (playerPos.h / 2));
			   projectileList[i].posRect.y = (projectileList[i].posRect.y - (projectileList[i].posRect.h / 2));

			   projectileList[i].posRect.x = playerPos.x;

			   projectileList[i].pos_Y = pos_Y;
			   projectileList[i].pos_X = playerPos.x;

			   break;
		  }
}

void UpdateScore(SDL_Renderer* renderer) {
	 tempScore = "Player Score: " + to_string(playerScore);
	 scoreSurface = TTF_RenderText_Solid(font, tempScore.c_str(), colorP1);
	 scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

	 SDL_QueryTexture(scoreTexture, NULL, NULL, &scorePos.w, &scorePos.h);
	 SDL_FreeSurface(scoreSurface);

	 oldScore = playerScore;
}

void UpdateLives(SDL_Renderer* renderer) {
	 tempLives = "Player Lives: " + to_string(playerLives);
	 livesSurface = TTF_RenderText_Solid(font, tempLives.c_str(), colorP1);
	 livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurface);

	 SDL_QueryTexture(livesTexture, NULL, NULL, &livesPos.w, &livesPos.h);
	 SDL_FreeSurface(livesSurface);

	 oldLives = playerLives;
}