require('scripts/actions/mobskills/cronos_sling_eta')
describe('Cronos Sling Eta mob skill', function()
    it('admits animationSub 0 and uses physical plan', function()
        local skill = require('scripts/actions/mobskills/cronos_sling_eta')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local anim = 1
        local mob = {
            getAnimationSub = function() return anim end,
            getWeaponDmg = function() return 50 end,
        }
        local target = { takeDamage = function(_, v) damage = v end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        anim = 0
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.25 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
