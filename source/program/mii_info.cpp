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
"MiiInfo" structure. This is going to get messy.
*/

struct miiInfo {
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
    long long field_8;  //0x28 Padding
    int field_9;    //0x30
    short field_10;   //0x34
    int field_11;   //0x38
    int field_12;   //0x3C
    uint field_13;  //0x40
    uint field_14;  //0x44
    uint field_15;  //0x48
    int field_16;   //0x4C
    long long field_17; //0x50 Padding
    int field_18;   //0x58 Padding
    char field_19;  //0x5C
    short field_20; //0x5D Padding
    char field_21;  //0x5F Padding
    int field_22;   //0x60
    char field_33;  //0x64
    char ShieldType;  //0x65 - See ShieldTypes enum
    short field_35; //0x66 Padding
    short field_36; //0x68
    short field_37; //0x6A Padding
    uint StatusEffect;  //0x6C - See StatusEffect enum
    ushort field_39;//0x70
    short field_40; //0x72
    short field_41; //0x74
    short field_42; //0x76
    int field_43;   //0x78 Padding
    short field_44; //0x7C - For enemies, this is a byte that controls
                    //       if the attack was a "HardAttack" or not
    short field_45; //0x7E Padding
    char field_46;  //0x80
    char field_47;  //0x81 Padding
    short field_48; //0x82 Padding
    uint field_49;  //0x84  - Could be job, needs to be checked
    uint field_50;  //0x88 - Something to do with relationships
    char field_51;  //0x8C Padding
    char field_52;  //0x8D
};

/*
Shield Types (MiiInfo->0x65)
*/

enum ShieldTypes : u64 {
  SHIELD_NONE = 0,
  SHIELD_BARRIER = 4,
  SHIELD_SPRINKLE = 8,
  SHIELD_AEGIS = 12,  // This assumed, needs testing
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