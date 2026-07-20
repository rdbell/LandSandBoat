require('scripts/actions/mobskills/provoke')

describe('Provoke mob skill', function()
    it('always allows use and adds fixed enmity with NONE message', function()
        local provoke = require('scripts/actions/mobskills/provoke')
        local enmity, message = nil, nil
        local mob = {}
        local target = {
            addEnmity = function(_, actor, ce, ve)
                enmity = { actor, ce, ve }
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(provoke.onMobSkillCheck(target, mob, skill) == 0)
        provoke.onMobWeaponSkill(mob, target, skill, {})
        assert(enmity[1] == mob and enmity[2] == 1 and enmity[3] == 1800)
        assert(message == xi.msg.basic.NONE)
    end)
end)
