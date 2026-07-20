require('scripts/actions/mobskills/pinecone_bomb_nm')
describe('Pinecone Bomb NM mob skill', function()
    it('uses ranged physical plan fTP 3.0 with skip defenses', function()
        local skill = require('scripts/actions/mobskills/pinecone_bomb_nm')
        local params = nil
        local origR, origD = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        xi.mobskills.mobRangedMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 90, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.fTP[1] == 3.0 and params.skipParry and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = origR, origD
    end)
end)
