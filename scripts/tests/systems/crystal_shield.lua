require('scripts/actions/mobskills/crystal_shield')

describe('Crystal Shield mob skill', function()
    it('applies Protect II and forwards the buff message', function()
        local crystalShield = require('scripts/actions/mobskills/crystal_shield')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end

        assert(crystalShield.onMobSkillCheck(nil, mob, skill) == 0)
        assert(crystalShield.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.PROTECT)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.PROTECT)
        assert(buff[3] == 20 and buff[4] == 0 and buff[5] == 300)
        assert(message == 123)
    end)
end)
