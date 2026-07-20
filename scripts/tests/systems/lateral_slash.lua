require('scripts/actions/mobskills/lateral_slash')

describe('Lateral Slash mob skill', function()
    it('uses a physical Slashing plan and applies Defense Down only after processing succeeds', function()
        local lateralSlash = require('scripts/actions/mobskills/lateral_slash')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) effect = { ... } end

        assert(lateralSlash.onMobSkillCheck(target, mob, {}) == 0)
        assert(lateralSlash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 1.5 and params.fTP[2] == 1.5 and params.fTP[3] == 1.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and effect == nil)

        xi.mobskills.processDamage = function() return true end
        assert(lateralSlash.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(effect[1] == mob and effect[2] == target and effect[3] == xi.effect.DEFENSE_DOWN)
        assert(effect[4] == 83 and effect[5] == 0 and effect[6] == 30)
    end)
end)
