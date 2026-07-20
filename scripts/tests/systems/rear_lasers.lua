require('scripts/actions/mobskills/rear_lasers')
describe('Rear Lasers mob skill', function()
    it('requires target behind mob and applies Petrification', function()
        local lasers = require('scripts/actions/mobskills/rear_lasers')
        local status = xi.mobskills.mobStatusEffectMove
        local behind, params, message = true, nil, nil
        local target = { isBehind = function() return behind end }
        local skill = { setMsg = function(_, value) message = value end }
        behind = false; assert(lasers.onMobSkillCheck(target, {}, skill) == 1)
        behind = true; assert(lasers.onMobSkillCheck(target, {}, skill) == 0)
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(lasers.onMobWeaponSkill({}, target, skill, {}) == xi.effect.PETRIFICATION)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.PETRIFICATION and params[4] == 1 and params[6] == 30 and message == 456)
    end)
end)
