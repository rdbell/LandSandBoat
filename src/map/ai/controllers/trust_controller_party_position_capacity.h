#pragma once

#include <cstddef>
#include <cstdint>

namespace trustcontrollerpartyposition
{
// Resolve returns the first matching trust position, or zero when the trust is absent.
template <typename TrustList, typename IDFor>
auto Resolve(const TrustList& trustList, const uint32_t ownerID, IDFor&& idFor) -> uint8_t
{
    for (std::size_t i = 0; i < trustList.size(); ++i)
    {
        if (idFor(trustList.at(i)) == ownerID)
        {
            return static_cast<uint8_t>(i);
        }
    }
    return 0;
}
} // namespace trustcontrollerpartyposition
