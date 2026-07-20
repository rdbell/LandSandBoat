require('scripts/actions/mobskills/microquake')
describe('Microquake mob skill', function()
    it('admits polearm form and uses physical plan', function()
        local skill = require('scripts/actions/mobskills/microquake')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, anim = nil, nil, 0
        local mob = { getAnimationSub = function() return anim end, getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        anim = 2
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.5 and damage == 100)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
    end)
end)
