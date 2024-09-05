//
// Created by Hector Mejia on 8/10/24.
//

#include "game_data.h"

#include <errno.h>
#include <cjson/cJSON.h>
#include <stdio.h>

#include "common.h"
#include "game.h"
#include "array/array.h"
#include "memory/memory.h"

static void *json_malloc(usize sz) {
    return mallocate(sz, MemoryTagJSON);
}

static void json_free(void *ptr) {
    return mfree(ptr, 0, MemoryTagJSON);
}

inline static cJSON *get_string(const cJSON *json, const char *fieldName) {
    cJSON *field = cJSON_GetObjectItemCaseSensitive(json, fieldName);
    panicIf(!cJSON_IsString(field) && !cJSON_IsNull(field), "invalid %s field", fieldName);
    return field;
}

inline static cJSON *get_object(const cJSON *json, const char *fieldName) {
    cJSON *field = cJSON_GetObjectItemCaseSensitive(json, fieldName);
    panicIf(!cJSON_IsObject(field), "invalid %s field", fieldName);
    return field;
}

inline static cJSON *get_array(const cJSON *json, const char *fieldName) {
    cJSON *field = cJSON_GetObjectItemCaseSensitive(json, fieldName);
    panicIf(!cJSON_IsArray(field), "invalid %s field", fieldName);
    return field;
}

inline static cJSON *get_bool(const cJSON *json, const char *fieldName) {
    cJSON *field = cJSON_GetObjectItemCaseSensitive(json, fieldName);
    panicIf(!cJSON_IsBool(field), "invalid %s field", fieldName);
    return field;
}

inline static cJSON *get_number(const cJSON *json, const char *fieldName) {
    cJSON *field = cJSON_GetObjectItemCaseSensitive(json, fieldName);
    panicIf(!cJSON_IsNumber(field), "invalid %s field", fieldName);
    return field;
}

static cJSON *load_json_for_data(const char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (fseek(file, 0, SEEK_END) != 0) {
        panic("failed to seek file to end for length");
    }
    const off_t fileLength = ftell(file);
    panicIf(fileLength == -1L, "failed to get file length: %s", strerror(errno));
    panicIf(fileLength > GiB, "file is too large for reading");

    if (fseek(file, 0, SEEK_SET) != 0) {
        panic("failed to seek file to beginning for reading");
    }
    char *fileData = mallocate(fileLength + 1, MemoryTagFile);
    panicIfNil(fileData);

    const usize bytesRead = fread(fileData, 1, fileLength, file);
    panicIf(bytesRead != (usize) fileLength, "length of data coming from file does not match expected");

    fclose(file);

    cJSON *data = cJSON_Parse(fileData);
    if (data == nil) {
        const char *error = cJSON_GetErrorPtr();
        if (error != NULL) {
            panic("failed to parse character data: %s\n", error);
        }
    }
    // allocating and free back to back like this is very inefficient, but
    // for a toy game is fine. In a more serious project, I would probably find
    // a nicer way to reuse this buffer, or use an arena instead.
    mfree(fileData, fileLength + 1, MemoryTagFile);
    fileData = nil;

    return data;
}

static void init_character_data() {
    if (game.data.characterData != nil) {
        array_free(game.data.characterData);
    }
    cJSON *json = load_json_for_data("./data/game/character_data.json");

    const cJSON *characterData = nil;
    cJSON_ArrayForEach(characterData, json) {
        const cJSON *biome = cJSON_GetObjectItemCaseSensitive(characterData, "biome");
        panicIf(!cJSON_IsString(biome) && !cJSON_IsNull(biome), "invalid biome field");

        const cJSON *defeated = cJSON_GetObjectItemCaseSensitive(characterData, "defeated");
        panicIf(!cJSON_IsBool(defeated), "invalid defeated field");

        const cJSON *look_around = cJSON_GetObjectItemCaseSensitive(characterData, "look_around");
        panicIf(!cJSON_IsBool(look_around), "invalid look_around field");

        const cJSON *directions = cJSON_GetObjectItemCaseSensitive(characterData, "directions");
        panicIf(!cJSON_IsArray(directions), "invalid directions field");

        const cJSON *dialog = cJSON_GetObjectItemCaseSensitive(characterData, "dialog");
        panicIf(!cJSON_IsObject(dialog), "invalid dialog field");

        const cJSON *monsters = cJSON_GetObjectItemCaseSensitive(characterData, "monsters");
        panicIf(monsters != nil && !cJSON_IsObject(monsters), "invalid monsters field");

        CharacterData data = {
            .lookAround = look_around->type == cJSON_True,
            .defeated = defeated->type == cJSON_True,
        };

        if (streq(characterData->string, "Nurse")) {
            const cJSON *direction = cJSON_GetObjectItemCaseSensitive(characterData, "direction");
            panicIf(!cJSON_IsString(direction), "invalid direction field");

            data.direction = character_direction_from_str(direction->valuestring);
        } else {
            strncpy(data.biome, biome->valuestring, sizeof(data.biome) / sizeof(data.biome[0]));
        }
        strncpy(data.id, characterData->string, sizeof(data.id) / sizeof(data.id[0]));

        // load monsters
        u8 count = 0;
        const cJSON *monster = nil;
        cJSON_ArrayForEach(monster, monsters) {
            panicIf(
                !cJSON_IsArray(monster),
                "invalid monsters field for character %s",
                characterData->string
            );

            const cJSON *monsterName = monster->child;
            const cJSON *monsterLevel = monster->child->next;
            panicIf(count >= MAX_MONSTER_PER_CHARACTER, "found more monsters than allowed");
            panicIf(
                !cJSON_IsString(monsterName),
                "invalid monster name for %s for character %s",
                monster->string,
                characterData->string
            );
            panicIf(
                !cJSON_IsNumber(monsterLevel),
                "invalid level for monster %s for character %s",
                monster->string,
                characterData->string
            );
            data.monsters[count].level = (u8) monsterLevel->valuedouble;
            const usize nameMaxLen = sizeof(data.monsters[count].name) / sizeof(data.monsters[count].name[0]);
            strncpy(
                data.monsters[count].name,
                monsterName->valuestring,
                nameMaxLen
            );
            count++;
        }

        // load dialog
        const cJSON *dialogDefault = cJSON_GetObjectItemCaseSensitive(dialog, "default");
        const cJSON *dialogDefeated = cJSON_GetObjectItemCaseSensitive(dialog, "defeated");
        panicIf(
            !cJSON_IsArray(dialogDefault) || (!cJSON_IsArray(dialogDefeated) && !cJSON_IsNull(dialogDefeated)),
            "invalid default or defeated fields"
        );

        count = 0;
        const cJSON *entry = nil;
        cJSON_ArrayForEach(entry, dialogDefault) {
            panicIf(count >= MAX_REGULAR_DIALOG_ENTRIES, "found more default dialogs than allowed");
            const usize dialogMaxLen = sizeof(data.dialog.regular[count]) / sizeof(data.dialog.regular[count][0]);
            strncpy(
                data.dialog.regular[count],
                entry->valuestring,
                dialogMaxLen
            );
            count++;
        }

        count = 0;
        entry = nil;
        cJSON_ArrayForEach(entry, dialogDefeated) {
            panicIf(count >= MAX_DEFEATED_DIALOG_ENTRIES, "found more defeated dialogs than allowed");
            const usize dialogMaxLen = sizeof(data.dialog.defeated[count]) / sizeof(data.dialog.defeated[count][0]);
            strncpy(
                data.dialog.defeated[count],
                entry->valuestring,
                dialogMaxLen
            );
            count++;
        }

        // directions
        count = 0;
        entry = nil;
        cJSON_ArrayForEach(entry, directions) {
            panicIf(count >= MAX_DIRECTIONS_ENTRIES, "found more directions than allowed");
            data.directions[count] = character_direction_from_str(entry->valuestring);
            count++;
        }
        data.directionsLen = count;
        array_push(game.data.characterData, data);
    }
    cJSON_free(json);
}

