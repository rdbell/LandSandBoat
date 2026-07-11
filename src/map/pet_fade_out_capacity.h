#pragma once

namespace petfadeouthelpers
{

template <typename MobFadeOut, typename SendDespawn>
inline void Apply(MobFadeOut&& mobFadeOut, SendDespawn&& sendDespawn)
{
    mobFadeOut();
    sendDespawn();
}

} // namespace petfadeouthelpers
