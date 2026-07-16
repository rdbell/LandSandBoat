#include "test_c2s_equip_set_runtime.h"
#include "map/packets/c2s/0x050_equip_set.h"
auto runC2SEquipSetRuntimeSelfTests() -> bool { const auto p=equipsethelpers::MakeDispatchPlan(); return p.equip&&p.persist&&p.checkGearSet&&p.updateHealth&&p.retriggerLatents; }
