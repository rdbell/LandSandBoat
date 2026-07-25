-----------------------------------
-- Pure system tests for ninja tool dual-wire (slice 6720).
-- Calls production xi.job_utils.ninja pure exports.
-- Goldens match internal/ninjatool (0891) and ninja_tool_capacity.h.
-----------------------------------

require('scripts/globals/job_utils/ninja')

local nin = xi.job_utils.ninja
local t = nin.tool

describe('Ninja tool constants', function()
    it('pins primary tools and universal fudas', function()
        assert(t.UCHITAKE == 1161)
        assert(t.SHIHEI == 1179)
        assert(t.INOSHISHINOFUDA == 2971)
        assert(t.SHIKANOFUDA == 2972)
        assert(t.CHONOFUDA == 2973)
        assert(t.RANKA == 8803)
        assert(t.FURUSUMI == 8804)
    end)
end)

describe('substituteTool', function()
    it('maps elemental utility and enfeeble groups', function()
        for _, id in ipairs({ t.UCHITAKE, t.TSURARA, t.KAWAHORI_OGI, t.MAKIBISHI, t.HIRAISHIN, t.MIZU_DEPPO }) do
            assert(nin.substituteTool(id) == t.INOSHISHINOFUDA)
        end

        for _, id in ipairs({ t.RYUNO, t.MOKUJIN, t.SANJAKU_TENUGUI, t.KABENRO, t.SHINOBI_TABI, t.SHIHEI, t.RANKA, t.FURUSUMI }) do
            assert(nin.substituteTool(id) == t.SHIKANOFUDA)
        end

        for _, id in ipairs({ t.SOSHI, t.KODOKU, t.KAGINAWA, t.JUSATSU, t.SAIRUI_RAN, t.JINKO }) do
            assert(nin.substituteTool(id) == t.CHONOFUDA)
        end
    end)

    it('returns nil for fudas and unknown', function()
        assert(nin.substituteTool(t.INOSHISHINOFUDA) == nil)
        assert(nin.substituteTool(t.SHIKANOFUDA) == nil)
        assert(nin.substituteTool(t.CHONOFUDA) == nil)
        assert(nin.substituteTool(0) == nil)
        assert(nin.substituteTool(9999) == nil)
    end)
end)

describe('isElementalWheelTool', function()
    it('only primary elemental tools', function()
        for _, id in ipairs({ t.UCHITAKE, t.TSURARA, t.KAWAHORI_OGI, t.MAKIBISHI, t.HIRAISHIN, t.MIZU_DEPPO }) do
            assert(nin.isElementalWheelTool(id))
        end

        for _, id in ipairs({ t.INOSHISHINOFUDA, t.SHIKANOFUDA, t.CHONOFUDA, t.SHIHEI, t.SOSHI, t.MOKUJIN }) do
            assert(not nin.isElementalWheelTool(id))
        end
    end)
end)

describe('resolveNinjaToolFromParams', function()
    it('preferred available skips substitute', function()
        local r = nin.resolveNinjaToolFromParams({
            preferred = t.SHIHEI, preferredAvailable = true, isNINMain = false,
        })
        assert(r.ok and r.toolId == t.SHIHEI and not r.usedSubstitute)
    end)

    it('non-NIN miss fails', function()
        local r = nin.resolveNinjaToolFromParams({
            preferred = t.SHIHEI, preferredAvailable = false, isNINMain = false, substituteAvailable = true,
        })
        assert(not r.ok)
    end)

    it('NIN substitute paths', function()
        local r = nin.resolveNinjaToolFromParams({
            preferred = t.SHIHEI, preferredAvailable = false, isNINMain = true, substituteAvailable = true,
        })
        assert(r.ok and r.toolId == t.SHIKANOFUDA and r.usedSubstitute)

        r = nin.resolveNinjaToolFromParams({
            preferred = t.UCHITAKE, preferredAvailable = false, isNINMain = true, substituteAvailable = true,
        })
        assert(r.ok and r.toolId == t.INOSHISHINOFUDA and r.usedSubstitute)

        r = nin.resolveNinjaToolFromParams({
            preferred = t.SHIHEI, preferredAvailable = false, isNINMain = true, substituteAvailable = false,
        })
        assert(not r.ok)

        r = nin.resolveNinjaToolFromParams({
            preferred = 9999, preferredAvailable = false, isNINMain = true, substituteAvailable = true,
        })
        assert(not r.ok)
    end)
end)

describe('expertiseChance shouldConsume consumeToolQty', function()
    it('expertise assembly and strict > compare', function()
        assert(nin.expertiseChance(10, 5) == 15)
        assert(nin.expertiseChance(0, 0) == 0)
        assert(nin.shouldConsumeTool(25, 26))
        assert(not nin.shouldConsumeTool(25, 25))
        assert(not nin.shouldConsumeTool(25, 0))
        assert(not nin.shouldConsumeTool(100, 99))
        assert(not nin.shouldConsumeTool(0, 0))
        assert(nin.shouldConsumeTool(0, 1))
    end)

    it('Futae qty and expertise qty', function()
        assert(nin.consumeToolQty(t.UCHITAKE, true, 100, 0) == 2)
        assert(nin.consumeToolQty(t.INOSHISHINOFUDA, true, 100, 0) == 0)
        assert(nin.consumeToolQty(t.INOSHISHINOFUDA, true, 0, 50) == 1)
        assert(nin.consumeToolQty(t.SHIHEI, false, 30, 31) == 1)
        assert(nin.consumeToolQty(t.SHIHEI, false, 30, 30) == 0)
    end)
end)

describe('nonPCAlwaysHasTool', function()
    it('is true', function()
        assert(nin.nonPCAlwaysHasTool())
    end)
end)
