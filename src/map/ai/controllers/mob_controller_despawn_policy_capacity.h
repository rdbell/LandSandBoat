#pragma once
namespace mobcontrollerdespawnpolicy { inline auto CanDespawn(bool local, bool global) -> bool { return !local && !global; } }
