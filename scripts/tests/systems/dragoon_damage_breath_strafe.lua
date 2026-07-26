require('scripts/globals/job_utils/dragoon')

describe('Dragoon Damage Breath enhanced-Strafe follow-up', function()
    it('grants five TP per Strafe Effect merit', function()
        local oldMagicBurst = xi.magicburst.formMagicBurst
        local oldResist     = xi.combat.magicHitRate.calculateResistRate
        local oldSDT        = xi.combat.damage.magicalElementSDT
        local oldAbsorb     = xi.spells.damage.calculateAbsorption
        local oldNullify    = xi.spells.damage.calculateNullification
        local oldAdjust     = xi.ability.adjustDamage
        local wyvernTP

        xi.magicburst.formMagicBurst = function() return nil, 0 end
        xi.combat.magicHitRate.calculateResistRate = function() return 1 end
        xi.combat.damage.magicalElementSDT = function() return 1 end
        xi.spells.damage.calculateAbsorption = function() return 1 end
        xi.spells.damage.calculateNullification = function() return 1 end
        xi.ability.adjustDamage = function(damage) return damage end

        local master = {
            getMerit = function(_, merit)
                if merit == xi.merit.STRAFE_EFFECT then return 2 end
                return 0
            end,
            getJobPointLevel = function() return 0 end,
            getMod = function(_, mod)
                if mod == xi.mod.ENHANCES_STRAFE then return 1 end
                return 0
            end,
        }
        local wyvern = {
            getMaster = function() return master end,
            hasStatusEffect = function() return false end,
            getHP = function() return 600 end,
            addTP = function(_, amount) wyvernTP = amount end,
        }
        local target = {
            getID = function() return 9 end,
            takeDamage = function() end,
        }
        local action = {
            recordDamage = function() end,
            messageID = function() end,
        }

        assert(xi.job_utils.dragoon.useDamageBreath(wyvern, target, {}, action, xi.damageType.ELEMENTAL) == 115)
        assert(wyvernTP == 10)

        xi.magicburst.formMagicBurst = oldMagicBurst
        xi.combat.magicHitRate.calculateResistRate = oldResist
        xi.combat.damage.magicalElementSDT = oldSDT
        xi.spells.damage.calculateAbsorption = oldAbsorb
        xi.spells.damage.calculateNullification = oldNullify
        xi.ability.adjustDamage = oldAdjust
    end)
end)
