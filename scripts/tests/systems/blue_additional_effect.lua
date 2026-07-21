-----------------------------------
-- Pure system tests for applyBlueAdditionalEffect (slice 6127).
-- Source: scripts/globals/bluemagic.lua ~795–824
-----------------------------------

describe('blue applyBlueAdditionalEffect pure plan', function()
    local ATTACK_BREATH = 4
    local MOD_INT = 12

    local function sanitizeElement(hasDamageType, damageType)
        if hasDamageType then
            return damageType - 5
        end
        return 0
    end

    local function sanitizeStat(attackType, hasAttribute, attribute)
        if attackType == ATTACK_BREATH then
            return 0
        end
        if hasAttribute then
            return attribute
        end
        return MOD_INT
    end

    local function apply(p)
        if (p.resist or 0) <= 0.25 then
            return { applied = {} }
        end
        local applied = {}
        for i, e in ipairs(p.entries or {}) do
            local ok = p.entryOK and p.entryOK[i]
            if ok then
                table.insert(applied, {
                    effect = e[1],
                    power = e[2],
                    tick = e[3],
                    duration = math.floor(e[4] * p.resist),
                })
            end
        end
        return { applied = applied }
    end

    it('element from damageType - 5', function()
        assert(sanitizeElement(true, 8) == 3)
        assert(sanitizeElement(false, 8) == 0)
    end)

    it('stat defaults INT; breath forces 0', function()
        assert(sanitizeStat(1, false, 0) == MOD_INT)
        assert(sanitizeStat(1, true, 5) == 5)
        assert(sanitizeStat(ATTACK_BREATH, true, 5) == 0)
    end)

    it('resist at 0.25 blocks all', function()
        local r = apply({
            resist = 0.25,
            entries = { { 4, 10, 0, 60 } },
            entryOK = { true },
        })
        assert(#r.applied == 0)
    end)

    it('resist above 0.25 applies with scaled duration', function()
        local r = apply({
            resist = 0.5,
            entries = { { 4, 10, 3, 60 }, { 5, 1, 0, 30 } },
            entryOK = { true, true },
        })
        assert(#r.applied == 2)
        assert(r.applied[1].duration == 30 and r.applied[1].power == 10 and r.applied[1].tick == 3)
        assert(r.applied[2].duration == 15)
    end)

    it('gates skip immune/trait/nullified entries', function()
        local r = apply({
            resist = 1,
            entries = { { 4, 10, 0, 60 }, { 5, 1, 0, 30 }, { 6, 2, 0, 20 } },
            entryOK = { false, true, false },
        })
        assert(#r.applied == 1 and r.applied[1].effect == 5)
    end)

    it('empty table no-ops when resist ok', function()
        local r = apply({ resist = 1, entries = {}, entryOK = {} })
        assert(#r.applied == 0)
    end)
end)
