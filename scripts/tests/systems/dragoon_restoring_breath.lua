require('scripts/globals/job_utils/dragoon')

describe('Dragoon Restoring Breath host', function()
    it('uses the highest missing-HP alive party member in wyvern range and modifies recast', function()
        local usedAbility, usedTarget, recast
        local wyvern = {
            checkDistance = function(_, target) return target.distance end,
            usePetAbility = function(_, ability, target) usedAbility, usedTarget = ability, target end,
        }
        local function member(hp, maxHP, distance, dead)
            return {
                getHP = function() return hp end,
                getMaxHP = function() return maxHP end,
                isDead = function() return dead end,
                distance = distance,
            }
        end
        local low = member(50, 100, 10, false)
        local best = member(20, 100, 12, false)
        local outOfRange = member(0, 100, 15, false)
        local dead = member(0, 100, 5, true)
        local player = {
            getPet = function() return wyvern end,
            getMainLvl = function() return 80 end,
            getPartyWithTrusts = function() return { low, best, outOfRange, dead } end,
            getMod = function() return 15 end,
        }
        local ability = { getRecast = function() return 60 end }
        local action = { setRecast = function(_, value) recast = value end }

        xi.job_utils.dragoon.useRestoringBreath(player, ability, action)

        assert(usedAbility == xi.jobAbility.HEALING_BREATH_IV)
        assert(usedTarget == best)
        assert(recast == 45)
    end)
end)
