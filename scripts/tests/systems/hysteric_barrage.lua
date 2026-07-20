require('scripts/actions/mobskills/hysteric_barrage')
describe('Hysteric Barrage mob skill', function()
    it('admits broken weapon and uses five-hit crit plan', function()
        local skill = require('scripts/actions/mobskills/hysteric_barrage')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, anim = nil, nil, 0
        local mob = { getAnimationSub = function() return anim end, getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        anim = 1
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.HAND_TO_HAND } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 5 and params.fTP[1] == 0.667 and params.canCrit)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
    end)
end)
