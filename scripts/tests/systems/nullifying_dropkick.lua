require('scripts/actions/mobskills/nullifying_dropkick')

describe('Nullifying Dropkick mob skill', function()
    it('always allows use and removes Physical and Magical Shields', function()
        local kick = require('scripts/actions/mobskills/nullifying_dropkick')
        local removed, message = {}, nil
        local target = {
            delStatusEffect = function(_, effect)
                table.insert(removed, effect)
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(kick.onMobSkillCheck(target, {}, skill) == 0)
        assert(kick.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(removed[1] == xi.effect.PHYSICAL_SHIELD and removed[2] == xi.effect.MAGIC_SHIELD)
        assert(message == xi.msg.basic.NONE)
    end)
end)
