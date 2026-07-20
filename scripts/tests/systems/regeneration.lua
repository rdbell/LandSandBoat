require('scripts/actions/mobskills/regeneration')
describe('Regeneration mob skill', function()
    it('blocks when already under Regen and applies TP-scaled Regen', function()
        local regen = require('scripts/actions/mobskills/regeneration')
        local buff = xi.mobskills.mobBuffMove
        local hasRegen, params, message, tp = false, nil, nil, 1000
        local mob = {
            hasStatusEffect = function(_, effect)
                assert(effect == xi.effect.REGEN)
                return hasRegen
            end,
        }
        local skill = {
            getTP = function() return tp end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        hasRegen = true; assert(regen.onMobSkillCheck({}, mob, skill) == 1)
        hasRegen = false; assert(regen.onMobSkillCheck({}, mob, skill) == 0)
        assert(regen.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.REGEN)
        assert(params[2] == xi.effect.REGEN and params[3] == 5 and params[4] == 3 and params[5] == 300)
        tp = 2000
        regen.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobBuffMove = buff
        assert(params[3] == 8 and message == 456)
    end)
end)
