#include <lib.hpp>
#include <nn.hpp>
#include <cstring>
#include <string>

#include "func_ptrs.hpp"
#include "sead/container/seadBuffer.h"
#include "sead/prim/seadSafeString.hpp"

#define LOG(...)                                                    \
  {                                                                 \
      int length = snprintf(buffer, sizeof(buffer), __VA_ARGS__);   \
      svcOutputDebugString(buffer, length);                         \
  }

/* 
Structures
All structures here. Help.
*/

// dmgclass/dmgconstructor
struct dmgclass {
    long long AtkKind;  //0x00 - The "attack" used. Is different, and consistent, based on the atk/skill used
                        // See enums for documentation
    float FinalDmgMod;  //0x08 - The final damage modifier applied to the damage
    long field_C;       //0x0C - Always 0
    float field_14;     //0x14 - Always 1.0
    float BaseMod;      //0x18 - Honestly dunno, end result is always 0.0
    char field_1C;      //0x1C - This is a byte, output is wrong atm
    bool SomeBool;      //0x1D - Constructed as true if the skill is magic based, false if not. 
                        //       Always set to true in the end result. If this is false in the end result, damage will always be 255
    char field_1E;      //0x1E - This is a byte, output is wrong atm
    char field_1F;      //0x1F - This is a byte, output is wrong atm
    short field_20;     //0x20 - Mostly consistent, some overlap/change
                        //      -1 - ???
                        //      0 - Default
                        //      3 - Lend a Hand (Pitch In)
                        //      16 - Slack Off, Spicy Feast, Lend a Hand, Pincer pt 1, Deominc Whisper, Punishing Pitchfork
                        //      20 - Show Off
                        //      23 - Lend a Hand
                        //      48 - Pincer pt 3
                        //      71 - Pincer pt 2
    short field_22;     //0x22 - Always output as 0 or -1
};

/*
Enums
*/

/*
field 0x00 of dmgclass (AtkUsed)
These are the values outputted by the console
when 
*/
enum AttackKind: u64 {
    ATTACK_KIND_FIRE = 24,
    ATTACK_KIND_MEGA_CURE_2 = 36,
    ATTACK_KIND_EXPLOSION = 39,
    ATTACK_KIND_DEMONIC_WHISPER = 50,
    ATTACK_KIND_UNSTABLE_FORMULA = 59,
    ATTACK_KIND_ENEMY_BASIC_ATK = 63,
    ATTACK_KIND_PINCER_FRONT = 71,
    ATTACK_KIND_BASIC_ATTACK = 72,
    ATTACK_KIND_DOUBLE_SCRATCH = 75,
    ATTACK_KIND_MEGA_CURE_1_3 = 79,
    ATTACK_KIND_CURE_EXE = 89,
    ATTACK_KIND_PUNISHING_PITCHFORK = 104,
    ATTACK_KIND_GLITCH = 109
};

/*
Skill IDs 
*/

