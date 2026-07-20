require('scripts/actions/mobskills/miasma')
describe('Miasma mob skill', function()
    it('uses Earth plan and applies Plague Poison Slow when processed', function()
        local skill = require('scripts/actions/mobskills/miasma')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statuses = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.EARTH } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) statuses[#statuses+1]={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.element == xi.element.EARTH and #statuses == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        assert(statuses[1][1] == xi.effect.PLAGUE and statuses[2][1] == xi.effect.POISON and statuses[2][2] == 16)
        assert(statuses[3][1] == xi.effect.SLOW and statuses[3][2] == 1250 and statuses[3][4] == 120)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
    end)
end)
