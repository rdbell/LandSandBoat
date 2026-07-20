require('scripts/actions/mobskills/warped_wail')
describe('Warped Wail mob skill', function()
    it('uses wind magical plan and max HP/MP down after processing', function()
        local skill = require('scripts/actions/mobskills/warped_wail')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, calls = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.mobStatusEffectMove = function(...) calls[#calls + 1] = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.00 and params.element == xi.element.WIND and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100)
        assert(calls[1][3] == xi.effect.MAX_HP_DOWN and calls[1][4] == 50 and calls[1][6] == 300)
        assert(calls[2][3] == xi.effect.MAX_MP_DOWN and calls[2][4] == 50)
    end)
end)
