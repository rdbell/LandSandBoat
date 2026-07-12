#pragma once

#include <functional>
#include <utility>

namespace chareventpackethelpers
{

template <typename StringsEmpty, typename HasParams, typename HasTextTable,
          typename SendBasic, typename SendNumeric, typename SendString,
          typename SetEventAnimation, typename MarkPosition, typename SendServerStatus>
inline void SendAndFinalize(
    StringsEmpty&&      stringsEmpty,
    HasParams&&         hasParams,
    HasTextTable&&      hasTextTable,
    SendBasic&&         sendBasic,
    SendNumeric&&       sendNumeric,
    SendString&&        sendString,
    SetEventAnimation&& setEventAnimation,
    MarkPosition&&      markPosition,
    SendServerStatus&&  sendServerStatus)
{
    if (std::invoke(std::forward<StringsEmpty>(stringsEmpty)))
    {
        if (std::invoke(std::forward<HasParams>(hasParams)) ||
            std::invoke(std::forward<HasTextTable>(hasTextTable)))
        {
            std::invoke(std::forward<SendNumeric>(sendNumeric));
        }
        else
        {
            std::invoke(std::forward<SendBasic>(sendBasic));
        }
    }
    else
    {
        std::invoke(std::forward<SendString>(sendString));
    }

    std::invoke(std::forward<SetEventAnimation>(setEventAnimation));
    std::invoke(std::forward<MarkPosition>(markPosition));
    std::invoke(std::forward<SendServerStatus>(sendServerStatus));
}

} // namespace chareventpackethelpers
