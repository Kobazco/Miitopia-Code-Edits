#pragma once

#include <lib.hpp>

#define _FUNC_PREFIX(name)  \
    slide::func_ptrs::name

#define _FUNC_TYPE(name)    \
    APPEND(_FUNC_PREFIX(name), _t)

#define _FUNC_OFFSET(name)  \
    APPEND(_FUNC_PREFIX(name), _Offset)

#define FUNC_PTR(offset, name, ...)                             \
    namespace slide::func_ptrs {                                \
        using APPEND(name, _t) = __VA_ARGS__;                   \
        static constexpr size_t APPEND(name, _Offset) = offset; \
    }                                                           \
    extern _FUNC_TYPE(name) name

#define SETUP_FUNC_PTR(name)     \
    _FUNC_TYPE(name) name = reinterpret_cast<_FUNC_TYPE(name)>(exl::util::modules::GetTargetOffset(_FUNC_OFFSET(name)))


#include "misc_types.hpp"

// Misc.
FUNC_PTR(0x002B7670, SkillCureAllStart, void(*)(uintptr_t, short, uintptr_t));
FUNC_PTR(0x002B63A0, BigWeaponAtk, bool(*)(uintptr_t, uintptr_t, long));
FUNC_PTR(0x00278430, DoesSkillTgtEnemy, bool(*)(long, int));
FUNC_PTR(0x00278500, DoesSkillTgtAlly, bool(*)(long, int));
FUNC_PTR(0x00278550, DoesSkillTgtAllEnemy, bool(*)(long, int));
// Required by Skills
FUNC_PTR(0x00c127f0, SomeInitFunc, void(*)(uintptr_t*, int, uintptr_t*));
FUNC_PTR(0x00274840, GetHelperMiis, void(*)(uintptr_t, uintptr_t*, uintptr_t));
FUNC_PTR(0x002766d0, SomeSkillStringFunc, void(*)(long, uintptr_t, int, uintptr_t, uintptr_t*));
FUNC_PTR(0x002de430, SkillEffectAndMsg, void(*)(uintptr_t, int, uint));
FUNC_PTR(0x002b28f0, MypaceAction, bool(*)(uintptr_t, int, float));
FUNC_PTR(0x002de470, SomeBStatePlayer2_0x55, void(*)(uintptr_t));
FUNC_PTR(0x00270000, PlayBattleState, void(*)(uintptr_t, const char *, long));
FUNC_PTR(0x10027bb20, Some0x10CheckToBStateOther, void(*)(uintptr_t, int));
FUNC_PTR(0x00279980, FUN_7100279980, bool(*)(uintptr_t));
FUNC_PTR(0x0027bd30, SomeBStatePlayer, void(*)(uintptr_t, int));
FUNC_PTR(0x00270630, BStateEffectHandler, void(*)(uintptr_t, uintptr_t, long));
FUNC_PTR(0x002ddec0, FUN_71002ddec0, long(*)(uintptr_t, int, int));
FUNC_PTR(0x0028B200, HandleEnemyDamage, long(*)(uintptr_t, uintptr_t, bool, bool, bool, bool));
FUNC_PTR(0x002af540, FUN_71002af540, long*(*)(uintptr_t, uintptr_t));
FUNC_PTR(0x00266230, BattleStateFinalPlayer, long*(*)(long int*, long int*, int, char *, bool));
FUNC_PTR(0x00270d00, DoMPHeal, void(*)(uintptr_t, long, int, ushort));
FUNC_PTR(0x0026f370, SomeHealingFunc, uintptr_t(*)(float, uintptr_t, long));
FUNC_PTR(0x0027bb20, CutInSkill, void(*)(uintptr_t, int));
FUNC_PTR(0x002ae6e0, InflictDance, void(*)(uintptr_t, uintptr_t, const char *, const char *));
FUNC_PTR(0x00266f40, GetActorIdx, ushort(*)(long *));
FUNC_PTR(0x00266f60, GetActorInfo, long(*)(long *, uint));
FUNC_PTR(0x00275390, AtkTypeJobSkillCheck, long(*)(uintptr_t, int, bool));
FUNC_PTR(0x0027d4a0, ReduceMP, void(*)(uintptr_t, int));
// PRNG
FUNC_PTR(0x00c484c0, get_rand_U32, uint(*)(uint *));
FUNC_PTR(0x0026f230, SkillActivationRNG, ulong(*)(uintptr_t));

#undef FUNC_PTR