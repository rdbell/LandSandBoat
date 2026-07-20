require('scripts/actions/mobskills/rapid_molt')
describe('Rapid Molt mob skill', function()
    it('admits when erasable effect exists and applies non-dispelable Regen', function()
        local skill = require('scripts/actions/mobskills/rapid_molt')
        local msg, buff, flagCleared, erasedAll = nil, nil, false, false
        local orig = xi.mobskills.mobBuffMove
        xi.mobskills.mobBuffMove = function(mob, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 194
        end
        local mob = {
            eraseStatusEffect = function() return xi.effect.POISON end,
            eraseAllStatusEffect = function() erasedAll = true end,
            getStatusEffect = function()
                return { delEffectFlag = function() flagCleared = true end }
            end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(mob, mob, sk) == 0)
        mob.eraseStatusEffect = function() return xi.effect.NONE end
        assert(skill.onMobSkillCheck(mob, mob, sk) == 1)
        assert(skill.onMobWeaponSkill(mob, mob, sk, {}) == xi.effect.REGEN)
        assert(erasedAll and buff[1] == xi.effect.REGEN and buff[2] == 40 and buff[3] == 3 and buff[4] == 30)
        assert(flagCleared and msg == 194)
        xi.mobskills.mobBuffMove = orig
    end)
end)
