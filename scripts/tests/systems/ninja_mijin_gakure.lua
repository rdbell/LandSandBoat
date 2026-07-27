require('scripts/globals/job_utils/ninja')

describe('Ninja Mijin Gakure', function()
    it('deals adjusted damage and consumes the player', function()
        local oldResist = xi.combat.magicHitRate.calculateResistRate
        local oldAdjustment = xi.combat.damage.calculateDamageAdjustment
        local oldStoneskin = utils.handleStoneskin
        local damage, localVar, hp
        local player = {
            getHP = function() return 1000 end,
            getJobPointLevel = function() return 10 end,
            setLocalVar = function(_, key, value) localVar = { key, value } end,
            setHP = function(_, value) hp = value end,
        }
        local target = {
            takeDamage = function(_, amount, source, attackType, damageType)
                damage = { amount, source, attackType, damageType }
            end,
        }

        xi.combat.magicHitRate.calculateResistRate = function() return 1 end
        xi.combat.damage.calculateDamageAdjustment = function() return 1 end
        utils.handleStoneskin = function(_, amount) return amount - 40 end

        local result = xi.job_utils.ninja.useMijinGakure(player, target, {}, {})

        xi.combat.magicHitRate.calculateResistRate = oldResist
        xi.combat.damage.calculateDamageAdjustment = oldAdjustment
        utils.handleStoneskin = oldStoneskin

        assert(result == 1000 and damage[1] == 1000 and damage[2] == player)
        assert(damage[3] == xi.attackType.SPECIAL and damage[4] == xi.damageType.ELEMENTAL)
        assert(localVar[1] == 'MijinGakure' and localVar[2] == 1 and hp == 0)
    end)
end)
