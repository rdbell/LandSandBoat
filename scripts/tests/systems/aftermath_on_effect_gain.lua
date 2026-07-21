-----------------------------------
-- Pure system tests for aftermath.onEffectGain plan (slice 6135).
-- Source: scripts/globals/aftermath.lua ~628–669
-----------------------------------

describe('aftermath onEffectGain pure plan', function()
    local TYPE_RELIC = 1
    local TYPE_MYTHIC = 2
    local TYPE_EMPY = 3
    local GUTTLER_T2 = 19

    local function includePets(id)
        return id == GUTTLER_T2
    end

    -- Minimal catalog stubs for plan shape (full tables in Go).
    local function relicMods(id)
        if id == 1 then return { { 289, 10 } } end -- SUBTLE_BLOW
        if id == GUTTLER_T2 then return { { 62, 10 } } end
        if id >= 1 and id <= 28 then return { { 1, 1 } } end
        return nil
    end

    local function mythicMods(id, tp)
        if id < 29 or id > 43 then return nil end
        local lvl = math.floor(tp / 1000)
        if lvl < 1 or lvl > 3 then return nil end
        return { { 25, math.floor(tp / 100) } } -- ACC-shaped stub
    end

    local function empyMods(id, tp)
        if id ~= 44 and id ~= 45 then return nil end
        local lvl = math.floor(tp / 1000)
        local powers = { [1] = 300, [2] = 400, [3] = 500 }
        return { { 863, powers[lvl] } }
    end

    local function plan(p)
        if p.tier == TYPE_RELIC then
            local mods = relicMods(p.power)
            if not mods then return { ok = false } end
            local r = { ok = true, mods = mods, copyPet = false }
            if p.hasPet and includePets(p.power) then
                r.copyPet = true
                r.petDurationSec = math.floor((p.durationMs or 0) / 1000)
                r.petNoLoss = true
            end
            return r
        end
        if p.tier == TYPE_MYTHIC then
            local mods = mythicMods(p.power, p.subPower or 0)
            if not mods then return { ok = false } end
            local r = { ok = true, mods = mods, copyPet = false }
            if p.hasPet then
                r.copyPet = true
                r.petDurationSec = math.floor((p.durationMs or 0) / 1000)
                r.petNoLoss = true
            end
            return r
        end
        if p.tier == TYPE_EMPY then
            local mods = empyMods(p.power, p.subPower or 0)
            if not mods then return { ok = false } end
            return { ok = true, mods = mods, copyPet = false }
        end
        return { ok = false }
    end

    it('relic applies mods without pet unless includePets', function()
        local r = plan({ power = 1, tier = TYPE_RELIC, hasPet = true, durationMs = 40000 })
        assert(r.ok == true and r.mods[1][2] == 10 and r.copyPet == false)
        local g = plan({ power = GUTTLER_T2, tier = TYPE_RELIC, hasPet = true, durationMs = 90000 })
        assert(g.copyPet == true and g.petDurationSec == 90 and g.petNoLoss == true)
    end)

    it('mythic always copies pet when present', function()
        local r = plan({ power = 29, tier = TYPE_MYTHIC, subPower = 1000, hasPet = true, durationMs = 60000 })
        assert(r.ok == true and r.copyPet == true and r.petDurationSec == 60)
        local n = plan({ power = 29, tier = TYPE_MYTHIC, subPower = 1000, hasPet = false, durationMs = 60000 })
        assert(n.copyPet == false)
    end)

    it('empyrean never copies pet', function()
        local r = plan({ power = 44, tier = TYPE_EMPY, subPower = 2000, hasPet = true, durationMs = 60000 })
        assert(r.ok == true and r.copyPet == false and r.mods[1][2] == 400)
    end)

    it('rejects unknown ids', function()
        assert(plan({ power = 99, tier = TYPE_RELIC }).ok == false)
        assert(plan({ power = 1, tier = TYPE_MYTHIC, subPower = 1000 }).ok == false)
    end)
end)
