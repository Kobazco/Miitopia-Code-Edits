#include <lib.hpp>
#include <nn.hpp>
#include <cstring>
#include <string>

#include "func_ptrs.hpp"
#include "sead/container/seadBuffer.h"
#include "sead/prim/seadSafeString.hpp"

// New files for organization
#include "enums.cpp"
#include "mii_info.cpp"

#define LOG(...)                                                    \
  {                                                                 \
      int length = snprintf(buffer, sizeof(buffer), __VA_ARGS__);   \
      svcOutputDebugString(buffer, length);                         \
  }

/*
An array for turning the enum into
printable strings. Better than doing
crazy long switch statements all the
time I suppose.

I hate that I have to have this as
a static in this file, but oh well.
*/

static std::pair<SkillEnum, const char*> SkillEnumStrings[] = {
    {SKILL_FIGHTER_DOUBLE, "SKILL_FIGHTER_DOUBLE"},
    {SKILL_FIGHTER_TWICE, "SKILL_FIGHTER_TWICE"},
    {SKILL_FIGHTER_SPIN, "SKILL_FIGHTER_SPIN"},
    {SKILL_FIGHTER_GUARD, "SKILL_FIGHTER_GUARD"},
    {SKILL_FIGHTER_EYE_SLASH, "SKILL_FIGHTER_EYE_SLASH"},
    {SKILL_FIGHTER_SPIN_2, "SKILL_FIGHTER_SPIN_2"},
    {SKILL_FIGHTER_REVIVE_SLAP, "SKILL_FIGHTER_REVIVE_SLAP"},
    {SKILL_FIGHTER_SLAP, "SKILL_FIGHTER_SLAP"},
    {SKILL_FIGHTER_09, "SKILL_FIGHTER_09"},
    {SKILL_FIGHTER_10, "SKILL_FIGHTER_10"},
    {SKILL_FIGHTER_11, "SKILL_FIGHTER_11"},
    {SKILL_FIGHTER_12, "SKILL_FIGHTER_12"},
    {SKILL_WIZARD_FIRE, "SKILL_WIZARD_FIRE"},
    {SKILL_WIZARD_FIRE_2, "SKILL_WIZARD_FIRE_2"},
    {SKILL_WIZARD_FLAME_TOWER, "SKILL_WIZARD_FLAME_TOWER"},
    {SKILL_WIZARD_LIGHTNING, "SKILL_WIZARD_LIGHTNING"},
    {SKILL_WIZARD_LIGHTNING_2, "SKILL_WIZARD_LIGHTNING_2"},
    {SKILL_WIZARD_LIGHTNING_3, "SKILL_WIZARD_LIGHTNING_3"},
    {SKILL_WIZARD_EXPLOSION, "SKILL_WIZARD_EXPLOSION"},
    {SKILL_WIZARD_EXPLOSION_2, "SKILL_WIZARD_EXPLOSION_2"},
    {SKILL_WIZARD_EXPLOSION_3, "SKILL_WIZARD_EXPLOSION_3"},
    {SKILL_WIZARD_BARRIER, "SKILL_WIZARD_BARRIER"},
    {SKILL_WIZARD_SLEEP, "SKILL_WIZARD_SLEEP"},
    {SKILL_WIZARD_BIG_WEAPON, "SKILL_WIZARD_BIG_WEAPON"},
    {SKILL_PRIEST_CURE, "SKILL_PRIEST_CURE"},
    {SKILL_PRIEST_CURE_2, "SKILL_PRIEST_CURE_2"},
    {SKILL_PRIEST_CURE_3, "SKILL_PRIEST_CURE_3"},
    {SKILL_PRIEST_CURE_ALL, "SKILL_PRIEST_CURE_ALL"},
    {SKILL_PRIEST_CURE_ALL_2, "SKILL_PRIEST_CURE_ALL_2"},
    {SKILL_PRIEST_CALM, "SKILL_PRIEST_CALM"},
    {SKILL_PRIEST_KARMA, "SKILL_PRIEST_KARMA"},
    {SKILL_PRIEST_RESURRECT, "SKILL_PRIEST_RESURRECT"},
    {SKILL_PRIEST_RESURRECT_2, "SKILL_PRIEST_RESURRECT_2"},
    {SKILL_PRIEST_HOLY, "SKILL_PRIEST_HOLY"},
    {SKILL_PRIEST_11, "SKILL_PRIEST_11"},
    {SKILL_PRIEST_12, "SKILL_PRIEST_12"},
    {SKILL_THIEF_DAGGER, "SKILL_THIEF_DAGGER"},
    {SKILL_THIEF_DAGGER_2, "SKILL_THIEF_DAGGER_2"},
    {SKILL_THIEF_ROCK, "SKILL_THIEF_ROCK"},
    {SKILL_THIEF_TRAP, "SKILL_THIEF_TRAP"},
    {SKILL_THIEF_LARGE_TRAP, "SKILL_THIEF_LARGE_TRAP"},
    {SKILL_THIEF_CREEP, "SKILL_THIEF_CREEP"},
    {SKILL_THIEF_STEAL_ITEM, "SKILL_THIEF_STEAL_ITEM"},
    {SKILL_THIEF_HIGH_JUMP, "SKILL_THIEF_HIGH_JUMP"},
    {SKILL_THIEF_09, "SKILL_THIEF_09"},
    {SKILL_THIEF_10, "SKILL_THIEF_10"},
    {SKILL_THIEF_11, "SKILL_THIEF_11"},
    {SKILL_THIEF_12, "SKILL_THIEF_12"},
    {SKILL_IDOL_ENCORE, "SKILL_IDOL_ENCORE"},
    {SKILL_IDOL_FUN_SERVICE, "SKILL_IDOL_FUN_SERVICE"},
    {SKILL_IDOL_LOVE_CALL, "SKILL_IDOL_LOVE_CALL"},
    {SKILL_IDOL_CONCERT, "SKILL_IDOL_CONCERT"},
    {SKILL_IDOL_HOWLING, "SKILL_IDOL_HOWLING"},
    {SKILL_IDOL_HOWLING_2, "SKILL_IDOL_HOWLING_2"},
    {SKILL_IDOL_DANCE, "SKILL_IDOL_DANCE"},
    {SKILL_IDOL_ANGEL_SONG, "SKILL_IDOL_ANGEL_SONG"},
    {SKILL_IDOL_LOVE_AND_PEACE, "SKILL_IDOL_LOVE_AND_PEACE"},
    {SKILL_IDOL_10, "SKILL_IDOL_10"},
    {SKILL_IDOL_11, "SKILL_IDOL_11"},
    {SKILL_IDOL_12, "SKILL_IDOL_12"},
    {SKILL_VAMPIRE_EAT, "SKILL_VAMPIRE_EAT"},
    {SKILL_VAMPIRE_BAT_ROSE, "SKILL_VAMPIRE_BAT_ROSE"},
    {SKILL_VAMPIRE_ZOMBIFY, "SKILL_VAMPIRE_ZOMBIFY"},
    {SKILL_VAMPIRE_BREATH, "SKILL_VAMPIRE_BREATH"},
    {SKILL_VAMPIRE_BREATH2, "SKILL_VAMPIRE_BREATH2"},
    {SKILL_VAMPIRE_BREATH3, "SKILL_VAMPIRE_BREATH3"},
    {SKILL_VAMPIRE_CURSE, "SKILL_VAMPIRE_CURSE"},
    {SKILL_VAMPIRE_REVIVE, "SKILL_VAMPIRE_REVIVE"},
    {SKILL_VAMPIRE_09, "SKILL_VAMPIRE_09"},
    {SKILL_VAMPIRE_10, "SKILL_VAMPIRE_10"},
    {SKILL_VAMPIRE_11, "SKILL_VAMPIRE_11"},
    {SKILL_VAMPIRE_12, "SKILL_VAMPIRE_12"},
    {SKILL_COOK_COOKING, "SKILL_COOK_COOKING"},
    {SKILL_COOK_COOKING2, "SKILL_COOK_COOKING2"},
    {SKILL_COOK_COOKING3, "SKILL_COOK_COOKING3"},
    {SKILL_COOK_HEATING, "SKILL_COOK_HEATING"},
    {SKILL_COOK_SPICY, "SKILL_COOK_SPICY"},
    {SKILL_COOK_SPICY_ALL, "SKILL_COOK_SPICY_ALL"},
    {SKILL_COOK_BANQUET, "SKILL_COOK_BANQUET"},
    {SKILL_COOK_COOK_MONSTER, "SKILL_COOK_COOK_MONSTER"},
    {SKILL_COOK_BAKED_BANANA, "SKILL_COOK_BAKED_BANANA"},
    {SKILL_COOK_10, "SKILL_COOK_10"},
    {SKILL_COOK_11, "SKILL_COOK_11"},
    {SKILL_COOK_12, "SKILL_COOK_12"},
    {SKILL_TANK_REPAIR, "SKILL_TANK_REPAIR"},
    {SKILL_TANK_WILD_SHOT, "SKILL_TANK_WILD_SHOT"},
    {SKILL_TANK_HUMAN_CANNON, "SKILL_TANK_HUMAN_CANNON"},
    {SKILL_TANK_BEAM, "SKILL_TANK_BEAM"},
    {SKILL_TANK_BEAM2, "SKILL_TANK_BEAM2"},
    {SKILL_TANK_HEAT_SHOT, "SKILL_TANK_HEAT_SHOT"},
    {SKILL_TANK_DEFENSE, "SKILL_TANK_DEFENSE"},
    {SKILL_TANK_08, "SKILL_TANK_08"},
    {SKILL_TANK_09, "SKILL_TANK_09"},
    {SKILL_TANK_10, "SKILL_TANK_10"},
    {SKILL_TANK_11, "SKILL_TANK_11"},
    {SKILL_TANK_12, "SKILL_TANK_12"},
    {SKILL_DEVIL_JOKE_FORK, "SKILL_DEVIL_JOKE_FORK"},
    {SKILL_DEVIL_PUNISH_FORK, "SKILL_DEVIL_PUNISH_FORK"},
    {SKILL_DEVIL_GRIND_FORK, "SKILL_DEVIL_GRIND_FORK"},
    {SKILL_DEVIL_MAGIC_DRAIN, "SKILL_DEVIL_MAGIC_DRAIN"},
    {SKILL_DEVIL_ENERGY_DRAIN, "SKILL_DEVIL_ENERGY_DRAIN"},
    {SKILL_DEVIL_SEDUCE, "SKILL_DEVIL_SEDUCE"},
    {SKILL_DEVIL_PICK_HIP, "SKILL_DEVIL_PICK_HIP"},
    {SKILL_DEVIL_WHISPER, "SKILL_DEVIL_WHISPER"},
    {SKILL_DEVIL_DEATH_WHISPER, "SKILL_DEVIL_DEATH_WHISPER"},
    {SKILL_DEVIL_10, "SKILL_DEVIL_10"},
    {SKILL_DEVIL_11, "SKILL_DEVIL_11"},
    {SKILL_DEVIL_12, "SKILL_DEVIL_12"},
    {SKILL_ROYALTY_WAVE, "SKILL_ROYALTY_WAVE"},
    {SKILL_ROYALTY_DOUBLE_WAVE, "SKILL_ROYALTY_DOUBLE_WAVE"},
    {SKILL_ROYALTY_WAVE_ALL, "SKILL_ROYALTY_WAVE_ALL"},
    {SKILL_ROYALTY_TEATIME, "SKILL_ROYALTY_TEATIME"},
    {SKILL_ROYALTY_COLOGNE, "SKILL_ROYALTY_COLOGNE"},
    {SKILL_ROYALTY_DANCE, "SKILL_ROYALTY_DANCE"},
    {SKILL_ROYALTY_ESCORT, "SKILL_ROYALTY_ESCORT"},
    {SKILL_ROYALTY_BLIND, "SKILL_ROYALTY_BLIND"},
    {SKILL_ROYALTY_09, "SKILL_ROYALTY_09"},
    {SKILL_ROYALTY_10, "SKILL_ROYALTY_10"},
    {SKILL_ROYALTY_11, "SKILL_ROYALTY_11"},
    {SKILL_ROYALTY_12, "SKILL_ROYALTY_12"},
    {SKILL_FLOWER_FLOWER, "SKILL_FLOWER_FLOWER"},
    {SKILL_FLOWER_FLOWER2, "SKILL_FLOWER_FLOWER2"},
    {SKILL_FLOWER_EARTH_ANGER, "SKILL_FLOWER_EARTH_ANGER"},
    {SKILL_FLOWER_FLOWER_PARK, "SKILL_FLOWER_FLOWER_PARK"},
    {SKILL_FLOWER_FLOWER_PARK2, "SKILL_FLOWER_FLOWER_PARK2"},
    {SKILL_FLOWER_LIFE_DEW, "SKILL_FLOWER_LIFE_DEW"},
    {SKILL_FLOWER_FAN, "SKILL_FLOWER_FAN"},
    {SKILL_FLOWER_FAN_ALL, "SKILL_FLOWER_FAN_ALL"},
    {SKILL_FLOWER_WHISTLE, "SKILL_FLOWER_WHISTLE"},
    {SKILL_FLOWER_10, "SKILL_FLOWER_10"},
    {SKILL_FLOWER_11, "SKILL_FLOWER_11"},
    {SKILL_FLOWER_12, "SKILL_FLOWER_12"},
    {SKILL_SCIENTIST_CURE_CODE, "SKILL_SCIENTIST_CURE_CODE"},
    {SKILL_SCIENTIST_BUG_CRUSH, "SKILL_SCIENTIST_BUG_CRUSH"},
    {SKILL_SCIENTIST_JET_FLASK, "SKILL_SCIENTIST_JET_FLASK"},
    {SKILL_SCIENTIST_JET_FLASK2, "SKILL_SCIENTIST_JET_FLASK2"},
    {SKILL_SCIENTIST_BLACK_HOLE, "SKILL_SCIENTIST_BLACK_HOLE"},
    {SKILL_SCIENTIST_GUARD_MASK, "SKILL_SCIENTIST_GUARD_MASK"},
    {SKILL_SCIENTIST_IGNITION, "SKILL_SCIENTIST_IGNITION"},
    {SKILL_SCIENTIST_ABSORBENT, "SKILL_SCIENTIST_ABSORBENT"},
    {SKILL_SCIENTIST_09, "SKILL_SCIENTIST_09"},
    {SKILL_SCIENTIST_10, "SKILL_SCIENTIST_10"},
    {SKILL_SCIENTIST_11, "SKILL_SCIENTIST_11"},
    {SKILL_SCIENTIST_12, "SKILL_SCIENTIST_12"},
    {SKILL_CAT_PUNCH, "SKILL_CAT_PUNCH"},
    {SKILL_CAT_DOUBLE_SCRATCH, "SKILL_CAT_DOUBLE_SCRATCH"},
    {SKILL_CAT_SCRATCH_ALL, "SKILL_CAT_SCRATCH_ALL"},
    {SKILL_CAT_LICK, "SKILL_CAT_LICK"},
    {SKILL_CAT_POLISH_NAIL, "SKILL_CAT_POLISH_NAIL"},
    {SKILL_CAT_FRISK, "SKILL_CAT_FRISK"},
    {SKILL_CAT_STEAL_DISH, "SKILL_CAT_STEAL_DISH"},
    {SKILL_CAT_GROOM, "SKILL_CAT_GROOM"},
    {SKILL_CAT_09, "SKILL_CAT_09"},
    {SKILL_CAT_10, "SKILL_CAT_10"},
    {SKILL_CAT_11, "SKILL_CAT_11"},
    {SKILL_CAT_12, "SKILL_CAT_12"},
    {SKILL_ELF_DANCE_ARROW, "SKILL_ELF_DANCE_ARROW"},
    {SKILL_ELF_MAGIC_ARROW, "SKILL_ELF_MAGIC_ARROW"},
    {SKILL_ELF_ARROW_RAIN, "SKILL_ELF_ARROW_RAIN"},
    {SKILL_ELF_FOREST_GRACE, "SKILL_ELF_FOREST_GRACE"},
    {SKILL_ELF_PRAYER, "SKILL_ELF_PRAYER"},
    {SKILL_ELF_CURE_MELODY, "SKILL_ELF_CURE_MELODY"},
    {SKILL_ELF_GUARD_ARROW, "SKILL_ELF_GUARD_ARROW"},
    {SKILL_ELF_08, "SKILL_ELF_08"},
    {SKILL_ELF_09, "SKILL_ELF_09"},
    {SKILL_ELF_10, "SKILL_ELF_10"},
    {SKILL_ELF_11, "SKILL_ELF_11"},
    {SKILL_ELF_12, "SKILL_ELF_12"},
};

