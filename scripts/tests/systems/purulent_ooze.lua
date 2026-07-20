require('scripts/actions/mobskills/purulent_ooze')

describe('Purulent Ooze mob skill', function()
    it('uses its water magical plan and applies Bio and Max HP Down after processing', function()
        local ooze = require('scripts/actions/mobskills/purulent_ooze')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, calls = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            calls[#calls + 1] = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(ooze.onMobSkillCheck(target, mob, {}) == 0 and ooze.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.element == xi.element.WATER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        ooze.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123)
        assert(calls[1][3] == xi.effect.BIO and calls[1][4] == 12 and calls[1][5] == 3 and calls[1][6] == 120 and calls[1][8] == 10)
        assert(calls[2][3] == xi.effect.MAX_HP_DOWN and calls[2][4] == 10 and calls[2][6] == 120)
    end)
end)
