-----------------------------------
-- Pure system tests for additionalEffect attack dispatch (slice 6140).
-- Source: scripts/globals/additional_effects.lua ~618–639
-----------------------------------

describe('additionalEffect attack dispatch pure plan', function()
    local function gate(p)
        if p.itemReqLvl > p.attackerLvl then
            return { ok = false }
        end
        if p.procRoll > p.chance then
            return { ok = false }
        end
        return { ok = true, damage = p.damage, addType = p.addType }
    end

    local function dispatch(p)
        local g = gate(p)
        if not g.ok then
            return { ok = false }
        end
        if not p.knownProcType then
            return { ok = false, unknownType = true }
        end
        return { ok = true, damage = g.damage, addType = g.addType }
    end

    it('blocks level sync and chance miss', function()
        assert(dispatch({
            itemReqLvl = 75, attackerLvl = 50, procRoll = 1, chance = 100,
            damage = 10, addType = 1, knownProcType = true,
        }).ok == false)
        assert(dispatch({
            itemReqLvl = 1, attackerLvl = 99, procRoll = 21, chance = 20,
            damage = 10, addType = 1, knownProcType = true,
        }).ok == false)
    end)

    it('flags unknown proc type', function()
        local r = dispatch({
            itemReqLvl = 1, attackerLvl = 99, procRoll = 1, chance = 100,
            damage = 10, addType = 99, knownProcType = false,
        })
        assert(r.ok == false and r.unknownType == true)
    end)

    it('dispatches when gates pass', function()
        local r = dispatch({
            itemReqLvl = 1, attackerLvl = 99, procRoll = 1, chance = 100,
            damage = 25, addType = 1, knownProcType = true,
        })
        assert(r.ok == true and r.damage == 25 and r.addType == 1)
    end)
end)
