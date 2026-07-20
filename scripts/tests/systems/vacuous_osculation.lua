require('scripts/actions/mobskills/vacuous_osculation')
describe('Vacuous Osculation mob skill', function()
    it('uses none magical plan and applies plague then poison after processing', function()
        local skill = require('scripts/actions/mobskills/vacuous_osculation')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, calls = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) calls[#calls + 1] = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.element == xi.element.NONE and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100)
        assert(calls[1][3] == xi.effect.PLAGUE and calls[1][4] == 5 and calls[1][5] == 3 and calls[1][6] == 30)
        assert(calls[2][3] == xi.effect.POISON and calls[2][4] == 8 and calls[2][5] == 3 and calls[2][6] == 60)
    end)
end)
