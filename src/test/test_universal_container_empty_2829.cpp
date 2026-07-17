#include "test_universal_container_empty_2829.h"

#include "map/universal_container.h"
#include "map/universal_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "universal container empty 2829 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runUniversalContainerEmpty2829SelfTests() -> bool
{
    using ucontainerhelpers::IsContainerTypeEmpty;

    bool ok = true;

    // Empty type constant matches itself.
    ok = expect(IsContainerTypeEmpty(static_cast<std::uint8_t>(UCONTAINER_EMPTY),
                                     static_cast<std::uint8_t>(UCONTAINER_EMPTY)),
                "empty == empty → true") &&
         ok;

    // Non-empty types are not empty against UCONTAINER_EMPTY.
    ok = expect(!IsContainerTypeEmpty(static_cast<std::uint8_t>(UCONTAINER_SHOP),
                                      static_cast<std::uint8_t>(UCONTAINER_EMPTY)),
                "shop == empty → false") &&
         ok;
    ok = expect(!IsContainerTypeEmpty(static_cast<std::uint8_t>(UCONTAINER_TRADE),
                                      static_cast<std::uint8_t>(UCONTAINER_EMPTY)),
                "trade == empty → false") &&
         ok;
    ok = expect(!IsContainerTypeEmpty(static_cast<std::uint8_t>(UCONTAINER_SYNTES),
                                      static_cast<std::uint8_t>(UCONTAINER_EMPTY)),
                "syntes == empty → false") &&
         ok;

    // Injected constant identity: same non-empty types compare equal.
    ok = expect(IsContainerTypeEmpty(static_cast<std::uint8_t>(UCONTAINER_SHOP),
                                     static_cast<std::uint8_t>(UCONTAINER_SHOP)),
                "shop == shop → true") &&
         ok;
    ok = expect(!IsContainerTypeEmpty(static_cast<std::uint8_t>(UCONTAINER_EMPTY),
                                      static_cast<std::uint8_t>(UCONTAINER_SHOP)),
                "empty == shop → false") &&
         ok;

    // Host dual-wire: default container is empty; typed is not.
    CUContainer container;
    ok = expect(container.IsContainerEmpty(), "default host empty") && ok;
    container.SetType(UCONTAINER_SHOP);
    ok = expect(!container.IsContainerEmpty(), "typed host not empty") && ok;
    container.Clean();
    ok = expect(container.IsContainerEmpty(), "clean restores host empty") && ok;

    return ok;
}
