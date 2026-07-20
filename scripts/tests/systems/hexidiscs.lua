require('scripts/actions/mobskills/hexidiscs')
describe('Hexidiscs mob skill', function()
    it('admits ball form and uses six-hit plan', function()
        local skill = require('scripts/actions/mobskills/hexidiscs')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, anim = nil, nil, 1
        local mob = { getAnimationSub = function() return anim end, getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        anim = 0
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.BLUNT } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 6 and params.attackMultiplier[1] == 1.25 and params.accuracyModifier[1] == -50)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_6)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
    end)
end)
