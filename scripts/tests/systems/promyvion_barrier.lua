require('scripts/actions/mobskills/promyvion_barrier')

describe('Promyvion Barrier mob skill', function()
    it('blocks when already Defense Boosted and applies fixed Defense Boost', function()
        local barrier = require('scripts/actions/mobskills/promyvion_barrier')
        local buff = xi.mobskills.mobBuffMove
        local hasBoost, params, message = false, nil, nil
        local mob = {
            hasStatusEffect = function(_, effect)
                assert(effect == xi.effect.DEFENSE_BOOST)
                return hasBoost
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end
        hasBoost = true
        assert(barrier.onMobSkillCheck({}, mob, skill) == 1)
        hasBoost = false
        assert(barrier.onMobSkillCheck({}, mob, skill) == 0)
        assert(barrier.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == mob and params[2] == xi.effect.DEFENSE_BOOST and params[3] == 20 and params[4] == 0 and params[5] == 180)
        assert(message == 456)
    end)
end)
