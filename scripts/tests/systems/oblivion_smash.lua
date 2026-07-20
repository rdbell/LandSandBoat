require('scripts/actions/mobskills/oblivion_smash')

describe('Oblivion Smash mob skill', function()
    it('uses its three-hit Slashing plan and applies four enfeebles only after processing', function()
        local smash = require('scripts/actions/mobskills/oblivion_smash')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) table.insert(effects, { ... }) end
        assert(smash.onMobSkillCheck(target, mob, {}) == 0 and smash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3 and params.fTP[1] == 2.5 and params.fTP[2] == 2.5 and params.fTP[3] == 2.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        smash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(effects[1][3] == xi.effect.BLINDNESS and effects[1][4] == 20 and effects[1][6] == 120)
        assert(effects[2][3] == xi.effect.SILENCE and effects[2][6] == 120)
        assert(effects[3][3] == xi.effect.BIND and effects[3][6] == 120)
        assert(effects[4][3] == xi.effect.WEIGHT and effects[4][4] == 50 and effects[4][6] == 120)
    end)
end)
