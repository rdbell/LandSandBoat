require('scripts/actions/mobskills/smoke_discharger')
describe('Smoke Discharger mob skill', function()
    it('uses earth breath with Petrification and clears nuclearWaste', function()
        local skill = require('scripts/actions/mobskills/smoke_discharger')
        local params, status, localVar = nil, nil, nil
        local origB, origD, origS = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobBreathMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local mob = {
            getMainLvl = function() return 70 end,
            setLocalVar = function(_, k, v) localVar = { k, v } end,
        }
        local target = {
            takeDamage = function() end,
            hasStatusEffect = function() return false end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.percentMultipier == 0.05 and params.damageCap == 750)
        assert(status[1] == xi.effect.PETRIFICATION and status[4] == 45)
        assert(localVar[1] == 'nuclearWaste' and localVar[2] == 0)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origB, origD, origS
    end)
end)
