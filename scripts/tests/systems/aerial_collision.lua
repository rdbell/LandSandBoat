require('scripts/actions/mobskills/aerial_collision')

describe('Aerial Collision mob skill', function()
    it('uses its physical plan and applies TP-scaled Defense Down only after processing', function()
        local params, damage, defenseDown = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 2000 end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local random = math.random
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) defenseDown = { ... } end
        math.random = function(minimum, maximum)
            assert(minimum == 45 and maximum == 75)
            return 60
        end

        local collision = require('scripts/actions/mobskills/aerial_collision')
        assert(collision.onMobSkillCheck({}, {}, {}) == 0)
        assert(collision.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.NONE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and defenseDown == nil)

        xi.mobskills.processDamage = function() return true end
        assert(collision.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        math.random = random
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.NONE)
        assert(defenseDown[3] == xi.effect.DEFENSE_DOWN and defenseDown[4] == 45 and defenseDown[5] == 0 and defenseDown[6] == 60)
    end)
end)
