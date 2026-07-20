require('scripts/actions/mobskills/warm-up')
describe('Warm-Up mob skill', function()
    it('refreshes existing accuracy/evasion boosts only', function()
        local skill = require('scripts/actions/mobskills/warm-up')
        local buff = xi.mobskills.mobBuffMove
        local message, buffs = nil, {}
        local sk = { setMsg = function(_, v) message = v end }
        local has = {}
        local mob = { hasStatusEffect = function(_, e) return has[e] == true end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            buffs[#buffs + 1] = { effect, power, duration }
            return 100 + #buffs
        end
        local ret = skill.onMobWeaponSkill(mob, {}, sk, {})
        assert(ret == nil and #buffs == 0)
        has[xi.effect.ACCURACY_BOOST] = true
        has[xi.effect.EVASION_BOOST] = true
        ret = skill.onMobWeaponSkill(mob, {}, sk, {})
        xi.mobskills.mobBuffMove = buff
        assert(ret == xi.effect.EVASION_BOOST and message == 102)
        assert(buffs[1][1] == xi.effect.ACCURACY_BOOST and buffs[1][2] == 40 and buffs[1][3] == 60)
        assert(buffs[2][1] == xi.effect.EVASION_BOOST)
    end)
end)
