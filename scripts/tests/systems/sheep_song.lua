require('scripts/actions/mobskills/sheep_song')
describe('Sheep Song mob skill', function()
    it('applies Sleep I status', function()
        local song = require('scripts/actions/mobskills/sheep_song')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(song.onMobSkillCheck({}, {}, skill) == 0)
        assert(song.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SLEEP_I and params[4] == 1 and params[6] == 45 and message == 456)
    end)
end)
