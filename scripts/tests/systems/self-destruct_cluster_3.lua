require('scripts/actions/mobskills/self-destruct_cluster_3')
describe('Self-Destruct Cluster 3 mob skill', function()
    it('uses fire breath fTP 0.33 with final animation sub 5', function()
        local skill = require('scripts/actions/mobskills/self-destruct_cluster_3')
        local params, anim = nil, nil
        local origM, origD = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getPool = function() return 0 end }
        local sk = { getMobHP = function() return 300 end, setFinalAnimationSub = function(_, a) anim = a end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.fTP[1] == 0.33 and anim == 5)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = origM, origD
    end)
end)
