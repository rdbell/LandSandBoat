require('scripts/actions/mobskills/plague_breath')
describe('Plague Breath mob skill', function()
    it('uses water breath plan and applies level-scaled Poison after processing', function()
        local breath = require('scripts/actions/mobskills/plague_breath')
        local move, process, status, random = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        math.random = function(low, high) assert(low==45 and high==60); return 50 end
        xi.mobskills.processDamage = function() return false end
        assert(breath.onMobSkillCheck(target, mob, {}) == 0 and breath.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.0625 and params.damageCap == 500 and params.element == xi.element.WATER)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = move, process, status, random
        assert(damage[1]==123 and statusParams[3]==xi.effect.POISON and statusParams[4]==5 and statusParams[5]==3 and statusParams[6]==50)
    end)
end)
