require('scripts/actions/mobskills/self-destruct_cluster_razon')
describe('Self-Destruct Cluster Razon mob skill', function()
    it('uses fire breath fTP 2.0 base mobHP', function()
        local skill = require('scripts/actions/mobskills/self-destruct_cluster_razon')
        local params = nil
        local origM, origD = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local sk = { getMobHP = function() return 800 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 100)
        assert(params.baseDamage == 800 and params.fTP[1] == 2.0)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = origM, origD
    end)
end)
