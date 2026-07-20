require('scripts/actions/mobskills/shackled_fists')
describe('Shackled Fists mob skill', function()
    it('requires MNK job and uses fivefold H2H plan', function()
        local fists = require('scripts/actions/mobskills/shackled_fists')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, job = nil, nil, xi.job.WAR
        local mob = {
            getMainJob = function() return job end,
            getWeaponDmg = function() return 20 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(fists.onMobSkillCheck(target, mob, {}) == 1)
        job = xi.job.MNK; assert(fists.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(fists.onMobWeaponSkill(mob, target, {}, {}) == 50)
        assert(params.numHits == 5 and params.fTP[1] == 0.5 and params.damageType == xi.damageType.HAND_TO_HAND and damage == nil)
        xi.mobskills.processDamage = function() return true end
        fists.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 50)
    end)
end)
