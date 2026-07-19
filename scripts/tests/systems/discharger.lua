require('scripts/actions/mobskills/discharger')

describe('Discharger mob skill', function()
    it('allows use, messages Magic Shield, then applies Shock Spikes', function()
        local discharger = require('scripts/actions/mobskills/discharger')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buffs, message = {}, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buffs[#buffs + 1] = { target, effect, power, tick, duration }
            return #buffs * 100
        end

        assert(discharger.onMobSkillCheck(nil, mob, skill) == 0)
        assert(discharger.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.MAGIC_SHIELD)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(#buffs == 2)
        assert(buffs[1][1] == mob and buffs[1][2] == xi.effect.MAGIC_SHIELD)
        assert(buffs[1][3] == 1 and buffs[1][4] == 0 and buffs[1][5] == 60)
        assert(buffs[2][1] == mob and buffs[2][2] == xi.effect.SHOCK_SPIKES)
        assert(buffs[2][3] == 25 and buffs[2][4] == 0 and buffs[2][5] == 60)
        assert(message == 100)
    end)
end)
