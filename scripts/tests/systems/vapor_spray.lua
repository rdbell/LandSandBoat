require('scripts/actions/mobskills/vapor_spray')
describe('Vapor Spray mob skill', function()
    it('uses water breath plan with bonus 1', function()
        local skill = require('scripts/actions/mobskills/vapor_spray')
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {}
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.percentMultipier == 0.10 and params.damageCap == 500 and params.bonusDamage == 1 and params.element == xi.element.WATER and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 200)
    end)
end)
