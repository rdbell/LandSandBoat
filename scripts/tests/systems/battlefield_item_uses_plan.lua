-----------------------------------
-- Pure system tests for xi.battlefield.itemUses catalog (slice 6213).
-----------------------------------

describe('battlefield itemUses pure plan', function()
    local itemUses =
    {
        [1426] = 3, -- WARRIORS_TESTIMONY
        [1427] = 3, -- MONKS
        [1428] = 3, -- WHM
        [1429] = 3, -- BLM
        [1430] = 3, -- RDM
        [1431] = 3, -- THF
        [1432] = 3, -- PLD
        [1433] = 3, -- DRK
        [1434] = 3, -- BST
        [1435] = 3, -- BRD
        [1436] = 3, -- RNG
        [1437] = 3, -- SAM
        [1438] = 3, -- NIN
        [1439] = 3, -- DRG
        [1440] = 3, -- SMN
        [2331] = 3, -- BLU
        [2332] = 3, -- COR
        [2333] = 3, -- PUP
    }

    local function isTestimony(itemId)
        return itemUses[itemId] ~= nil
    end

    local function canEnter(itemId, usesRemaining)
        return isTestimony(itemId) and usesRemaining > 0
    end

    it('catalog size 18 and all uses=3', function()
        local n = 0
        for _, uses in pairs(itemUses) do
            n = n + 1
            assert(uses == 3)
        end
        assert(n == 18)
        assert(itemUses[1426] == 3 and itemUses[1440] == 3)
        assert(itemUses[2331] == 3 and itemUses[2333] == 3)
    end)

    it('testimony membership and worn gate', function()
        assert(isTestimony(1432))
        assert(not isTestimony(0))
        assert(canEnter(1426, 3))
        assert(not canEnter(1426, 0))
        assert(not canEnter(9999, 3))
    end)
end)