static void init_monster_data() {
    if (game.data.monsterData != nil) {
        array_free(game.data.monsterData);
    }

    cJSON *json = load_json_for_data("./data/game/monster_data.json");
    const cJSON *monsterData = nil;
    cJSON_ArrayForEach(monsterData, json) {
        const cJSON *statsJSON = get_object(monsterData, "stats");
        const cJSON *abilitiesJSON = get_object(monsterData, "abilities");

        MonsterData data = {
            .id = monster_name_from_str(monsterData->string),
            .element = monster_type_from_str(get_string(statsJSON, "element")->valuestring),
            .stats = {
                .maxHealth = (f32) get_number(statsJSON, "max_health")->valuedouble,
                .maxEnergy = (f32) get_number(statsJSON, "max_energy")->valuedouble,
                .attack = (f32) get_number(statsJSON, "attack")->valuedouble,
                .defense = (f32) get_number(statsJSON, "defense")->valuedouble,
                .recovery = (f32) get_number(statsJSON, "recovery")->valuedouble,
                .speed = (f32) get_number(statsJSON, "speed")->valuedouble,
            },
        };
        strncpy(data.name, monsterData->string, MAX_MONSTER_NAME_LEN);

        const cJSON *ability = nil;
        data.abilitiesLen = 0;
        cJSON_ArrayForEach(ability, abilitiesJSON) {
            panicIf(data.abilitiesLen >= MAX_MONSTER_ABILITIES_LEN, "found abilities than the max allowed");
            data.abilities[data.abilitiesLen].level = strtoul(ability->string, nil, 10);
            data.abilities[data.abilitiesLen].ability = monster_ability_from_str(ability->valuestring);
            data.abilitiesLen++;
        }

        const cJSON *evolveJSON = cJSON_GetObjectItemCaseSensitive(monsterData, "evolve");
        if (!cJSON_IsNull(evolveJSON)) {
            // evolve[0]
            data.evolution.monster = monster_name_from_str(evolveJSON->child->valuestring);
            // evolve[1]
            data.evolution.level = (u8) evolveJSON->child->next->valueint;
        }

        array_push(game.data.monsterData, data);
    }
    cJSON_free(json);
}

void game_data_init() {
    cJSON_Hooks hook = {
        .malloc_fn = json_malloc,
        .free_fn = json_free,
    };
    cJSON_InitHooks(&hook);

    // cJSON *monsterData = load_json_for_data("./data/game/monster_data.json");
    // cJSON *attackData = load_json_for_data("./data/game/attack_data.json");

    init_character_data();
    init_monster_data();
}

void game_data_free() {
    array_free(game.data.characterData);
    array_free(game.data.monsterData);
}

CharacterData *game_data_for_character_id(const char *characterID) {
    array_range(game.data.characterData, i) {
        const CharacterData data = game.data.characterData[i];
        if ((strncmp(data.id, characterID, MAX_DIRECTIONS_ENTRIES) == 0)) {
            return &game.data.characterData[i];
        }
    }
    panic("unknown character ID %s\n", characterID);
}

MonsterData *game_data_for_monster_id(MonsterID monsterID) {
    array_range(game.data.monsterData, i) {
        const MonsterData data = game.data.monsterData[i];
        if (data.id == monsterID) {
            return &game.data.monsterData[i];
        }
    }
    panic("unknown monster ID %d\n", monsterID);
}
