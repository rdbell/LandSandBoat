require('scripts/actions/mobskills/azure_lore')

describe('Azure Lore mob skill', function()
    it('allows use, applies its fixed buff, and emits USES', function()
        local azureLore = require('scripts/actions/mobskills/azure_lore')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 999
        end

        assert(azureLore.onMobSkillCheck(nil, mob, skill) == 0)
        assert(azureLore.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.AZURE_LORE)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.AZURE_LORE)
        assert(buff[3] == 1 and buff[4] == 0 and buff[5] == 45)
        assert(message == xi.msg.basic.USES)
    end)
end)
