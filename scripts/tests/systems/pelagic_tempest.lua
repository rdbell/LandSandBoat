require('scripts/actions/mobskills/pelagic_tempest')

describe('Pelagic Tempest mob skill', function()
    it('uses its blunt physical plan and applies Shock and Terror after processing', function()
        local tempest = require('scripts/actions/mobskills/pelagic_tempest')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusCalls = nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            statusCalls[#statusCalls + 1] = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(tempest.onMobSkillCheck(target, mob, {}) == 0 and tempest.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and #statusCalls == 0)
        xi.mobskills.processDamage = function() return true end
        tempest.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
        assert(statusCalls[1][3] == xi.effect.SHOCK and statusCalls[1][4] == 28 and statusCalls[1][5] == 3 and statusCalls[1][6] == 180)
        assert(statusCalls[2][3] == xi.effect.TERROR and statusCalls[2][4] == 1 and statusCalls[2][5] == 0 and statusCalls[2][6] == 180)
    end)
end)
