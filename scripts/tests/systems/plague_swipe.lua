require('scripts/actions/mobskills/plague_swipe')

describe('Plague Swipe mob skill', function()
    it('requires target behind mob and applies Bio and Plague after processing', function()
        local swipe = require('scripts/actions/mobskills/plague_swipe')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local behind, params, damage, calls = true, nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end }
        local target = {
            isBehind = function() return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        behind = false
        assert(swipe.onMobSkillCheck(target, mob, {}) == 1)
        behind = true
        assert(swipe.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            calls[#calls + 1] = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(swipe.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 3 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4)
        assert(damage == nil and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        swipe.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123)
        assert(calls[1][3] == xi.effect.BIO and calls[1][4] == 7 and calls[1][5] == 3 and calls[1][6] == 60 and calls[1][8] == 15)
        assert(calls[2][3] == xi.effect.PLAGUE and calls[2][4] == 5 and calls[2][5] == 3 and calls[2][6] == 60)
    end)
end)
