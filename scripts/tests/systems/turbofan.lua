require('scripts/actions/mobskills/turbofan')
describe('Turbofan mob skill', function()
    it('uses level-capped wind breath and silence after processing', function()
        local skill = require('scripts/actions/mobskills/turbofan')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, localVar, delSilent = nil, nil, nil, nil, false
        local mob = {
            getMainLvl = function() return 50 end,
            setLocalVar = function(_, k, v) localVar = { k, v } end,
        }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            hasStatusEffect = function(_, e) return e == xi.effect.ELEMENTALRES_DOWN end,
            delStatusEffectSilent = function(_, e) if e == xi.effect.ELEMENTALRES_DOWN then delSilent = true end end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.BREATH, damageType = xi.damageType.WIND }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.percentMultipier == 0.05 and params.damageCap == 490 and statusParams == nil)
        assert(localVar[1] == 'nuclearWaste' and localVar[2] == 0 and not delSilent)
        localVar = nil
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.SILENCE and statusParams[4] == 1 and statusParams[5] == 3 and statusParams[6] == 30)
        assert(delSilent and localVar[2] == 0)
    end)
end)
