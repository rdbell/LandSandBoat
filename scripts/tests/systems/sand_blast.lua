require('scripts/actions/mobskills/sand_blast')
describe('Sand Blast mob skill', function()
    it('applies Blindness and does not spawn Alastor for normal pools', function()
        local blast = require('scripts/actions/mobskills/sand_blast')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local mob = {
            getPool = function() return 0 end,
            getLocalVar = function() return 0 end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(blast.onMobSkillCheck({}, mob, skill) == 0)
        assert(blast.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.BLINDNESS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.BLINDNESS and params[4] == 40 and params[6] == 180 and message == 456)
    end)
end)
