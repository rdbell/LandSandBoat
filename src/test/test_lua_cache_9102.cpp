#include "test_lua_cache_9102.h"

#include "map/lua/lua_cache.h"
#include "omega_self_test_registry.h"

#include <iostream>
#include <string>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "lua cache 9102 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runLuaCache9102SelfTests() -> bool
{
    sol::state luaState;
    luaState.open_libraries(sol::lib::base);
    luaState.script("function cached_handler() return 9102 end");
    const sol::function handler = luaState["cached_handler"];

    LuaCache cache;
    bool      ok = true;

    ok = expect(!cache.find("missing").valid(), "find miss returns nil") && ok;
    ok = expect(cache.findEntry("missing") == nullptr, "findEntry miss returns null") && ok;

    cache.store("handler", handler);
    ok = expect(cache.find("handler") == handler, "find returns stored function") && ok;
    ok = expect(cache.findEntry("handler") != nullptr && cache.findEntry("handler")->valid(),
                "findEntry returns valid stored function") &&
         ok;

    // An invalid reference is a deliberate negative cache entry and must remain
    // distinguishable from a key that was never stored.
    cache.store("negative", sol::reference{});
    ok = expect(!cache.find("negative").valid(), "negative find returns nil") && ok;
    ok = expect(cache.findEntry("negative") != nullptr && !cache.findEntry("negative")->valid(),
                "negative findEntry remains present") &&
         ok;

    cache.store("handler", sol::lua_nil);
    ok = expect(cache.findEntry("handler") != nullptr && !cache.findEntry("handler")->valid(),
                "store overwrites an existing entry") &&
         ok;

    cache.store("handler", handler);
    int resolveCount = 0;
    const auto resolved = cache.getOrResolveFunction("handler", [&]()
                                                      {
                                                          ++resolveCount;
                                                          return handler;
                                                      });
    ok = expect(resolved == handler && resolveCount == 0, "hit does not resolve") && ok;

    const auto negative = cache.getOrResolveFunction("negative", [&]()
                                                      {
                                                          ++resolveCount;
                                                          return handler;
                                                      });
    ok = expect(!negative.valid() && resolveCount == 0, "negative hit does not resolve") && ok;

    auto& key = cache.keyBuffer();
    key       = "snapshotted";
    const auto snapshotted = cache.getOrResolveFunction(key, [&]()
                                                        {
                                                            key = "clobbered";
                                                            ++resolveCount;
                                                            return handler;
                                                        });
    ok = expect(snapshotted == handler && resolveCount == 1, "miss resolves once") && ok;
    ok = expect(cache.find("snapshotted") == handler, "miss stores the original key") && ok;
    ok = expect(!cache.find("clobbered").valid(), "resolver key mutation does not retarget store") && ok;

    auto& sameBuffer = cache.keyBuffer();
    ok = expect(&sameBuffer == &key && sameBuffer == "clobbered", "keyBuffer is reusable") && ok;

    cache.clear();
    ok = expect(cache.findEntry("handler") == nullptr && cache.findEntry("negative") == nullptr &&
                    cache.findEntry("snapshotted") == nullptr,
                "clear removes positive and negative entries") &&
         ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("lua-cache-9102", runLuaCache9102SelfTests);
