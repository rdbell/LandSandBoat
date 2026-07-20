require('scripts/actions/mobskills/rushing_slash')
describe('Rushing Slash mob skill', function()
    it('requires BST form and uses fourfold slashing plan', function()
        local slash = require('scripts/actions/mobskills/rushing_slash')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local animSub, job, params, damage = 0, xi.job.BST, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getMainJob = function() return job end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub = 1; assert(slash.onMobSkillCheck(target, mob, {}) == 1)
        animSub, job = 0, xi.job.WAR; assert(slash.onMobSkillCheck(target, mob, {}) == 1)
        animSub, job = 0, xi.job.BST; assert(slash.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(slash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 4 and params.fTP[1] == 1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        slash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
