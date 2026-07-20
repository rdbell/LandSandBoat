require('scripts/actions/mobskills/sand_pit')
describe('Sand Pit mob skill', function()
    it('applies Bind and defers pool side-effects', function()
        local pit = require('scripts/actions/mobskills/sand_pit')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message, pool = nil, nil, 0
        local mob = { getPool = function() return pool end, getXPos = function() return 0 end, getYPos = function() return 0 end, getZPos = function() return 0 end, getID = function() return 1 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(pit.onMobSkillCheck({}, mob, skill) == 0)
        assert(pit.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.BIND)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.BIND and params[4] == 1 and params[6] == 60 and message == 456)
    end)
end)
