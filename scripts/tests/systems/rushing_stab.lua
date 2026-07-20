require('scripts/actions/mobskills/rushing_stab')
describe('Rushing Stab mob skill', function()
    it('requires DRG form and uses fourfold piercing plan', function()
        local stab = require('scripts/actions/mobskills/rushing_stab')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local animSub, job, params, damage = 0, xi.job.DRG, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getMainJob = function() return job end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub = 1; assert(stab.onMobSkillCheck(target, mob, {}) == 1)
        animSub, job = 0, xi.job.WAR; assert(stab.onMobSkillCheck(target, mob, {}) == 1)
        animSub, job = 0, xi.job.DRG; assert(stab.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(stab.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 4 and params.damageType == xi.damageType.PIERCING and damage == nil)
        xi.mobskills.processDamage = function() return true end
        stab.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
