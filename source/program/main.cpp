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

int last_job = 0;

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
/*
HOOK_DEFINE_TRAMPOLINE(RockyPersonalCheck) {
    static bool Callback(uintptr_t *MiiPtr, uintptr_t skillIdx) {
        
        char buffer[500];
        LOG("Stubborn Again! check for skill %ld.", skillIdx);

        return true;
    }
};*/

struct dmgclass {
    long long AtkUsed;      //0x00 - The "attack" used. Is different, and consistent, based on the atk/skill used
                        //      24 - Pincer (Back), Fire
                        //      36 - Mega Cure x2
                        //      39 - Explosion
                        //      50 - Demonic Whisper
                        //      59 - Unstable Formula
                        //      63 - Enemy Basic Atk
                        //      71 - Pincer (Front), Super Distracted Basic Atk
                        //      72 - Basic Atk
                        //      75 - Double Scratch
                        //      79 x3 - Lend a Hand?, Mega Cure x1,3
                        //      89 - Cure.exe
                        //      104 - Punishing Pitchfork
                        //      109 - Glitch
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
        //LOG("AtkUsed 0x00: %d", arg2->AtkUsed);
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

static bool SkillNewStart(intptr_t MiiInfo, intptr_t MiiTgtInfo) {
    bool PersonalityCheck;
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
    }
    return 0;
};

HOOK_DEFINE_TRAMPOLINE(HealingSkills) {
    static bool Callback(uintptr_t MiiInfo, short skillIdx, uintptr_t param3, uintptr_t MiiTgtInfo) {
        
        char buffer[500];
        LOG("HealingSkills Entered");
        LOG("Curr Skill: %d", skillIdx);

        switch(skillIdx) {
            case 0x8C:
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

        if (SomeVar == 140) {
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
    static bool Callback(uintptr_t BattleInfo, int SkillIdx) {
        switch(SkillIdx){
            case 0x85:
                return false;
            case 0x8c:
                return true;
            default:
                return Orig(BattleInfo, SkillIdx);
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(DoesSkillTgtEnemy_Hook) {
    static bool Callback(uintptr_t BattleInfo, int SkillIdx) {
        switch(SkillIdx){
            case 0x85:
                return true;
            default:
                return Orig(BattleInfo, SkillIdx);
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(DoesSkillTgtAllEnemy_Hook) {
    static bool Callback(uintptr_t BattleInfo, int SkillIdx) {
        switch(SkillIdx){
            case 0x85:
                return false;
            default:
                return Orig(BattleInfo, SkillIdx);
        }
    }
};

HOOK_DEFINE_INLINE(InlineChecker) {
    static void Callback(exl::hook::InlineCtx* ctx) {
        char buffer[100];
        LOG("===Inline===");
        /*for(int i = 0; i < 29; i++) {
            LOG("X%d: %lx", i, ctx->X[i]);
        }*/
        last_job = ctx->X[8];
        LOG("Stored job %d", last_job);
    }
};

HOOK_DEFINE_TRAMPOLINE(BasicAttackState) {
    static bool Callback(uintptr_t MiiInfo, uintptr_t TgtInfo, uintptr_t dmgstructInput, uintptr_t HelperInfo, uintptr_t param_5) {
        char buffer[500];
        //double uVar2;
        //long *plVar1;
        long IsEnemyDead;
        dmgclass dmg_struct;
        long *puVar8;

        LOG("===BasicAtkBState===");
        if (last_job != 0) {
            LOG("Job was stored as: %d", last_job); // 1 is Wizard(Mage)
        }
        // Mage new basic attack states
        if (last_job == 1){
            LOG("Job is Wizard, do new basic attack");

            puVar8 = FUN_71002af540(MiiInfo,dmgstructInput);
            if (puVar8 == 0x0) {
                dmg_struct.AtkUsed = 0x7100e0a12c;
                dmg_struct.FinalDmgMod = (float)(*(short *)((TgtInfo + 0x8) + 0x78));
                puVar8 = BattleStateFinalPlayer(MiiInfo + 0x10, MiiInfo + 0x8 , 1, (long *)&dmg_struct, 0);
            }
            //plVar1 = (long *)FUN_71002ddec0(MiiInfo, 0xffffffff, 0);
            //uVar2 = SomeBStatePlayer3(MiiInfo, 0xf, 1);
            
            // BStates
            SomeBStatePlayer2_0x55(MiiInfo);
            PlayBattleState(MiiInfo, "SkillMagicStart", 1);
            PlayBattleState(MiiInfo, "SkillFire", 1);
            FUN_7100270630(MiiInfo, 0, 0);

            // Handle Enemy Damage
            IsEnemyDead = HandleEnemyDamage(MiiInfo, TgtInfo, false, false, false, false);
            if (IsEnemyDead == 0) {
                PlayBattleState(MiiInfo, "DefeatEnemySkillMagic", 1);
            }
            //PostAttackStatusCheck(MiiInfo);
            return 1;

        }
        // Otherwise, do things as normal
        else {
            return Orig(MiiInfo, TgtInfo, dmgstructInput, HelperInfo, param_5);
        }
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    //envSetOwnProcessHandle(exl::util::proc_handle::Get());
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
    //RockyPersonalCheck::InstallAtOffset(0x00279850);
    DmgConstructor1::InstallAtOffset(0x00269D60);
    //BasicAtk::InstallAtOffset(0x002EA6A0);
    //SkillDisabler::InstallAtOffset(0x00235720);
    SkillDisabler::InstallAtOffset(0x00235580);
    DoesSkillTgtAlly_Hook::InstallAtOffset(0x00278500);
    DoesSkillTgtEnemy_Hook::InstallAtOffset(0x00278430);
    DoesSkillTgtAllEnemy_Hook::InstallAtOffset(0x00278550);
    InlineChecker::InstallAtOffset(0x00288f20);
    BasicAttackState::InstallAtOffset(0x0028ca10);
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}