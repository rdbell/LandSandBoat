require('scripts/actions/mobskills/particle_shield')

describe('Particle Shield mob skill', function()
    it('always allows use and applies fixed Defense Boost self-buff', function()
        local shield = require('scripts/actions/mobskills/particle_shield')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end
        assert(shield.onMobSkillCheck({}, mob, skill) == 0)
        assert(shield.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == mob and params[2] == xi.effect.DEFENSE_BOOST and params[3] == 50 and params[4] == 0 and params[5] == 300)
        assert(message == 456)
    end)
end)
