require('scripts/actions/mobskills/cold_breath')
describe('Cold Breath mob skill', function()
    it('uses Ice breath plan with Bind and breakBind false', function()
        local skill = require('scripts/actions/mobskills/cold_breath')
        local breathMove = xi.mobskills.mobBreathMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, opts, status = nil, nil, nil, nil
        local target = {
            takeDamage = function(_, v, _, _, _, o)
                damage = v
                opts = o
            end,
        }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.BREATH, damageType = xi.damageType.ICE }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 50)
        assert(params.percentMultipier == 0.125 and params.damageCap == 600 and params.element == xi.element.ICE)
        assert(status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 50)
        assert(damage == 50 and opts.breakBind == false)
        assert(status[1] == xi.effect.BIND and status[4] == 60)
        xi.mobskills.mobBreathMove = breathMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
