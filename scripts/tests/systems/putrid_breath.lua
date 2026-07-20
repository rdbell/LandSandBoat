require('scripts/actions/mobskills/putrid_breath')

describe('Putrid Breath mob skill', function()
    it('uses dark magical breath params and sets HIT_DMG for skill version 2', function()
        local breath = require('scripts/actions/mobskills/putrid_breath')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, skillID = nil, nil, xi.mobSkill.PUTRID_BREATH_1
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getID = function() return skillID end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        assert(breath.onMobSkillCheck(target, mob, skill) == 0 and breath.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 8 and params.element == xi.element.DARK)
        assert(params.attackType == xi.attackType.BREATH and params.primaryMessage == nil and damage == nil)
        skillID = xi.mobSkill.PUTRID_BREATH_2
        breath.onMobWeaponSkill(mob, target, skill, {})
        assert(params.primaryMessage == xi.msg.basic.HIT_DMG)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