enum SkillEnum: u64 {
    SKILL_FIGHTER_DOUBLE = 0,
    SKILL_FIGHTER_TWICE = 1,
    SKILL_FIGHTER_SPIN = 2,
    SKILL_FIGHTER_GUARD = 3,
    SKILL_FIGHTER_EYE_SLASH = 4,
    SKILL_FIGHTER_SPIN_2 = 5,
    SKILL_FIGHTER_REVIVE_SLAP = 6,
    SKILL_FIGHTER_SLAP = 7,
    SKILL_FIGHTER_09= 8,
    SKILL_FIGHTER_10= 9,
    SKILL_FIGHTER_11= 10,
    SKILL_FIGHTER_12= 11,
    SKILL_WIZARD_FIRE = 12,
    SKILL_WIZARD_FIRE_2 = 13,
    SKILL_WIZARD_FLAME_TOWER = 14,
    SKILL_WIZARD_LIGHTNING = 15,
    SKILL_WIZARD_LIGHTNING_2 = 16,
    SKILL_WIZARD_LIGHTNING_3 = 17,
    SKILL_WIZARD_EXPLOSION = 18,
    SKILL_WIZARD_EXPLOSION_2 = 19,
    SKILL_WIZARD_EXPLOSION_3 = 20,
    SKILL_WIZARD_BARRIER = 21,
    SKILL_WIZARD_SLEEP = 22,
    SKILL_WIZARD_BIG_WEAPON = 23,
    SKILL_PRIEST_CURE = 24,
    SKILL_PRIEST_CURE_2 = 25,
    SKILL_PRIEST_CURE_3 = 26,
    SKILL_PRIEST_CURE_ALL = 27,
    SKILL_PRIEST_CURE_ALL_2 = 28,
    SKILL_PRIEST_CALM = 29,
    SKILL_PRIEST_KARMA = 30,
    SKILL_PRIEST_RESURRECT = 31,
    SKILL_PRIEST_RESURRECT_2 = 32,
    SKILL_PRIEST_HOLY = 33,
    SKILL_PRIEST_11= 34,
    SKILL_PRIEST_12= 35,
    SKILL_THIEF_DAGGER = 36,
    SKILL_THIEF_DAGGER_2 = 37,
    SKILL_THIEF_ROCK = 38,
    SKILL_THIEF_TRAP = 39,
    SKILL_THIEF_LARGE_TRAP = 40,
    SKILL_THIEF_CREEP = 41,
    SKILL_THIEF_STEAL_ITEM = 42,
    SKILL_THIEF_HIGH_JUMP = 43,
    SKILL_THIEF_09= 44,
    SKILL_THIEF_10= 45,
    SKILL_THIEF_11= 46,
    SKILL_THIEF_12= 47,
    SKILL_IDOL_ENCORE = 48,
    SKILL_IDOL_FUN_SERVICE = 49,
    SKILL_IDOL_LOVE_CALL = 50,
    SKILL_IDOL_CONCERT = 51,
    SKILL_IDOL_HOWLING = 52,
    SKILL_IDOL_HOWLING_2 = 53,
    SKILL_IDOL_DANCE = 54,
    SKILL_IDOL_ANGEL_SONG = 55,
    SKILL_IDOL_LOVE_AND_PEACE = 56,
    SKILL_IDOL_10= 57,
    SKILL_IDOL_11= 58,
    SKILL_IDOL_12= 59,
    SKILL_VAMPIRE_EAT = 60,
    SKILL_VAMPIRE_BAT_ROSE = 61,
    SKILL_VAMPIRE_ZOMBIFY = 62,
    SKILL_VAMPIRE_BREATH = 63,
    SKILL_VAMPIRE_BREATH2 = 64,
    SKILL_VAMPIRE_BREATH3 = 65,
    SKILL_VAMPIRE_CURSE = 66,
    SKILL_VAMPIRE_REVIVE = 67,
    SKILL_VAMPIRE_09= 68,
    SKILL_VAMPIRE_10= 69,
    SKILL_VAMPIRE_11= 70,
    SKILL_VAMPIRE_12= 71,
    SKILL_COOK_COOKING = 72,
    SKILL_COOK_COOKING2 = 73,
    SKILL_COOK_COOKING3 = 74,
    SKILL_COOK_HEATING = 75,
    SKILL_COOK_SPICY = 76,
    SKILL_COOK_SPICY_ALL = 77,
    SKILL_COOK_BANQUET = 78,
    SKILL_COOK_COOK_MONSTER = 79,
    SKILL_COOK_BAKED_BANANA = 80,
    SKILL_COOK_10= 81,
    SKILL_COOK_11= 82,
    SKILL_COOK_12= 83,
    SKILL_TANK_REPAIR = 84,
    SKILL_TANK_WILD_SHOT = 85,
    SKILL_TANK_HUMAN_CANNON = 86,
    SKILL_TANK_BEAM = 87,
    SKILL_TANK_BEAM2 = 88,
    SKILL_TANK_HEAT_SHOT = 89,
    SKILL_TANK_DEFENSE = 90,
    SKILL_TANK_08= 91,
    SKILL_TANK_09= 92,
    SKILL_TANK_10= 93,
    SKILL_TANK_11= 94,
    SKILL_TANK_12= 95,
    SKILL_DEVIL_JOKE_FORK = 96,
    SKILL_DEVIL_PUNISH_FORK = 97,
    SKILL_DEVIL_GRIND_FORK = 98,
    SKILL_DEVIL_MAGIC_DRAIN = 99,
    SKILL_DEVIL_ENERGY_DRAIN = 100,
    SKILL_DEVIL_SEDUCE = 101,
    SKILL_DEVIL_PICK_HIP = 102,
    SKILL_DEVIL_WHISPER = 103,
    SKILL_DEVIL_DEATH_WHISPER = 104,
    SKILL_DEVIL_10= 105,
    SKILL_DEVIL_11= 106,
    SKILL_DEVIL_12= 107,
    SKILL_ROYALTY_WAVE = 108,
    SKILL_ROYALTY_DOUBLE_WAVE = 109,
    SKILL_ROYALTY_WAVE_ALL = 110,
    SKILL_ROYALTY_TEATIME = 111,
    SKILL_ROYALTY_COLOGNE = 112,
    SKILL_ROYALTY_DANCE = 113,
    SKILL_ROYALTY_ESCORT = 114,
    SKILL_ROYALTY_BLIND = 115,
    SKILL_ROYALTY_09= 116,
    SKILL_ROYALTY_10= 117,
    SKILL_ROYALTY_11= 118,
    SKILL_ROYALTY_12= 119,
    SKILL_FLOWER_FLOWER = 120,
    SKILL_FLOWER_FLOWER2 = 121,
    SKILL_FLOWER_EARTH_ANGER = 122,
    SKILL_FLOWER_FLOWER_PARK = 123,
    SKILL_FLOWER_FLOWER_PARK2 = 124,
    SKILL_FLOWER_LIFE_DEW = 125,
    SKILL_FLOWER_FAN = 126,
    SKILL_FLOWER_FAN_ALL = 127,
    SKILL_FLOWER_WHISTLE = 128,
    SKILL_FLOWER_10= 129,
    SKILL_FLOWER_11= 130,
    SKILL_FLOWER_12= 131,
    SKILL_SCIENTIST_CURE_CODE = 132,
    SKILL_SCIENTIST_BUG_CRUSH = 133,
    SKILL_SCIENTIST_JET_FLASK = 134,
    SKILL_SCIENTIST_JET_FLASK2 = 135,
    SKILL_SCIENTIST_BLACK_HOLE = 136,
    SKILL_SCIENTIST_GUARD_MASK = 137,
    SKILL_SCIENTIST_IGNITION = 138,
    SKILL_SCIENTIST_ABSORBENT = 139,
    SKILL_SCIENTIST_09= 140,
    SKILL_SCIENTIST_10= 141,
    SKILL_SCIENTIST_11= 142,
    SKILL_SCIENTIST_12= 143,
    SKILL_CAT_PUNCH = 144,
    SKILL_CAT_DOUBLE_SCRATCH = 145,
    SKILL_CAT_SCRATCH_ALL = 146,
    SKILL_CAT_LICK = 147,
    SKILL_CAT_POLISH_NAIL = 148,
    SKILL_CAT_FRISK = 149,
    SKILL_CAT_STEAL_DISH = 150,
    SKILL_CAT_GROOM = 151,
    SKILL_CAT_09= 152,
    SKILL_CAT_10= 153,
    SKILL_CAT_11= 154,
    SKILL_CAT_12= 155,
    SKILL_ELF_DANCE_ARROW = 156,
    SKILL_ELF_MAGIC_ARROW = 157,
    SKILL_ELF_ARROW_RAIN = 158,
    SKILL_ELF_FOREST_GRACE = 159,
    SKILL_ELF_PRAYER = 160,
    SKILL_ELF_CURE_MELODY = 161,
    SKILL_ELF_GUARD_ARROW = 162,
    SKILL_ELF_08= 163,
    SKILL_ELF_09= 164,
    SKILL_ELF_10= 165,
    SKILL_ELF_11= 166,
    SKILL_ELF_12= 167,
};

