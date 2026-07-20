require('scripts/actions/mobskills/Preternatural_Gleam')

describe('Preternatural Gleam mob skill', function()
    it('uses its light magical plan and applies Mute and Paralysis after processing', function()
        local gleam = require('scripts/actions/mobskills/Preternatural_Gleam')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, calls = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            calls[#calls + 1] = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(gleam.onMobSkillCheck(target, mob, {}) == 0 and gleam.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.5 and params.element == xi.element.LIGHT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        gleam.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123)
        assert(calls[1][3] == xi.effect.MUTE and calls[1][4] == 1 and calls[1][6] == 60)
        assert(calls[2][3] == xi.effect.PARALYSIS and calls[2][4] == 70 and calls[2][6] == 60)
    end)
end)
