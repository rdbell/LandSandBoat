require('scripts/globals/job_utils/dragoon')

describe('Dragoon Healing Breath follow-ups', function()
    it('also heals the wyvern with Spirit Bond and grants enhanced-Strafe TP', function()
        local wyvernHeal
        local wyvernTP
        local additionalTarget
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
            hasStatusEffect = function(_, effect) return effect == xi.effect.SPIRIT_BOND end,
        }
        local wyvern = {
            getMaster = function() return master end,
            hasStatusEffect = function() return false end,
            getMaxHP = function() return 2560 end,
            addHP = function(_, amount) wyvernHeal = amount; return amount end,
            addTP = function(_, amount) wyvernTP = amount end,
            getID = function() return 7 end,
        }
        local target = { addHP = function(_, amount) return amount end, getID = function() return 9 end }
        local skill = { getID = function() return xi.jobAbility.HEALING_BREATH end, setMsg = function() end }
        local action = {
            addAdditionalTarget = function(_, id) additionalTarget = id end,
            setAnimation = function() end,
            getAnimation = function() return 0 end,
            messageID = function() end,
            param = function() end,
        }

        assert(xi.job_utils.dragoon.useHealingBreath(wyvern, target, skill, action) == 358)
        assert(wyvernHeal == 358 and additionalTarget == 7)
        assert(wyvernTP == 100)
    end)
end)
