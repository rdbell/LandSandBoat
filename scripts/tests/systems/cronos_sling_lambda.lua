require('scripts/actions/mobskills/cronos_sling_lambda')
describe('Cronos Sling Lambda mob skill', function()
    it('admits animationSub 2 and uses physical plan', function()
        local skill = require('scripts/actions/mobskills/cronos_sling_lambda')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local anim = 0
        local mob = {
            getAnimationSub = function() return anim end,
            getWeaponDmg = function() return 50 end,
        }
        local target = { takeDamage = function(_, v) damage = v end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        anim = 2
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.75 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
