require('scripts/actions/mobskills/auto_attack_shantotto_ii')
describe('Auto Attack Shantotto II mob skill', function()
    it('uses NONE-element magical plan with HIT_DMG', function()
        local skill = require('scripts/actions/mobskills/auto_attack_shantotto_ii')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 50)
        assert(params.baseDamage == 40 and params.fTP[1] == 1.0 and params.element == xi.element.NONE)
        assert(params.damageType == xi.damageType.NONE and params.primaryMessage == xi.msg.basic.HIT_DMG)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 50)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 50)
    end)
end)