/*
Love Skill IDs
*/
enum LoveSkillEnum: u64 {
    LOVE_SKILL_APPEAL       = 0,
    LOVE_SKILL_COVER_1      = 1,
    LOVE_SKILL_REVENGE_1    = 2,
    LOVE_SKILL_HELP_1       = 3,
    LOVE_SKILL_GIVE_ITEM_1  = 4,
    LOVE_SKILL_PINCER_1     = 5,
    LOVE_SKILL_ADVICE_1     = 6,
    LOVE_SKILL_GET_WELL_1   = 7,
    LOVE_SKILL_ANGER_REVIVE_1   = 8,
    LOVE_SKILL_HEART_VOICE_1    = 9,
    LOVE_SKILL_PRAISE_1     = 10,
    LOVE_SKILL_CARE_1       = 11,
    LOVE_SKILL_SHARE_ITEM_1 = 12,
    LOVE_SKILL_APPEAL_2     = 13,
    LOVE_SKILL_COVER_2      = 14,
    LOVE_SKILL_REVENGE_2    = 15,
    LOVE_SKILL_HELP_2       = 16,
    LOVE_SKILL_PINCER_2     = 17,
    LOVE_SKILL_ADVICE_2     = 18,
    LOVE_SKILL_GET_WELL_2   = 19,
    LOVE_SKILL_APPEAL_3     = 20,
    LOVE_SKILL_COVER_3      = 21,
    LOVE_SKILL_REVENGE_3    = 22,
    LOVE_SKILL_HELP_3       = 23,
    LOVE_SKILL_PINCER_3     = 24,
    LOVE_SKILL_GET_WELL_3   = 25,
    /*Below here are quarrel skills*/
    LOVE_SKILL_INTERRUPT    = 26,
    LOVE_SKILL_SULK         = 27,
    LOVE_SKILL_DISTRACTED   = 28,
    LOVE_SKILL_DOGFIGHT     = 29,
    LOVE_SKILL_CRANKY       = 30,
    LOVE_SKILL_ANGRY        = 31,
};

