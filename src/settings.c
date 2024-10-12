//
// Created by Hector Mejia on 6/1/24.
//

#include "settings.h"

GameSettings settings = {
	.playerSpeed = 250 * 3,
	.charactersSpeed = 250,
	.playerAnimationSpeed = 4,
	.monsterAnimationSpeed= 4,
	.monsterAttackAnimationSpeed = 4,
	.playerNoticedTimerSec = 0.5f,
	.waterAnimationSpeed = 4,
	.coastLineAnimationSpeed = 4,
	.charactersPatrolIntervalSecs = 3,
	.fadeTransitionSpeed =  600.f,
	.monsterBattleRemoveHighlightIntervalSecs = 0.3f,
};
