#pragma once

namespace trustfadeouthelpers
{

template <typename BaseFadeOut, typename SendDespawn>
inline void Apply(BaseFadeOut&& baseFadeOut, SendDespawn&& sendDespawn)
{
    baseFadeOut();
    sendDespawn();
}

} // namespace trustfadeouthelpers
