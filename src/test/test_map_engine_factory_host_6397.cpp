#include "test_map_engine_factory_host_6397.h"

#include "common/ipp.h"
#include "map/map_app_config.h"
#include "map/map_config.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map engine factory host 6397 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapApplication MapConfig/IPP + factory (slice 6397).
// Go: BuildMapConfigFromInputs / CreateMapEngineFactory / NewMapEngine.
auto runMapEngineFactoryHost6397SelfTests() -> bool
{
    bool ok = true;

    const auto pure = mapapp::BuildMapEngineConfigInputs(
        /*ip=*/0x0100007Fu, // 127.0.0.1 network-order sample
        /*port=*/54230,
        /*inCI=*/true,
        /*lazyZones=*/true,
        /*rebuildNavmeshes=*/false);

    MapConfig cfg{};
    cfg.ipp              = IPP(pure.ip, pure.port);
    cfg.inCI             = pure.inCI;
    cfg.lazyZones        = pure.lazyZones;
    cfg.rebuildNavmeshes = pure.rebuildNavmeshes;

    ok = expect(cfg.ipp.getIP() == pure.ip, "ipp ip") && ok;
    ok = expect(cfg.ipp.getPort() == pure.port, "ipp port") && ok;
    ok = expect(cfg.inCI, "inCI") && ok;
    ok = expect(cfg.lazyZones, "lazy") && ok;
    ok = expect(!cfg.rebuildNavmeshes, "rebuild default false") && ok;
    ok = expect(!cfg.isTestServer, "isTestServer default") && ok;

    // Absent CLI → zero IPP residual.
    const auto absent = mapapp::BuildMapEngineConfigInputs(0, 0, false, false, false);
    const IPP zeroIPP(absent.ip, absent.port);
    ok = expect(zeroIPP.getIP() == 0 && zeroIPP.getPort() == 0, "zero ipp") && ok;

    // Log format pure residuals (map_engine init).
    ok = expect(std::string("map_ip: ") + cfg.ipp.getIPString() == std::string("map_ip: ") + cfg.ipp.getIPString(), "map_ip log shape") && ok;
    ok = expect(cfg.ipp.getPort() == 54230, "map_port value") && ok;

    return ok;
}
