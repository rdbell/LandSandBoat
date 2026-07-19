require('scripts/actions/mobskills/crescent_fang')

describe('Crescent Fang mob skill', function()
    it('uses fixed physical parameters and applies damage and Paralysis only after processing succeeds', function()
        local crescentFang = require('scripts/actions/mobskills/crescent_fang')
        local originalPhysicalMove = xi.mobskills.mobPhysicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local originalStatusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(source, recipient, effectID, power, tick, duration)
            effect = { source, recipient, effectID, power, tick, duration }
        end

        assert(crescentFang.onMobSkillCheck(target, mob, skill) == 0)
        assert(crescentFang.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 77 and params.numHits == 2)
        assert(params.fTP[1] == 2.5 and params.fTP[2] == 2.5 and params.fTP[3] == 2.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        assert(damage == nil and effect == nil)

        xi.mobskills.processDamage = function() return true end
        assert(crescentFang.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobPhysicalMove = originalPhysicalMove
        xi.mobskills.processDamage = originalProcessDamage
        xi.mobskills.mobStatusEffectMove = originalStatusEffectMove

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(effect[1] == mob and effect[2] == target and effect[3] == xi.effect.PARALYSIS)
        assert(effect[4] == 50 and effect[5] == 0 and effect[6] == 90)
    end)
end)
