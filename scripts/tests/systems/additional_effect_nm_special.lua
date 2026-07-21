-----------------------------------
-- Pure system tests for nmSpecificConfigs specialAction plans (slice 6138).
-- Source: scripts/globals/additional_effects.lua ~524–560
-----------------------------------

describe('additionalEffect Brigandish specialAction pure plan', function()
    local THRESHOLD = -10000

    local function plan(udmgphys)
        if udmgphys ~= THRESHOLD then
            return { clear = false }
        end
        return {
            clear = true,
            setUDMGPHYS = 0, setUDMGRANGE = 0, setUDMGMAGIC = 0, setUDMGBREATH = 0,
            setKillable = true,
        }
    end

    it('no-ops when not fully immune', function()
        assert(plan(0).clear == false)
        assert(plan(-5000).clear == false)
    end)

    it('clears immunities at -10000 UDMGPHYS', function()
        local r = plan(THRESHOLD)
        assert(r.clear == true and r.setKillable == true)
        assert(r.setUDMGPHYS == 0 and r.setUDMGBREATH == 0)
    end)
end)

describe('additionalEffect wind NM specialAction pure plan', function()
    local MOBMOD_ADD_EFFECT = 45

    local function plan()
        return { mobMod = MOBMOD_ADD_EFFECT, value = 0 }
    end

    it('disables ADD_EFFECT mobmod', function()
        local r = plan()
        assert(r.mobMod == 45 and r.value == 0)
    end)
end)