/*
Array for job enum to string for
logging.
*/
static const char* jobEnumToString[] = {
    "Fighter",
    "Wizard",
    "Priest",
    "Thief",
    "IdolM",
    "IdolF",
    "Vampire",
    "Cook",
    "Tank",
    "Devil",
    "Royalty",
    "Flower",
    "Scientist",
    "Cat",
    "Elf",
};

JobEnum last_job = JOB_FIGHTER;
int EventNum = 0;

bool PriestMPAtk = false;
bool PriestMPState = false;
int PriestMPRestore = 0;

HOOK_DEFINE_TRAMPOLINE(ELinkCreate) {

    static uintptr_t Callback(uintptr_t _this, const char* name) {

        /*char buffer[500];
        int len = std::snprintf(buffer, sizeof(buffer), "ELink Load: %s", name);
        svcOutputDebugString(buffer, len);*/

        return Orig(_this, name);
    }
};

HOOK_DEFINE_TRAMPOLINE(ELinkLookup) {
    static int Callback(uintptr_t _this, const char* name) {
        
        auto ret = Orig(_this, name);

        /*if(ret >= 0) {
            char buffer[500];
            int len = std::snprintf(buffer, sizeof(buffer), "ELink Lookup: %s", name);
            svcOutputDebugString(buffer, len);
        }*/

        return ret;
    }
};

