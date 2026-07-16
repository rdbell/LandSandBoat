#include "test_c2s_equipset_set_runtime.h"
#include "map/packets/c2s/0x051_equipset_set.h"
auto runC2SEquipSetSetRuntimeSelfTests() -> bool { const auto p=equipsetsethelpers::MakeDispatchPlan(16); return p.equipCount==16&&p.persist&&p.checkGearSet&&p.updateHealth&&p.retriggerLatents; }
