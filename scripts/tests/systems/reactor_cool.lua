require('scripts/actions/mobskills/reactor_cool')
describe('Reactor Cool mob skill', function()
    it('applies undispellable Ice Spikes and Defense Boost', function()
        local cool = require('scripts/actions/mobskills/reactor_cool')
        local buff, random = xi.mobskills.mobBuffMove, math.random
        local calls, message, effects = {}, nil, {}
        local mob = {
            getStatusEffect = function(_, effect)
                return effects[effect]
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            calls[#calls + 1] = { effect, power, tick, duration }
            local se = {
                delEffectFlag = function(self, flag)
                    self.flag = flag
                end,
            }
            effects[effect] = se
            return 456
        end
        math.random = function(low, high) assert(low==15 and high==30); return 20 end
        assert(cool.onMobSkillCheck({}, mob, skill) == 0)
        assert(cool.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.ICE_SPIKES)
        xi.mobskills.mobBuffMove, math.random = buff, random
        assert(calls[1][1] == xi.effect.ICE_SPIKES and calls[1][2] == 20 and calls[1][4] == 120)
        assert(calls[2][1] == xi.effect.DEFENSE_BOOST and calls[2][2] == 25)
        assert(effects[xi.effect.ICE_SPIKES].flag == xi.effectFlag.DISPELABLE)
        assert(effects[xi.effect.DEFENSE_BOOST].flag == xi.effectFlag.DISPELABLE)
        assert(message == 456)
    end)
end)
