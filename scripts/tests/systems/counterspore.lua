require('scripts/actions/mobskills/counterspore')
describe('Counterspore mob skill', function()
    it('uses Water plan with HIT_DMG primary message', function()
        local skill = require('scripts/actions/mobskills/counterspore')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.WATER } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.primaryMessage == xi.msg.basic.HIT_DMG and params.element == xi.element.WATER)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
