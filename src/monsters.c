//
// Created by Hector Mejia on 8/23/24.
//

#include "monsters.h"

MonsterType monster_type_from_str(const char *name) {
    if (streq(name, "plant")) {
        return MonsterTypePlant;
    }
    if (streq(name, "fire")) {
        return MonsterTypeFire;
    }
    if (streq(name, "water")) {
        return MonsterTypeWater;
    }

    panic("unknown type \"%s\" provided", name);
    return MonsterTypeNone;
}

MonsterAbility monster_ability_from_str(const char *name) {
    if (streq(name, "scratch")) {
        return MonsterAbilityScratch;
    }
    if (streq(name, "spark")) {
        return MonsterAbilitySpark;
    }
    if (streq(name, "fire")) {
        return MonsterAbilityFire;
    }
    if (streq(name, "battlecry")) {
        return MonsterAbilityBattleCry;
    }
    if (streq(name, "explosion")) {
        return MonsterAbilityExplosion;
    }
    if (streq(name, "annihilate")) {
        return MonsterAbilityAnnihilate;
    }
    if (streq(name, "splash")) {
        return MonsterAbilitySpark;
    }
    if (streq(name, "ice")) {
        return MonsterAbilityIce;
    }
    if (streq(name, "heal")) {
        return MonsterAbilityHeal;
    }
    if (streq(name, "burn")) {
        return MonsterAbilityBurn;
    }

    panic("unknown type \"%s\" provided", name);
    return MonsterAbilityNone;
}

// since there is a finite, not too large number of monsters,
// this if statements are more than enough. If we had 100s, as it would in a real
// game, I would come with a more resilient approach. Would likely have some sort
// of file database I can read into memory, and be editable from the outside.
MonsterID monster_name_from_str(const char *name) {
    if (streq(name, "Plumette")) {
        return MonsterNamePlumette;
    }
    if (streq(name, "Ivieron")) {
        return MonsterNameIvieron;
    }
    if (streq(name, "Pluma")) {
        return MonsterNamePluma;
    }
    if (streq(name, "Sparchu")) {
        return MonsterNameSparchu;
    }
    if (streq(name, "Cindrill")) {
        return MonsterNameCindrill;
    }
    if (streq(name, "Charmadillo")) {
        return MonsterNameCharmadillo;
    }
    if (streq(name, "Finsta")) {
        return MonsterNameFinsta;
    }
    if (streq(name, "Gulfin")) {
        return MonsterNameGulfin;
    }
    if (streq(name, "Finiette")) {
        return MonsterNameFiniette;
    }
    if (streq(name, "Atrox")) {
        return MonsterNameAtrox;
    }
    if (streq(name, "Pouch")) {
        return MonsterNamePouch;
    }
    if (streq(name, "Draem")) {
        return MonsterNameDraem;
    }
    if (streq(name, "Larvea")) {
        return MonsterNameLarvea;
    }
    if (streq(name, "Cleaf")) {
        return MonsterNameCleaf;
    }
    if (streq(name, "Jacana")) {
        return MonsterNameJacana;
    }
    if (streq(name, "Friolera")) {
        return MonsterNameFriolera;
    }

    panic("unknown name \"%s\" provided", name);
    return MonsterNameNone;
}
