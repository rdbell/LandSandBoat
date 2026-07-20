require('scripts/actions/mobskills/pole_swing')

describe('Pole Swing mob skill', function()
    it('requires non-sub-1 animation and SMN/BLM job, then uses blunt physical plan', function()
        local swing = require('scripts/actions/mobskills/pole_swing')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local animSub, job, params, damage = 0, xi.job.SMN, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getMainJob = function() return job end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub, job = 1, xi.job.SMN
        assert(swing.onMobSkillCheck(target, mob, {}) == 1)
        animSub, job = 0, xi.job.WAR
        assert(swing.onMobSkillCheck(target, mob, {}) == 1)
        animSub, job = 0, xi.job.SMN
        assert(swing.onMobSkillCheck(target, mob, {}) == 0)
        animSub, job = 0, xi.job.BLM
        assert(swing.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(swing.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        swing.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
