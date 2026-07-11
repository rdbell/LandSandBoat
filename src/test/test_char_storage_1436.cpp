#include "test_char_storage_1436.h"

#include "map/char_storage_capacity.h"
#include "map/item_container.h"

#include <array>
#include <iostream>

auto runCharStorage1436SelfTests() -> bool
{
    std::array<int, MAX_CONTAINER_ID> containers{};
    for (std::size_t i = 0; i < containers.size(); ++i)
    {
        containers[i] = static_cast<int>(i + 100);
    }

    std::array<int*, MAX_CONTAINER_ID> containerPointers{};
    for (std::size_t i = 0; i < containerPointers.size(); ++i)
    {
        containerPointers[i] = &containers[i];
    }

    bool ok = true;
    for (uint8 locationId = LOC_INVENTORY; locationId < MAX_CONTAINER_ID; ++locationId)
    {
        auto resolved = charstoragehelpers::Resolve(locationId, containerPointers);
        ok = resolved.has_value() && *resolved == &containers[locationId] && ok;
    }

    containerPointers[LOC_MOGSAFE] = nullptr;
    auto validNull                 = charstoragehelpers::Resolve(LOC_MOGSAFE, containerPointers);
    ok = validNull.has_value() && *validNull == nullptr && ok;

    ok = charstoragehelpers::Resolve(MAX_CONTAINER_ID, containerPointers) == std::nullopt &&
         ok;
    ok = charstoragehelpers::Resolve(255, containerPointers) == std::nullopt &&
         ok;

    if (!ok)
    {
        std::cerr << "char storage 1436 self-test failed\n";
    }
    return ok;
}
