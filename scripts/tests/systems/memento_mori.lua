require('scripts/actions/mobskills/memento_mori')

describe('Memento Mori mob skill', function()
    it('always allows use and applies its Magic Attack Boost self-buff', function()
        local memento = require('scripts/actions/mobskills/memento_mori')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = {
            setMsg = function(_, value)
                message = value
            end,
        }

        xi.mobskills.mobBuffMove = function(source, effect, power, tick, duration)
            params = { source, effect, power, tick, duration }
            return 456
        end

        assert(memento.onMobSkillCheck({}, mob, skill) == 0)
        assert(memento.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.MAGIC_ATK_BOOST)
        xi.mobskills.mobBuffMove = buff

        assert(params[1] == mob and params[2] == xi.effect.MAGIC_ATK_BOOST and params[3] == 20 and params[4] == 0 and params[5] == 300)
        assert(message == 456)
    end)
end)
