#pragma once

namespace automatoncontrollertpmovegate
{
inline auto CanTryTPMove(int tp) -> bool
{
    return tp >= 1000;
}
} // namespace automatoncontrollertpmovegate
