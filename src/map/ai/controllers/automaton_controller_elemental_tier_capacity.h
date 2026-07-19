#pragma once

#include <cstdint>

namespace automatoncontrollerelementaltier
{
inline auto Select(int32_t targetHP, int32_t automatonMP) -> int8_t
{
    if (automatonMP < 4)
    {
        return -1;
    }
    if (targetHP <= 50 || automatonMP < 16)
    {
        return 0;
    }
    if (targetHP <= 150 || automatonMP < 40)
    {
        return 1;
    }
    if (targetHP <= 200 || automatonMP < 88)
    {
        return 2;
    }
    if (targetHP <= 600 || automatonMP < 156)
    {
        return 3;
    }
    return 4;
}
} // namespace automatoncontrollerelementaltier
