require('scripts/actions/mobskills/self-destruct_cluster_2_death')
describe('Self-Destruct Cluster 2 Death mob skill', function()
    it('uses fire breath plan fTP 1.0 IGNORE_SHADOWS', function()
        local skill = require('scripts/actions/mobskills/self-destruct_cluster_2_death')
        local params = nil
        local origM, origD, origR = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, math.random
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = p.baseDamage, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        math.random = function(a, b) return 1.0 end
        local mob = { getHP = function() return 500 end }
        local target = { getMaxHP = function() return 1000 end, takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 500)
        assert(params.element == xi.element.FIRE and params.fTP[1] == 1.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, math.random = origM, origD, origR
    end)
end)
