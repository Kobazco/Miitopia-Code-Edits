#include <lib.hpp>
#include <nn.hpp>
#include <cstring>
#include <string>

#include "func_ptrs.hpp"
#include "sead/container/seadBuffer.h"
#include "sead/prim/seadSafeString.hpp"

#include "new_skills.hpp"
#include "mii_info.hpp"
#include "enums.hpp"

#define LOG(...)                                                    \
  {                                                                 \
      int length = snprintf(buffer, sizeof(buffer), __VA_ARGS__);   \
      svcOutputDebugString(buffer, length);                         \
  }

static void Dmg_SkillStartCommon(actorInfo *MiiInfo, actorInfo *EnemyInfo, uintptr_t *HelperMiis, SkillEnum SkillIdx, const char *BStateStart, const char *BStateHelpStart) {
    //float local_b0 [10];
    //long lVar4;
    //uintptr_t auStack_88 = 0;
    char buffer [100];
    //float local_70;
    LOG("Dmg_SkillStartCommon");

    //SkillInfoRelated(0.0, MiiInfo, SkillIdx, HelperMiis);
    //LOG("SkillInfoRelated Passed");
    SkillEffectAndMsg((uintptr_t)MiiInfo, 133, 1);
    LOG("SkillEffectAndMessage Passed");
    //if (*HelperMiis == 0) { // No Helper Miis
        //local_b0[0] = 1.0;
        /*lVar4 = MypaceAction((uintptr_t)MiiInfo, SkillIdx, local_b0);
        if (lVar4 == 0) {
            LendAHand(uintptr_t(MiiInfo), auStack_88);
        }
        else {
            local_70 = local_b0[0] * 0.5;
        }*/
        SomeBStatePlayer2_0x55(uintptr_t(MiiInfo));
        PlayBattleState(uintptr_t(MiiInfo), BStateStart, 68902991952);
        CutInSkill(uintptr_t(MiiInfo), SkillIdx);
    //}
    return;
};

// 68902991952 - Start
// 68902991856 - Attack/Hit
// 68902991880 - End

// New Skills
void Scientist09_Start(actorInfo *MiiInfo, actorInfo *EnemyInfo, uintptr_t *HelperMiis) {
    char buffer [100];
    uintptr_t auStack_88 [24];

    LOG("Scientist09_Start");
    Dmg_SkillStartCommon(MiiInfo, EnemyInfo, HelperMiis, SKILL_SCIENTIST_09, "SkillBugCrushStart", "SkillBugCrushHelpStart");

    BugCrush(MiiInfo, EnemyInfo, auStack_88);
    return;
};
