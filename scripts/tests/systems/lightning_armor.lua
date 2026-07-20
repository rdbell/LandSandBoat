require('scripts/actions/mobskills/lightning_armor')

describe('Lightning Armor mob skill', function()
    it('forwards the Shock Spikes buff result to its message and returns Shock Spikes', function()
        local armor = require('scripts/actions/mobskills/lightning_armor')
        local buff = xi.mobskills.mobBuffMove
        local call, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) call = { ... }; return 456 end

        assert(armor.onMobSkillCheck({}, mob, skill) == 0)
        assert(armor.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.SHOCK_SPIKES)
        xi.mobskills.mobBuffMove = buff

        assert(call[1] == mob and call[2] == xi.effect.SHOCK_SPIKES)
        assert(call[3] == 10 and call[4] == 0 and call[5] == 180 and message == 456)
    end)
end)
