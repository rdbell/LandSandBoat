require('scripts/actions/mobskills/poison_breath_hound')
describe('Poison Breath Hound mob skill', function()
    it('uses water breath plan and applies fixed Poison after processing', function()
        local breath = require('scripts/actions/mobskills/poison_breath_hound')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(breath.onMobSkillCheck(target, {}, {}) == 0 and breath.onMobWeaponSkill({}, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.0625 and params.damageCap == 400)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill({}, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1]==123 and statusParams[3]==xi.effect.POISON and statusParams[4]==1 and statusParams[5]==3 and statusParams[6]==300)
    end)
end)
