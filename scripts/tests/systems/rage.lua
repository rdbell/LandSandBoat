require('scripts/actions/mobskills/rage')
describe('Rage mob skill', function()
    it('always allows use and applies Berserk to the skill target', function()
        local rage = require('scripts/actions/mobskills/rage')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end
        assert(rage.onMobSkillCheck(target, {}, skill) == 0)
        assert(rage.onMobWeaponSkill({}, target, skill, {}) == xi.effect.BERSERK)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == target and params[2] == xi.effect.BERSERK and params[3] == 45 and params[4] == 0 and params[5] == 120)
        assert(message == 456)
    end)
end)
