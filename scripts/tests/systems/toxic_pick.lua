require('scripts/actions/mobskills/toxic_pick')
describe('Toxic Pick mob skill', function()
    it('uses piercing plan and applies poison/plague/weight after processing', function()
        local skill = require('scripts/actions/mobskills/toxic_pick')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusCalls = nil, nil, {}
        local mob = { getWeaponDmg = function() return 80 end, getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusCalls[#statusCalls + 1] = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.damageType == xi.damageType.PIERCING and #statusCalls == 0)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100)
        assert(statusCalls[1][3] == xi.effect.POISON and statusCalls[1][4] == 25 and statusCalls[1][5] == 3 and statusCalls[1][6] == 180)
        assert(statusCalls[2][3] == xi.effect.PLAGUE and statusCalls[2][4] == 5)
        assert(statusCalls[3][3] == xi.effect.WEIGHT and statusCalls[3][4] == 75 and statusCalls[3][6] == 120)
    end)
end)
