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
    char field_34;  //0x65
    short field_35; //0x66 Padding
    short field_36; //0x68
    short field_37; //0x6A Padding
    uint status_effect;  //0x6C
    ushort field_39;//0x70
    short field_40; //0x72
    short field_41; //0x74
    short field_42; //0x76
    int field_43;   //0x78 Padding
    short field_44; //0x7C
    short field_45; //0x7E Padding
    char field_46;  //0x80
    char field_47;  //0x81 Padding
    short field_48; //0x82 Padding
    uint field_49;  //0x84
    uint field_50;  //0x88
    char field_51;  //0x8C Padding
    char field_52;  //0x8D
};
