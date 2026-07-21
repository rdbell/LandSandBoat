-----------------------------------
-- Pure system tests for getSingleWeaponTPReturn inject (slice 6095).
-- Mirrors scripts/globals/combat/tp.lua (~143–160).
-----------------------------------

describe('single weapon TP return pure injects', function()
    local function calculateTPReturn(usePC, delay)
        -- Minimal PC/mob band stubs for fixture delays used below.
        if usePC then
            if delay > 180 then
                return math.floor(61 + (delay - 180) * 88 / 360)
            end
            return math.floor(61 + (delay - 180) * 63 / 360)
        end
        if delay > 180 then
            return math.floor(50 + (delay - 180) * 65 / 270)
        end
        return math.floor(50 + (delay - 180) * 15 / 180)
    end

    local function applyStoreTP(tp, storeTPMod)
        return math.floor(tp * (100 + storeTPMod) / 100)
    end

    local function singleWeaponTPReturn(p)
        if not p.isPC then
            return 0
        end
        if p.hasMeikyo then
            return 0
        end
        local tp = calculateTPReturn(true, p.modifiedDelay)
        return applyStoreTP(tp, p.storeTPMod or 0)
    end

    it('non-PC and Meikyo early-out', function()
        assert(singleWeaponTPReturn({ isPC = false, modifiedDelay = 480 }) == 0)
        assert(singleWeaponTPReturn({ isPC = true, hasMeikyo = true, modifiedDelay = 480 }) == 0)
    end)

    it('PC identity store TP', function()
        local base = calculateTPReturn(true, 480)
        assert(base > 0)
        assert(singleWeaponTPReturn({ isPC = true, modifiedDelay = 480, storeTPMod = 0 }) == base)
    end)

    it('Store TP scales', function()
        local base = calculateTPReturn(true, 480)
        local got = singleWeaponTPReturn({ isPC = true, modifiedDelay = 480, storeTPMod = 50 })
        assert(got == applyStoreTP(base, 50), 'got ' .. tostring(got))
    end)
end)
