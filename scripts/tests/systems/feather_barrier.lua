describe('Feather Barrier mob skill', function()
    it('rejects an existing Evasion Boost and scales duration from TP', function()
        local featherBarrier = require('scripts/actions/mobskills/feather_barrier')
        local buffMove = xi.mobskills.mobBuffMove
        local params, message, hasBoost, tp = nil, nil, false, 1500
        local mob = { hasStatusEffect = function() return hasBoost end }
        local skill = {
            getTP = function() return tp end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            params = { effect, power, tick, duration }
            return 777
        end
        assert(featherBarrier.onMobSkillCheck({}, mob, {}) == 0)
        hasBoost = true
        assert(featherBarrier.onMobSkillCheck({}, mob, {}) == 1)
        assert(featherBarrier.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.EVASION_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(params[1] == xi.effect.EVASION_BOOST and params[2] == 25 and params[3] == 0 and params[4] == 390)
        assert(message == 777)
    end)
end)