HOOK_DEFINE_TRAMPOLINE(PctlLoad) {
    static void Callback(uintptr_t _this, uintptr_t arg1, const char** arg2) {
        /* Do the normal load. */
        Orig(_this, arg1, arg2);

        /* If we aren't already trying to load battle, also load battle. */
        if(strcmp(arg2[1], "battle") != 0) {
            /* Horribly hackily copy the sead::SafeString and change it's string. */
            const char* fakeStr[2];
            static_assert(sizeof(fakeStr) == sizeof(sead::SafeString), "");
            
            std::memcpy(fakeStr, reinterpret_cast<void*>(arg2), sizeof(fakeStr));
            fakeStr[1] = "battle";

            /* Do the battle load. */
            Orig(_this, arg1, fakeStr);
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(BattleState) {
    static void Callback(intptr_t * miiInfo, const char* state, long * arg1) {

        char buffer[500];

        //LOG("Mii Ptr: %ld", *miiInfo);

        int len = std::snprintf(buffer, sizeof(buffer), "Battle State: %s", state);
        svcOutputDebugString(buffer, len);
        
        //LOG("3rd Param: %ld", arg1[1]);

        return Orig(miiInfo, state, arg1);
    }
};

HOOK_DEFINE_TRAMPOLINE(IsFueding) {
    static bool Callback(intptr_t miiPtr, intptr_t miiTgtPtr, bool arg1) {
        return true;
    }
};

static std::string s_FolderPath = "content:/cmn/elink/ex/";
static std::vector<u8*> s_ELinkBins;

static Result TryLoad() {

    char buf[500];
    #define PRINT(...)                                              \
        {                                                           \
            int len = std::snprintf(buf, sizeof(buf), __VA_ARGS__); \
            svcOutputDebugString(buf, len);                         \
        }

    /* Yes I know this is ew. */
    #define TRY1(expr)                              \
        {                                           \
            Result r = expr;                        \
            if(R_FAILED(r)) {                       \
                nn::fs::CloseDirectory(dirHandle);  \
                return r;                           \
            }                                       \
        }
        
    #define TRY2(expr)                              \
        {                                           \
            Result r = expr;                        \
            if(R_FAILED(r)) {                       \
                nn::fs::CloseDirectory(dirHandle);  \
                delete[] entries;                   \
                return r;                           \
            }                                       \
        }
    #define TRY3(expr)                              \
        {                                           \
            Result r = expr;                        \
            if(R_FAILED(r)) {                       \
                nn::fs::CloseDirectory(dirHandle);  \
                delete[] entries;                   \
                delete[] data;                      \
                return r;                           \
            }                                       \
        }

    PRINT("[ELink::Load] Initializing...");

    nn::fs::DirectoryHandle dirHandle;
    R_TRY(nn::fs::OpenDirectory(&dirHandle, s_FolderPath.c_str(), nn::fs::OpenDirectoryMode_File));

    PRINT("[ELink::Load] Opened directory...");

    s64 entryCount;
    TRY1(nn::fs::GetDirectoryEntryCount(&entryCount, dirHandle));

    PRINT("[ELink::Load] Got file count ...");

    auto entries = new nn::fs::DirectoryEntry[entryCount];
    TRY2(nn::fs::ReadDirectory(&entryCount, entries, dirHandle, entryCount));

    PRINT("[ELink::Load] Read directory...");

    for(int i = 0; i < entryCount; i++) {

        auto& entry = entries[i];

        PRINT("[ELink::Load] Opening file \"%s\"...", entry.m_Name);

        nn::fs::FileHandle fileHandle;
        TRY2(nn::fs::OpenFile(&fileHandle, (s_FolderPath + std::string(entry.m_Name)).c_str(), nn::fs::OpenMode_Read));

        PRINT("[ELink::Load] Reading file \"%s\"...", entry.m_Name);

        auto data = new u8[entry.m_FileSize];
        TRY3(nn::fs::ReadFile(fileHandle, 0, data, entry.m_FileSize));

        s_ELinkBins.push_back(data);

        nn::fs::CloseFile(fileHandle);
    }

    PRINT("[ELink::Load] Done.");

    #undef TRY1
    #undef TRY2
    #undef TRY3

    delete[] entries;
    nn::fs::CloseDirectory(dirHandle);

    return 0;
}

struct Buffer {
    s32 mLength;
    void** mPtrs;
};

HOOK_DEFINE_REPLACE(ELinkInject) {

    static int Callback(uintptr_t _this, void* vanillaElink) {
        char buf[500];
        auto array = exl::util::pointer_path::Follow<Buffer*, 0x40>(_this);

        /* Add bins. */
        for(size_t i = 0; i < s_ELinkBins.size(); i++) {
            PRINT("[ELink::Inject] Injecting %ld/%ld...", i+1, s_ELinkBins.size());
            array->mPtrs[i] = s_ELinkBins.at(i);
        }
        /* Add vanilla at the end for least priority. */
        PRINT("[ELink::Inject] Injecting vanilla...");
        array->mPtrs[s_ELinkBins.size()] = vanillaElink;

        PRINT("[ELink::Inject] Done.");

        /* Index of the ELink file just inserted, although this ends up being discarded. Meh? */
        return array->mLength-1;
    }

};

HOOK_DEFINE_TRAMPOLINE(ELinkBufferCtor) {
    static void Callback(Buffer* _this, uintptr_t parent, uintptr_t heap, int count) {
        TryLoad();

        /* +1 to make room for vanilla ELink bin. */
        Orig(_this, parent, heap, s_ELinkBins.size() + 1);
    }
};

HOOK_DEFINE_TRAMPOLINE(RockyPersonalCheck) {
    static bool Callback(miiInfo *MiiInfo, uintptr_t skillIdx) {
        
        char buffer[500];
        LOG("Stubborn Again! check for skill %ld.", skillIdx);

        // If we got here while this is set, it
        // means that the priest just did a basic
        // attack on an enemy, so do custom stuff.
        if (PriestMPState) {
            LOG("Start Priest MP Heal");
            PriestMPAtk = true;
            DoMPHeal((uintptr_t)MiiInfo, 71554516732, (int)*(short *)(MiiInfo->field_1 + 0xf), 1);   // 71437483340
            PriestMPState = false;
        }

        return Orig(MiiInfo, skillIdx);
    }
};

HOOK_DEFINE_TRAMPOLINE(DmgConstructor1) {
    static void Callback(float dmgMod, float arg1, dmgclass *arg2, int arg3, bool IsMagic, short arg5, short arg6) {

        char buffer[500];

        //LOG("Initial Mod: %f", dmgMod);
        //LOG("Bunus Hits Mod: %f", arg1);
        //LOG("Current Atk/Skill: %d", arg3);
        //LOG("Skill/Magic?: %s", IsMagic ? "true" : "false");
        //LOG("param 6: %d", arg5);
        //LOG("param 7: %d", arg6);

        Orig(dmgMod, arg1, arg2, arg3, IsMagic, arg5, arg6);
        arg2->FinalDmgMod = 0;
        LOG("Dmg set to 1 for testing");

        //LOG("========DMGCLASS========");
        //LOG("AtkKind 0x00: %d", arg2->AtkKind);
        //LOG("field_2 0x02: %d", arg2->field_2);
        //LOG("field_4 0x04: %d", arg2->field_4);
        //LOG("Final Dmg Mod 0x08: %f" , arg2->FinalDmgMod);
        //LOG("Final Dmg %% 0x08: %f %%", arg2->FinalDmgMod * 100);
        //LOG("field_C 0x0C: %ld", arg2->field_C);
        //LOG("field_14 0x14: %f", arg2->field_14);
        //LOG("BaseMod 0x18: %f", arg2->BaseMod);
        //LOG("field_1C 0x1C: %d", arg2->field_1C);
        //LOG("Bool 0x1D: %s", arg2->SomeBool ? "true" : "false");
        //LOG("field_1E 0x1E: %d", arg2->field_1E);
        //LOG("field_1F 0x1F: %d", arg2->field_1F);
        //LOG("field_20 0x20: %d", arg2->field_20);
        //LOG("field_22 0x22: %d", arg2->field_22);
    }
};



HOOK_DEFINE_TRAMPOLINE(BasicAtk) {
    static void Callback(intptr_t miiInfo, bool arg1, uintptr_t arg2) {

        char buffer[500];
        LOG("BAtk param_3: %ld", arg2);

        Orig(miiInfo, true, arg2);
    }
};

static bool SkillNewStart(miiInfo *MiiInfo, intptr_t MiiTgtInfo) {
    /*bool PersonalityCheck;
    //long uVar1;
    //float *Side;
    //long uVar2;
    //long uVar3;
    //long MiiHelpInfo;
    float local_e0 [10];
    //short auStack_b8[24];
    float local_a0;
    uintptr_t HelperMiis;
    //long local_88;
    uintptr_t auStack_80[32];
    const char *BState;

    char buffer[500];

    LOG("=====Scientist09 Start=====");

    HelperMiis = 0;
    //local_88 = 0;
    SomeInitFunc(&HelperMiis, 4, auStack_80); //auStack_80
    LOG("SomeInitFunc: Passed");
    GetHelperMiis(MiiInfo, &HelperMiis, MiiTgtInfo);
    LOG("GetHelperMiis: Passed");
    SomeSkillStringFunc(0x3f800000, MiiInfo, 0x8C, MiiTgtInfo, &HelperMiis);
    LOG("SomeSkillStringFunc: Passed");

    SomeBStatePlayer3(MiiInfo, 0x8C, 1);
    LOG("SomeBStatePlayer 3: Passed");

    HelperMiis = 0;
    if ((int)HelperMiis == 0){
        local_e0[0] = 1.0;
        PersonalityCheck = MypaceAction(MiiInfo, 0x8C, 1.0);
        LOG("MypaceAction: %s", PersonalityCheck ? "true" : "false");
        if (PersonalityCheck) {
            local_a0 = local_e0[0] * local_a0;
        }
        SomeBStatePlayer2_0x55(MiiInfo);
        LOG("SomeBStatePlayer2_0x55: Passed");
        BState = "SkillCureCodeSelfStart";
        if (MiiTgtInfo != MiiInfo) {
            BState = "SkillCureCodeStart";
        }
        //local_e0[0] = (float)CONCAT22(0xffff, *(short *)(MiiTgtInfo[1] + 0x78));
        PlayBattleState(MiiInfo, BState, 1.0);
        LOG("PlayBattleState: Passed");
        Some0x10CheckToBStateOther(MiiInfo, 0x8C);
        LOG("Some0x10CheckToBStateOther: Passed");
        if ((MiiTgtInfo != MiiInfo) && (PersonalityCheck = FUN_7100279980(MiiTgtInfo), PersonalityCheck)) {
            SomeBStatePlayer(MiiTgtInfo, 0x17);
            LOG("SomeBStatePlayer: Passed");
            *(uint *)(MiiTgtInfo + 0x11) = *(uint *)(MiiTgtInfo + 0x11) | 0x1000;
            *(uint *)(MiiInfo + 0x11) = *(uint *)(MiiInfo + 0x11) | 0x2000;
            //SkillNew(MiiInfo, MiiTgtInfo, auStack_B8);
            return 1;
        }
    }*/
    return 0;
};

HOOK_DEFINE_TRAMPOLINE(HealingSkills) {
    static bool Callback(miiInfo *MiiInfo, SkillEnum skillIdx, uintptr_t param3, uintptr_t MiiTgtInfo) {
        
        char buffer[500];
        LOG("HealingSkills Entered");

        for (auto& skill : SkillEnumStrings) {
            if (skill.first == skillIdx) {
                LOG("CurrSkill: %s", skill.second)
                break;
            }
        }

        //LOG("Curr Skill: %s", skillEnumToString[static_cast<int>(skillIdx)]);

        switch(skillIdx) {
            case SkillEnum::SKILL_SCIENTIST_09:
                /*Scientist09_*/
                LOG("Scientist09_");
                //return false;
                SkillNewStart(MiiInfo, MiiTgtInfo);
                return true;
            break;
            default:
                return Orig(MiiInfo, skillIdx, param3, MiiTgtInfo);
            break;
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(SkillDisabler) {
    static bool Callback(uintptr_t BattleInfo, int SkillSlot) {
        char buffer[500];
        LOG("=====Skill Checker=====")
        LOG("Game is checking if a skill is usable...");
        //LOG("BattleInfo: %ld", BattleInfo);
        //LOG("Skill Slot: %d", SkillSlot);

        bool Tgt;
        int *SkillData;
        uint SkillStatus;
        int SomeVar;

        // Updates slot based on skill page
        SkillSlot = (uint)*(char *)(BattleInfo + 0x161) * 3 + SkillSlot;
        LOG("Skill Slot: %d", SkillSlot);
        // Set up
        SkillStatus = (uint)(char)SkillSlot;
        SkillData = *(int **)(BattleInfo + 0x158) + (long)(int)SkillStatus * 2;
        SomeVar = *SkillData;
        //LOG("BattleInfo 0x158: %ld", BattleInfo + 0x158);
        LOG("SkillIdx: %d", SkillData[0]);
        LOG("SkillStatus: %d", SkillData[1]);
        //LOG("SkillData: %d", SomeVar);

        if (SomeVar == SkillEnum::SKILL_SCIENTIST_09) {
            SkillData[1] = 0;
            SomeVar = *SkillData;
        }

        // This is required otherwise it always gets skill info of slot 0
        *(char *)(BattleInfo + 0x192) = (char)SkillSlot;
        //LOG("0x192: %d", (char)BattleInfo + 0x192);
        *(int *)(BattleInfo + 0x144) = 0;

        // If the skill targets 1 enemy
        Tgt = DoesSkillTgtEnemy(*(long *)(BattleInfo + 0x148), SomeVar);
        LOG("Targets 1 Enemy: %s", Tgt ? "Yes" : "Nope");
        if (Tgt) {
            SomeVar = 6;
            *(int *)(BattleInfo + 0x144) = SomeVar;
            return true;
        }
        else {
            // Does it target an ally?
            Tgt = DoesSkillTgtAlly(*(long *)(BattleInfo + 0x148), SomeVar);
            LOG("Targets Ally: %s", Tgt ? "Yes" : "Nope");
            if (!Tgt) {
                // If not, does is it a skill that targets all enemies?
                Tgt = DoesSkillTgtAllEnemy(*(long *)(BattleInfo + 0x148), SomeVar);
                LOG("Targets All: %s", Tgt ? "Yes" : "Nope");
                if (Tgt) {
                    // Targets all enemies
                    *(int *)(BattleInfo + 0x144) = 6;
                    *(int *)(BattleInfo + 0x19C) = 1;
                    return true;
                }
                // Targets random enemies/allies
                LOG("Targets Random");
                *(int *)(BattleInfo + 0x196) = *(int *)(BattleInfo + 0x191);
                return true;
            }
            SomeVar = 7;
            *(int *)(BattleInfo + 0x144) = SomeVar;
            return true;
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(DoesSkillTgtAlly_Hook) {
    static bool Callback(uintptr_t BattleInfo, SkillEnum SkillIdx) {
        char buffer[100];
        switch(SkillIdx){
            case SkillEnum::SKILL_SCIENTIST_09:
                LOG("SKILL_SCIENTIST_09, targets ally.")
                return true;
            default:
                return Orig(BattleInfo, SkillIdx);
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(DoesSkillTgtEnemy_Hook) {
    static bool Callback(uintptr_t BattleInfo, SkillEnum SkillIdx) {
        switch(SkillIdx){
            default:
                return Orig(BattleInfo, SkillIdx);
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(DoesSkillTgtAllEnemy_Hook) {
    static bool Callback(uintptr_t BattleInfo, SkillEnum SkillIdx) {
        switch(SkillIdx){
            default:
                return Orig(BattleInfo, SkillIdx);
        }
    }
};

HOOK_DEFINE_INLINE(EnemyDiml) {
    static void Callback(exl::hook::InlineCtx* ctx) {
        char buffer[100];
        LOG("===EnemyDimlInfo===");
        LOG("xml = %ld", ctx->X[8]);

        ctx->W[8] = 0x200;
        //ctx->X[8] = ctx->X[29] - 0xc0;
        //ctx->X[9] = ctx->X[26] + 0x2A8;
    }
};

HOOK_DEFINE_INLINE(EnemyAssetsSwitch) {
    static void Callback(exl::hook::InlineCtx* ctx) {
        char buffer[100];
        LOG("===EnemyAssetsSwitch 0===");
        ctx->X[1] = 0;
    }
};

HOOK_DEFINE_INLINE(GetLastJob) {
    static void Callback(exl::hook::InlineCtx* ctx) {
        char buffer[100];
        LOG("===Inline===");
        /*for(int i = 0; i < 29; i++) {
            LOG("X%d: %lx", i, ctx->X[i]);
        }*/
        last_job = (JobEnum)ctx->X[8];
        LOG("Stored job %s", jobEnumToString[static_cast<int>(last_job)]);
    }
};

HOOK_DEFINE_TRAMPOLINE(GetDmgOrHealAmount) {
    static int Callback(uintptr_t param_1) {
        char buffer [100];
        //LOG("===GetDmgOrHealAmount===");
        //LOG("Input: %ld", param_1);

        // If we are here through a Priest MP Basic Atk, do custom stuff
        if (PriestMPAtk) {
            // Make sure we unset this before leaving...
            PriestMPAtk = false;
            // Return our new heal amount
            LOG("Return MP Heal: %d", PriestMPRestore);
            return PriestMPRestore;
        }
        // This is what dictates the number in the UI. For some reason,
        //  its seperate. Happens after my manual call of DoMPHeal.
        if (param_1 == 71555737708) {
            //LOG("Return MP Heal: %d", PriestMPRestore);
            return PriestMPRestore;
        }

        int ret = Orig(param_1);
        //LOG("Output: %d", ret);
        return ret;
    }
};

HOOK_DEFINE_TRAMPOLINE(HandleEnemyDamage_Hook) {
    static void Callback(miiInfo *MiiInfo, uintptr_t EnemyInfo, bool param3, bool param4, bool param5, bool param6) {
        char buffer[500];
        LOG("===HandleEnemydamage===")
        return Orig(MiiInfo, EnemyInfo, param3, param4, param5, param6);
    }
};

HOOK_DEFINE_TRAMPOLINE(BasicAttackState) {
    static bool Callback(miiInfo *MiiInfo, uintptr_t TgtInfo, uintptr_t dmgstructInput, uintptr_t HelperInfo, uintptr_t param_5) {
        char buffer[500];
        //double uVar2;
        //long *plVar1;
        long IsEnemyDead;
        //dmgclass dmg_struct;
        //long *puVar8;

        LOG("===BasicAtkBState===");
        //LOG("===MiiInfoStruct===");

        /*size_t structSize = sizeof(miiInfo);
        char* structBytes = (char*)MiiInfo;

        for (size_t i = 0; i < structSize; i++) {
            LOG("byte %ld: %02X", i, structBytes[i]);
        }*/
        
        if (last_job >= 0 && last_job < 15) {
            LOG("Job doing basic atk: %s", jobEnumToString[static_cast<int>(last_job)]);
        }

        // Priest basic attacks restore MP
        if (last_job == JobEnum::JOB_PRIEST) {
            LOG("Trying Priest new basic atk effect...");

            // Do and store math for MP to restore
            int CurMP = MiiInfo->CurMP;
            int MaxMP = MiiInfo->MaxMP;
            float MPHeal = (MaxMP - CurMP) * 0.3;
            PriestMPRestore = (int)MPHeal;
            PriestMPState = true;

            //DoMPHeal((uintptr_t)MiiInfo, 71554516732, (int)*(short *)(MiiInfo->field_1 + 0xf), 1);   // 71437483340
        }

        // Mage new basic attack states
        if (last_job == JobEnum::JOB_WIZARD){
            LOG("Job is Wizard, do new basic attack");

            /*puVar8 = FUN_71002af540((uintptr_t)MiiInfo,dmgstructInput);
            if (puVar8 == 0x0) {
                dmg_struct.AtkKind = 0x7100e0a12c;
                dmg_struct.FinalDmgMod = (float)(*(short *)((TgtInfo + 0x8) + 0x78));
                puVar8 = BattleStateFinalPlayer(MiiInfo->field_2, MiiInfo->field_1, 1, (long *)&dmg_struct, 0);
            }*/
            //plVar1 = (long *)FUN_71002ddec0(MiiInfo, 0xffffffff, 0);

            //uVar2 = SomeBStatePlayer3(MiiInfo, 0xf, 1);
            
            // BStates
            SomeBStatePlayer2_0x55((uintptr_t)MiiInfo);
            PlayBattleState((uintptr_t)MiiInfo, "SkillMagicStart", 1);
            PlayBattleState((uintptr_t)MiiInfo, "SkillFire", 1);
            FUN_7100270630((uintptr_t)MiiInfo, 0, 0);

            // Handle Enemy Damage
            IsEnemyDead = HandleEnemyDamage((uintptr_t)MiiInfo, TgtInfo, false, false, false, false);
            if (IsEnemyDead == 0) {
                PlayBattleState((uintptr_t)MiiInfo, "DefeatEnemySkillMagic", 1);
            }
            PostAttackStatusCheck((uintptr_t)MiiInfo);
            return 1;
        }
        // Otherwise, do things as normal
        else {
            return Orig(MiiInfo, TgtInfo, dmgstructInput, HelperInfo, param_5);
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(EventActionSetup) {
    static void Callback(uintptr_t param1) {
        char buffer[100];

        // Event # tracking
        EventNum += 1;
        LOG("Event#: %d", EventNum);

        return Orig(param1);
    }
};

HOOK_DEFINE_TRAMPOLINE(EventActionBool) {
    static bool Callback(uintptr_t param1, uintptr_t param2, uint16_t param3) {
        char buffer[100];
        bool value = Orig(param1, param2, param3);
        LOG("Event#: %d bool: %s", EventNum, value ? "true" : "false");

        return value;
    }
};

HOOK_DEFINE_TRAMPOLINE(SetVisibleHPMP) {
    static void Callback(long *param1, long param2, long ****param3) {
        char buffer[100];
        LOG("SetVisibleHPMP Accessed (Event#: %d)", EventNum);
        LOG("Param1: %ld", *param1); // Map Event: 150213912 // Non map: 150213912
        LOG("Param2: %ld", param2); // Map Event: 75345443312 // Non map: 75345442592
        LOG("Param3: %ld", ****param3); // Map Event: 71675650792 // Non Map: 75345442320
        
        return Orig(param1, param2, param3);
    }
};

HOOK_DEFINE_INLINE(VisibleHPMPInline) {
    static void Callback(exl::hook::InlineCtx* ctx) {
        char buffer[100];
        ctx->X[22] = 71562689040;
        ctx->X[21] = *(long *)(ctx->X[22] + 0x388);
        LOG("lVar5: %ld", ctx->X[21]);
        LOG("lVar6: %ld", ctx->X[22]);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();

    namespace patch = exl::patch;
    namespace armv8 = exl::armv8;
    namespace reg = armv8::reg;
    namespace inst = armv8::inst;

    //ELinkCreate::InstallAtOffset(0x00B1DCE0);
    //ELinkLookup::InstallAtOffset(0x00B1D590);
    //ELinkInject::InstallAtOffset(0x00B1D7C0);
    //ELinkBufferCtor::InstallAtOffset(0x00B1DBF0);
    PctlLoad::InstallAtOffset(0x006C7340);
    //BattleState::InstallAtOffset(0x00270000);
    //IsFueding::InstallAtOffset(0x00273360);
    HealingSkills::InstallAtOffset(0x00291AC0);
    RockyPersonalCheck::InstallAtOffset(0x00279850);
    DmgConstructor1::InstallAtOffset(0x00269D60);
    //BasicAtk::InstallAtOffset(0x002EA6A0);
    //SkillDisabler::InstallAtOffset(0x00235720);
    SkillDisabler::InstallAtOffset(0x00235580);
    DoesSkillTgtAlly_Hook::InstallAtOffset(0x00278500);
    DoesSkillTgtEnemy_Hook::InstallAtOffset(0x00278430);
    DoesSkillTgtAllEnemy_Hook::InstallAtOffset(0x00278550);
    GetLastJob::InstallAtOffset(0x00288f20);
    BasicAttackState::InstallAtOffset(0x0028ca10);
    //EnemyDiml::InstallAtOffset(0x004599d0);
    //EnemyAssetsSwitch::InstallAtOffset(0x00200bd8);
    //EventActionSetup::InstallAtOffset(0x00941920);
    //SetVisibleHPMP::InstallAtOffset(0x008efa20);
    //EventActionBool::InstallAtOffset(0x00944ff0);
    //VisibleHPMPInline::InstallAtOffset(0x008efacc);
    GetDmgOrHealAmount::InstallAtOffset(0x0026a010);
    //HandleEnemyDamage_Hook::InstallAtOffset(0x0028b200);
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}