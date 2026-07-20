require('scripts/actions/mobskills/thornsong')
describe('Thornsong mob skill', function()
    it('refuses when silenced and applies DAMAGE_SPIKES power 10', function()
        local skill = require('scripts/actions/mobskills/thornsong')
        local msg, buff = nil, nil
        local orig = xi.mobskills.mobBuffMove
        xi.mobskills.mobBuffMove = function(mob, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 194
        end
        local mob = { hasStatusEffect = function(_, e) return e == xi.effect.SILENCE end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.hasStatusEffect = function() return false end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.DAMAGE_SPIKES)
        assert(buff[1] == xi.effect.DAMAGE_SPIKES and buff[2] == 10 and buff[4] == 30 and msg == 194)
        xi.mobskills.mobBuffMove = orig
    end)
end)
