#ifndef __CANNON_SCENES_H
#define __CANNON_SCENES_H

class Game;

// A mass of balls, boxes and strips raining down without end
void initDownpourScene(Game *game);

// The cannon against a fortress out of view, with a camera that follows the shots
void initFortressScene(Game *game);

#endif
