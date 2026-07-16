#pragma once
#include "common/cbasetypes.h"
namespace mobvisibilityhelpers { constexpr uint32 HideHP=0x100, Untargetable=0x800; inline auto SetFlag(uint32 flags,uint8& updateMask,uint32 flag,bool value)->uint32 { if(value) flags|=flag; else flags&=~flag; updateMask|=0x01; return flags; } inline auto Has(uint32 flags,uint32 flag)->bool{return flags&flag;} }
