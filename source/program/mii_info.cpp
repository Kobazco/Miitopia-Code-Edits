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
This file will have all things related to the
"ActorInfo" structure. This is going to get messy.

Oh also, this is used for enemies as well. Needs
further testing on what is similar between them
though.
*/

/*
Documentation of (*(code *)actorInfo->field_0[0xFF]) (); stuff
[0x1f] = Job
*/

struct actorInfo {
    long field_0;   //0x0
    long *field_1;  //0x8   - BState Related
    long *field_2;  //0x10  - BState Related
    short MaxHP;    //0x18  - Max HP after modifiers    // All of these
    short MaxMP;    //0x1A  - Max MP after modifiers    // after "modifiers"
    short Atk;      //0x1C  - Atk after modifiers       // are the stats
    short Def;      //0x1E  - Def after modifiers       // after grub and
    short Mag;      //0x20  - Mag after modifiers       // equipment modifiers.
    short Spd;      //0x22  - Spd after modifiers       //
    short CurHP;    //0x24  - Current HP
    short CurMP;    //0x26  - Current MP
    char field_8;   //0x28 ???
    char field_9;   //0x29 Set to 1 when a Mii is Angry or Excited
    int field_10;   //0x2A Padding
    short field_11; //0x2E Padding
    int field_12;   //0x30
    short field_13; //0x34 ???
    short field_14; //0x36 ???
    short field_15; //0x38 ???
    short field_16; //0x3A Padding
    int field_17;   //0x3C ???
    uint field_18;  //0x40 ???
    uint field_19;  //0x44 ???
    uint field_20;  //0x48 ???
    int field_21;   //0x4C ???
    short field_22; //0x50 Padding
    char field_23;  //0x52 ???
    char field_24;  //0x53 ???
    int field_25;   //0x58 Padding
    int field_26;   //0x58 Padding
    char field_27;  //0x5C ???
    char field_28;  //0x5D Padding
    bool EnemyGuard;//0x5C - Is the enemy guarding. Prevents status effects.
    char field_29;  //0x5F Padding
    int field_30;   //0x60 ???
    char field_31;  //0x64 ???
    char ShieldType;  //0x65 - See ShieldTypes enum
    short field_35; //0x66 ???
    short field_36; //0x68 ???
    short field_37; //0x6A ???
    short StatusEffect;  //0x6C - See StatusEffect enum
                    // *(uint *)&ActorInfo->status_effect_0x6c;
                    // ^ can be used to check status, or even set it?
    short field_38; //0x6E ???
    ushort field_39;//0x70 ???
    short field_40; //0x72 ???
    short field_41; //0x74 ???
    char EnemySize; //0x76 - '\x01' small, '\x02' medium
    char EnemySize2; //0x77 - '\0' normal size
    short field_43; //0x78 ???
    short field_44; //0x7A Padding
    short EnemyAtk; //0x7C - Basic attack type for both Miis
                    //       and enemies.
    short field_45; //0x7E ???
    short field_46;  //0x80 ???
    short field_47; //0x82 Padding
    uint field_48;  //0x84  - Could be job, needs to be checked
    uint SkillInfo;  //0x88 - Something to do with relationships
    char field_50;  //0x8C Padding
    char field_51;  //0x8D
};

/*
Shield Types (MiiInfo->0x65)
*/

enum ShieldTypes : u64 {
  SHIELD_NONE = 0,
  SHIELD_BARRIER = 4,
  SHIELD_SPRINKLE = 8,
  SHIELD_AEGIS = 12,  // This is assumed, needs testing
};


/*
Status Effects (MiiInfo->0x6C)
*/
enum StatusEffect: u64 {
    STATUS_NORMAL = 0,
    STATUS_ANGRY = 1,
    STATUS_LAUGH = 2,
    STATUS_TERROR = 3,
    STATUS_CRY = 4,
    STATUS_EXCITE = 5,
    STATUS_SPITE = 6,
    STATUS_ANXIOUS = 7,
    STATUS_VACANCY = 8,
    STATUS_GLEEFUL = 9,
    STATUS_CRANKY = 10,
    STATUS_SLEEP = 11,
    STATUS_NIGHTMARE = 12,
    STATUS_DANCE = 13,
    STATUS_PUPPET = 14,
    STATUS_AGING = 15,
    STATUS_BURN = 16,
    STATUS_EAT = 17,
    STATUS_FACELESS = 18,
    STATUS_PHARAOH = 19,
    STATUS_SPIDER = 20,
    STATUS_STONE = 21,
    STATUS_ICE = 22,
    STATUS_BLOW = 23,
    STATUS_OVER_SLEEP = 24,
};
