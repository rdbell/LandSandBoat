require('scripts/actions/mobskills/metallic_body')

describe('Metallic Body mob skill', function()
    it('always allows use and applies its fixed Stoneskin target buff', function()
        local metallic = require('scripts/actions/mobskills/metallic_body')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local target = {}
        local skill = {
            setMsg = function(_, value)
                message = value
            end,
        }

        xi.mobskills.mobBuffMove = function(source, effect, power, tick, duration)
            params = { source, effect, power, tick, duration }
            return 456
        end

        assert(metallic.onMobSkillCheck(target, mob, skill) == 0)
        assert(metallic.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.STONESKIN)
        xi.mobskills.mobBuffMove = buff

        assert(params[1] == target and params[2] == xi.effect.STONESKIN and params[3] == 25 and params[4] == 0 and params[5] == 300)
        assert(message == 456)
    end)
end)
