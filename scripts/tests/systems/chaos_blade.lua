require('scripts/actions/mobskills/chaos_blade')
describe('Chaos Blade mob skill', function()
    it('uses Dark magical plan and Curse when processed', function()
        local skill = require('scripts/actions/mobskills/chaos_blade')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=80, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.DARK } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.element == xi.element.DARK and status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(damage == 80 and status[1] == xi.effect.CURSE_I and status[2] == 25 and status[4] == 420)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
    end)
end)
