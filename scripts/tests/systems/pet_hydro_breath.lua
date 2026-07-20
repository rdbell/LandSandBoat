require('scripts/actions/mobskills/pet_hydro_breath')

describe('Pet Hydro Breath mob skill', function()
    it('uses its water breath plan and damages only after processing', function()
        local breath = require('scripts/actions/mobskills/pet_hydro_breath')
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        assert(breath.onMobSkillCheck(target, {}, {}) == 0 and breath.onMobWeaponSkill({}, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.13 and params.damageCap == 400 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.resistStat == xi.mod.INT)
        assert(params.element == xi.element.WATER and params.attackType == xi.attackType.BREATH)
        assert(params.damageType == xi.damageType.WATER)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill({}, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.WATER)
    end)
end)
