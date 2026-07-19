#pragma once

namespace petcontrollerbufftick
{
inline auto AllowsBuffTick(bool isPetEntity, bool isLightSpirit) -> bool
{
    return isPetEntity && isLightSpirit;
}
} // namespace petcontrollerbufftick
