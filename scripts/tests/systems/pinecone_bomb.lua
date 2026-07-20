require('scripts/actions/mobskills/pinecone_bomb')

describe('Pinecone Bomb mob skill', function()
    it('uses its ranged piercing plan and applies Sleep after processing', function()
        local bomb = require('scripts/actions/mobskills/pinecone_bomb')
        local move, process, status = xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            statusParams = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(bomb.onMobSkillCheck(target, mob, {}) == 0 and bomb.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1.5)
        assert(params.skipParry and params.skipGuard and params.skipBlock)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        bomb.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(statusParams[3] == xi.effect.SLEEP_I and statusParams[4] == 1 and statusParams[5] == 0 and statusParams[6] == 60)
    end)
end)
