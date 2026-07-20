require('scripts/actions/mobskills/earth_breath')

describe('Earth Breath mob skill', function()
    it('requires a target in front and uses Earth breath parameters', function()
        local breath = require('scripts/actions/mobskills/earth_breath')
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {}
        local target = {
            inFront = false,
            isInfront = function(self, _, angle) return self.inFront and angle == 128 end,
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobBreathMove = function(_, _, _, _, p)
            params = p
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return true end

        assert(breath.onMobSkillCheck(target, mob, {}) == 1)
        target.inFront = true
        assert(breath.onMobSkillCheck(target, mob, {}) == 0)
        assert(breath.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process

        assert(params.percentMultipier == 0.10 and params.damageCap == 500 and params.bonusDamage == 100)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.EARTH)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.EARTH)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.EARTH)
    end)
end)
