-----------------------------------
-- Pure system tests for chocobo walk pure plan (slice 6158).
-----------------------------------

describe('chocobo walk pure plan', function()
    local energyAmount = { 25, 33, 50 }
    local energyRandomness = 5
    local eventChance = 33
    local ADULT_1 = 4

    local function walkEnergyCost(energyIdx, randomBonus)
        local base = energyAmount[energyIdx]
        if not base then return 0 end
        if randomBonus < 0 then randomBonus = 0 end
        if randomBonus > energyRandomness then randomBonus = energyRandomness end
        return base + randomBonus
    end

    local WALK_FAIL_ENERGY = 0
    local WALK_WHISTLE     = 1
    local WALK_FIND_ITEM   = 2
    local WALK_DEFAULT     = 3

    local function planWalk(p)
        local cost = walkEnergyCost(p.energyIdx, p.randomBonus)
        if p.energy < cost then
            return { path = WALK_FAIL_ENERGY, cost = cost, energy = p.energy }
        end
        local newEnergy = p.energy - cost
        if p.whistleQuestProg == 2 and p.stage >= ADULT_1 then
            return { path = WALK_WHISTLE, cost = cost, energy = newEnergy }
        end
        if p.eventRoll <= eventChance and p.heldItem == 0 then
            return { path = WALK_FIND_ITEM, cost = cost, energy = newEnergy, itemId = p.itemId }
        end
        return { path = WALK_DEFAULT, cost = cost, energy = newEnergy }
    end

    it('walk energy cost base plus bonus', function()
        assert(walkEnergyCost(1, 0) == 25)
        assert(walkEnergyCost(2, 5) == 38)
        assert(walkEnergyCost(3, 3) == 53)
        assert(walkEnergyCost(1, 99) == 30) -- clamp bonus to 5
        assert(walkEnergyCost(9, 0) == 0)
    end)

    it('energy fail before other outcomes', function()
        local r = planWalk({
            energyIdx = 1, randomBonus = 0, energy = 24,
            whistleQuestProg = 2, stage = ADULT_1, eventRoll = 1, heldItem = 0, itemId = 1,
        })
        assert(r.path == WALK_FAIL_ENERGY and r.cost == 25)
    end)

    it('whistle quest path after energy spend', function()
        local r = planWalk({
            energyIdx = 1, randomBonus = 0, energy = 25,
            whistleQuestProg = 2, stage = ADULT_1, eventRoll = 1, heldItem = 0, itemId = 1,
        })
        assert(r.path == WALK_WHISTLE and r.energy == 0)
        r = planWalk({
            energyIdx = 1, randomBonus = 0, energy = 25,
            whistleQuestProg = 2, stage = ADULT_1 - 1, eventRoll = 1, heldItem = 0, itemId = 1,
        })
        assert(r.path ~= WALK_WHISTLE)
    end)

    it('find item vs default', function()
        local r = planWalk({
            energyIdx = 2, randomBonus = 1, energy = 50,
            whistleQuestProg = 0, stage = 3, eventRoll = 33, heldItem = 0, itemId = 123,
        })
        assert(r.path == WALK_FIND_ITEM and r.itemId == 123 and r.energy == 50 - 34)
        r = planWalk({
            energyIdx = 2, randomBonus = 1, energy = 50,
            whistleQuestProg = 0, stage = 3, eventRoll = 34, heldItem = 0, itemId = 123,
        })
        assert(r.path == WALK_DEFAULT)
        r = planWalk({
            energyIdx = 2, randomBonus = 1, energy = 50,
            whistleQuestProg = 0, stage = 3, eventRoll = 1, heldItem = 99, itemId = 123,
        })
        assert(r.path == WALK_DEFAULT) -- already holding item
    end)
end)
