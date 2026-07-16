#include "test_zone_pc_candidate_gate_2712.h"
#include "map/zone_pc_candidate_gate.h"
auto runZonePCCandidateGate2712SelfTests() -> bool { return zoneentityvisibility::ShouldConsiderPC(45.0f) && !zoneentityvisibility::ShouldConsiderPC(45.001f); }
