#ifndef __BOIDS_SCENE_H
#define __BOIDS_SCENE_H

class Game;

// A flock of soft body balls swimming around rocks, hunted by bigger ones
void initBoidsScene(Game *game);

// The same flock seen at an angle, like a strategy game's map, rather than from straight above
void initIsometricBoidsScene(Game *game);

#endif
