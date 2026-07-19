require('scripts/actions/mobskills/dancing_chains')

describe('Dancing Chains mob skill', function()
    it('is always available', function()
        local skill = require('scripts/actions/mobskills/dancing_chains')

        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('requests Drown with its fixed status-effect parameters', function()
        local message, request = nil, nil
        local mob = {}
        local target = {}
        local skill = {
            setMsg = function(_, value)
                message = value
            end,
        }
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(moveMob, moveTarget, effect, power, tick, duration)
            request = { moveMob, moveTarget, effect, power, tick, duration }
            return 123
        end

        local chains = require('scripts/actions/mobskills/dancing_chains')
        assert(chains.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.DROWN)

        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(message == 123)
        assert(request[1] == mob and request[2] == target)
        assert(request[3] == xi.effect.DROWN)
        assert(request[4] == 15 and request[5] == 0 and request[6] == 60)
    end)
end)
