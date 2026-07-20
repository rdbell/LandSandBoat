require('scripts/actions/mobskills/rushing_drub')
describe('Rushing Drub mob skill', function()
    it('requires staff form BLM/WHM and uses fourfold blunt plan', function()
        local drub = require('scripts/actions/mobskills/rushing_drub')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local animSub, job, params, damage = 0, xi.job.BLM, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getMainJob = function() return job end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub, job = 1, xi.job.BLM; assert(drub.onMobSkillCheck(target, mob, {}) == 1)
        animSub, job = 0, xi.job.WAR; assert(drub.onMobSkillCheck(target, mob, {}) == 1)
        animSub, job = 0, xi.job.BLM; assert(drub.onMobSkillCheck(target, mob, {}) == 0)
        animSub, job = 0, xi.job.WHM; assert(drub.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(drub.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 4 and params.fTP[1] == 0.75 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        drub.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
