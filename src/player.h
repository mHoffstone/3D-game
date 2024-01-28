#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "rigidbody.h"

#include "engine/gameengine.h"

class Player : public Entity{
public:
	Player(GameEngine* gameEngine);
	virtual ~Player();

	void tryInteract();
	void processInput(float dt, GameEngine* gameEngine);
	virtual void tick(float dt) override;

	std::shared_ptr<Player> player;

	ItemStack handItem;

private:
	std::shared_ptr<RigidCube> heldCube = nullptr;

};

#endif // PLAYER_H
