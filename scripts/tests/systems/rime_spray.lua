require('scripts/actions/mobskills/rime_spray')
describe('Rime Spray mob skill', function()
    it('uses ice magical plan and applies Frost plus all stat downs after processing', function()
        local spray = require('scripts/actions/mobskills/rime_spray')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, calls = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) calls[#calls+1] = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(spray.onMobSkillCheck(target, mob, {}) == 0 and spray.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.element == xi.element.ICE and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        spray.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and #calls == 8)
        assert(calls[1][3] == xi.effect.FROST and calls[1][4] == 15)
        assert(calls[8][3] == xi.effect.CHR_DOWN and calls[8][4] == 20)
    end)
end)
