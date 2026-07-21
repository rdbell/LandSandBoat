-----------------------------------
-- Pure system tests for chocobo dig start gates / loot (slice 6146).
-----------------------------------

describe('chocobo dig start pure plan', function()
    local GATE_COOLDOWN = 0
    local GATE_AMK      = 1
    local GATE_FATIGUE  = 2
    local GATE_POSITION = 3
    local GATE_PROCEED  = 4

    local function planGate(cooldownsOK, amkActive, fatigueBlocks, positionTooClose)
        if not cooldownsOK then
            return GATE_COOLDOWN
        end
        if amkActive then
            return GATE_AMK
        end
        if fatigueBlocks then
            return GATE_FATIGUE
        end
        if positionTooClose then
            return GATE_POSITION
        end
        return GATE_PROCEED
    end

    local function bypassFatigue(roll, bypassMod)
        return roll <= bypassMod
    end

    local function layerGrant(setting)
        return setting > 0
    end

    local function planLoot(treasure, regular, burrow, bore)
        if treasure > 0 then
            return {
                treasureHit  = true,
                anyItem      = true,
                applyFatigue = true,
                findNothing  = false,
                skillUp      = true,
                roeSuccess   = true,
            }
        end
        local any = regular > 0 or burrow > 0 or bore > 0
        return {
            treasureHit  = false,
            anyItem      = any,
            applyFatigue = any,
            findNothing  = not any,
            skillUp      = true,
            roeSuccess   = any,
        }
    end

    local function posSign(v)
        return v < 0 and 2 or 0
    end

    local function posRestore(abs, signVar)
        return abs * (1 - signVar)
    end

    it('early gates ordered cooldown amk fatigue position', function()
        assert(planGate(false, true, true, true) == GATE_COOLDOWN)
        assert(planGate(true, true, true, true) == GATE_AMK)
        assert(planGate(true, false, true, true) == GATE_FATIGUE)
        assert(planGate(true, false, false, true) == GATE_POSITION)
        assert(planGate(true, false, false, false) == GATE_PROCEED)
    end)

    it('bypass fatigue and layer grants', function()
        assert(bypassFatigue(50, 50) == true)
        assert(bypassFatigue(51, 50) == false)
        assert(bypassFatigue(1, 0) == false)
        assert(layerGrant(1) == true and layerGrant(0) == false)
    end)

    it('treasure path short-circuits loot plan', function()
        local r = planLoot(99, 1, 1, 1)
        assert(r.treasureHit and r.anyItem and r.applyFatigue and r.skillUp and r.roeSuccess)
        assert(not r.findNothing)
    end)

    it('regular path finds nothing or fatigue', function()
        local miss = planLoot(0, 0, 0, 0)
        assert(not miss.anyItem and miss.findNothing and miss.skillUp and not miss.applyFatigue)
        local hit = planLoot(0, 10, 0, 0)
        assert(hit.anyItem and hit.applyFatigue and hit.roeSuccess and not hit.findNothing)
        local burrow = planLoot(0, 0, 5, 0)
        assert(burrow.anyItem)
        local bore = planLoot(0, 0, 0, 7)
        assert(bore.anyItem)
    end)

    it('position sign pack and restore', function()
        assert(posSign(-3.5) == 2 and posSign(3.5) == 0 and posSign(0) == 0)
        assert(posRestore(3.5, 0) == 3.5)
        assert(posRestore(3.5, 2) == -3.5)
    end)
end)
