require('scripts/actions/mobskills/arrow_deluge')
describe('Arrow Deluge mob skill', function()
    it('admits RNG with animationSub~=1 and uses ranged plan', function()
        local skill = require('scripts/actions/mobskills/arrow_deluge')
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {
            getAnimationSub = function() return 1 end,
            getMainJob = function() return xi.job.RNG end,
            getWeaponDmg = function() return 50 end,
        }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.fTP[1] == 1.5 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(params.skipParry and params.attackType == xi.attackType.PHYSICAL and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 70)
    end)
end)
