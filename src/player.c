//
// Created by Hector Mejia on 5/18/24.
//

#include "player.h"
#include "common.h"
#include "assets.h"
#include "character_entity.h"
#include "game.h"
#include "game_data.h"
#include "array/array.h"
#include "raymath.h"
#include "settings.h"

static void player_handle_dialog(Player *p);

Player player_new(const Vector2 position) {
	Player player = {
		.characterComponent = character_new(
			position,
			assets.tileMaps.player,
			CharacterDirectionDown,
			"player"
		),
	};
	player.characterComponent.isPlayer = true; // todo - ugh
	player.characterComponent.speed = settings.playerSpeed;
	return player;
}

void player_free(const Player *p) {
	character_free(&p->characterComponent);
}

void player_input(Player *p) {
	if (IsKeyPressed(KEY_SPACE)) {
		player_handle_dialog(p);
	}

	if (game.player.characterComponent.blocked) {
		return;
	}

	p->characterComponent.velocity = (Vector2){};
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
		p->characterComponent.direction = CharacterDirectionUp;
		p->characterComponent.velocity.y -= 1;
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
		p->characterComponent.direction = CharacterDirectionDown;
		p->characterComponent.velocity.y += 1;
	}
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
		p->characterComponent.direction = CharacterDirectionLeft;
		p->characterComponent.velocity.x -= 1;
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
		p->characterComponent.direction = CharacterDirectionRight;
		p->characterComponent.velocity.x += 1;
	}
	p->characterComponent.velocity = Vector2Normalize(p->characterComponent.velocity);
}

void player_update(Player *p, const f32 deltaTime) {
	const Rectangle oldPlayerFrame = p->characterComponent.frame;
	character_update(&p->characterComponent, deltaTime);

	// collisions
	array_range(game.currentMap->collisionBoxes, i) {
		const Rectangle box = game.currentMap->collisionBoxes[i];
		if (CheckCollisionRecs(p->characterComponent.hitBox, box)) {
			p->characterComponent.frame = oldPlayerFrame;
			break;
		}
	}
	array_range(game.currentMap->mainSprites, i) {
		const Entity entity = game.currentMap->mainSprites[i].entity;
		if (!entity.collideable) { continue; }

		if (CheckCollisionRecs(p->characterComponent.hitBox, entity.hitBox)) {
			game.player.characterComponent.frame = oldPlayerFrame;
			break;
		}
	}
	array_range(game.currentMap->overWorldCharacters, i) {
		const Rectangle box = game.currentMap->overWorldCharacters[i].hitBox;

		if (CheckCollisionRecs(p->characterComponent.hitBox, box)) {
			game.player.characterComponent.frame = oldPlayerFrame;
			break;
		}
	}
}

static void player_handle_dialog(Player *p) {
	if (!game.dialogBubble.visible) {
		array_range(game.currentMap->overWorldCharacters, i) {
			Character *character = &game.currentMap->overWorldCharacters[i];

			if (check_character_connection(&p->characterComponent, character, 100.f)) {
				printf("dialog with %s\n", character->id);
				// block player input
				player_block(p);
				// entities face each other
				character_change_direction(character, character_get_center(&p->characterComponent));
				// create dialog
				character_create_dialog(character);
			}
		}
		return;
	}

	game.dialogBubble.index++;
	const CharacterData *data = game_data_for_character_id(game.dialogBubble.characterID);
	const i32 maxDialogs = data->defeated ? MAX_DEFEATED_DIALOG_ENTRIES : MAX_REGULAR_DIALOG_ENTRIES;
	const bool overflows = game.dialogBubble.index >= maxDialogs;
	bool isEmptyEntry = false;
	if (!overflows) {
		isEmptyEntry = data->defeated ?
			streq(data->dialog.defeated[game.dialogBubble.index], "") :
			streq(data->dialog.regular[game.dialogBubble.index], "");
	}

	if (isEmptyEntry || overflows) {
		player_unblock(p);
		game.dialogBubble.visible = false;
		game.dialogBubble.index = 0;
		printfln("ended dialog with %s", data->id);
		if (streq(data->id, "Nurse")) {
			for (i32 i = 0; i < MAX_PARTY_MONSTERS_LEN; i++) {
				if (game.playerMonsters[i].id == MonsterIDNone) { continue; }
				game.playerMonsters[i].health = (i32)game.playerMonsters[i].stats.maxHealth;
				game.playerMonsters[i].energy = (i32)game.playerMonsters[i].stats.maxEnergy;
			}
		} else if (!data->defeated) {

			const usize monstersLen = comptime_array_len(data->monsters);
			Monster monsters[monstersLen] = {};
			for (usize i = 0; i < comptime_array_len(data->monsters); i++) {
				if (streq(data->monsters[i].name, "")) { continue; }
				monsters[i] = monster_new(monster_name_from_str(data->monsters[i].name), data->monsters[i].level);
			}
			BattleStageBackground bg = BattleStageBackgroundCount;
			if (streq(data->biome, "sand")) {
				bg = BattleStageBackgroundSand;
			} else if (streq(data->biome, "ice")) {
				bg = BattleStageBackgroundIce;
			} else if (streq(data->biome, "forest")) {
				bg = BattleStageBackgroundForest;
			} else {
				panic("unknown biome %s", data->biome);
			}

			// ok to pass here a stack allocated array since it will be copied
			// todo(hector) - we dont have a way to set the character to defeated.
			//  this may be okay since it lets us re-battle them for the demo?
			game_start_battle(BattleTypeTrainer, bg, monsters, monstersLen);
		}
	}
}


void player_draw(const Player *p) {
	// ok because the characters and the player are the same exact structure.
	character_draw(&p->characterComponent);
	if (p->noticed) {
		const f32 padding = 10;
		const Vector2 exclamationPos = {
			.x = p->characterComponent.frame.x + padding + padding,
			.y = p->characterComponent.frame.y - assets.exclamationMarkTexture.height - padding,
		};
		DrawTextureV(assets.exclamationMarkTexture, exclamationPos, WHITE);
	}
}

Vector2 player_get_center(const Player *p) {
	// ok because the characters and the player are the same exact structure.
	return character_get_center(&p->characterComponent);
}

void player_block(Player *p) {
	p->characterComponent.blocked = true;
	p->characterComponent.velocity = (Vector2){};
}

void player_unblock(Player *p) {
	p->characterComponent.blocked = false;
}

void player_set_noticed(Player *p) {
	p->noticed = true;
	timer_start(&p->noticedTimer, settings.playerNoticedTimerSec);
}

void player_unset_noticed(Player *p) {
	p->noticed = false;
	timer_stop(&p->noticedTimer);
}
