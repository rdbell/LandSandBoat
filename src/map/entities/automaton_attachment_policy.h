#pragma once
#include "common/cbasetypes.h"
#include <array>
namespace automatonattachmenthelpers { inline auto HasAttachment(const std::array<uint8,12>& attachments,uint8 attachment)->bool { for(auto id:attachments) if(id==attachment) return true; return false; } }
