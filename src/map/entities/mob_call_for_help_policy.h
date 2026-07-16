#pragma once
#include "common/cbasetypes.h"
namespace mobcallforhelphelpers { inline auto HasCallForHelp(uint16 flags, uint16 callForHelpFlag) -> bool { return (flags & callForHelpFlag) != 0; } }
