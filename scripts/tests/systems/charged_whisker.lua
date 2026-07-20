require('scripts/actions/mobskills/charged_whisker')
describe('Charged Whisker mob skill', function()
    it('uses Thunder wipe-shadows plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/charged_whisker')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 40 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=70, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.THUNDER } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.element == xi.element.THUNDER and params.fTP[1] == 3.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(damage == 70)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
    end)
end)