enum PersonalSkillEnum: u64 {
    PERSONAL_SKILL_KIND_NOT_KILL = 0,
    PERSONAL_SKILL_KIND_GIVE_ITEM = 1,
    PERSONAL_SKILL_KIND_COVER = 2,
    PERSONAL_SKILL_VITAL_SLIP = 3,
    PERSONAL_SKILL_VITAL_SURVIVE = 4,
    PERSONAL_SKILL_VITAL_CHEER = 5,
    PERSONAL_SKILL_MYPACE_LAZE = 6,
    PERSONAL_SKILL_MYPACE_SERIOUS = 7,
    PERSONAL_SKILL_MYPACE_TAKE_ITEM = 8,
    PERSONAL_SKILL_MYPACE_HIDE = 9,
    PERSONAL_SKILL_MYPACE_AVOID_FEEL = 10,
    PERSONAL_SKILL_COOL_AVOID = 11,
    PERSONAL_SKILL_COOL_AVOID_FEEL = 12,
    PERSONAL_SKILL_COOL_CRITICAL = 13,
    PERSONAL_SKILL_COOL_NO_COVER = 14,
    PERSONAL_SKILL_WARY_SLOW = 15,
    PERSONAL_SKILL_WARY_FINISHER = 16,
    PERSONAL_SKILL_WARY_PREPARED = 17,
    PERSONAL_SKILL_GOOFY_WRONG = 18,
    PERSONAL_SKILL_GOOFY_SLEEP = 19,
    PERSONAL_SKILL_GOOFY_PLAY = 20,
    PERSONAL_SKILL_ROCKY_AGAIN = 21,
    PERSONAL_SKILL_ROCKY_HARDEN = 22,
    PERSONAL_SKILL_ROCKY_REJECT = 23,
};

enum JobEnum: u64 {
    JOB_FIGHTER = 0,
    JOB_WIZARD = 1,
    JOB_PRIEST = 2,
    JOB_THIEF = 3,
    JOB_IDOL_M = 4,
    JOB_IDOL_F = 5,
    JOB_VAMPIRE = 6,
    JOB_COOK = 7,
    JOB_TANK = 8,
    JOB_DEVIL = 9,
    JOB_ROYALTY = 10,
    JOB_FLOWER = 11,
    JOB_SCIENTIST = 12,
    JOB_CAT = 13,
    JOB_ELF = 14,
};

enum PersonalityEnum : s64 {
    PERSONALITY_NULL = -1,
    PERSONALITY_KIND = 0,   // Kind
    PERSONALITY_VITAL = 1,  // Energetic
    PERSONALITY_MYPACE = 2, // Laid-Back
    PERSONALITY_COOL = 3,   // Cool
    PERSONALITY_WARY = 4,   // Cautious
    PERSONALITY_GOOFY = 5,  // Airheaded
    PERSONALITY_ROCKY = 6,  // Stubborn
};

/*
An example of using enums with structures is:
/////////////////////////////////////////////
if(dmg->mAttackKind == ATTACK_KIND_MEGA_CURE) {

}
/////////////////////////////////////////////
*/