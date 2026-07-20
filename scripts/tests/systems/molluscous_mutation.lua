require('scripts/actions/mobskills/molluscous_mutation')
describe('Molluscous Mutation mob skill', function()
    it('applies DEFENSE_BOOST power scaled by TP', function()
        local skill = require('scripts/actions/mobskills/molluscous_mutation')
        local msg, buff = nil, nil
        local orig = xi.mobskills.mobBuffMove
        xi.mobskills.mobBuffMove = function(mob, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 194
        end
        local sk = { getTP = function() return 1000 end, setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.DEFENSE_BOOST)
        assert(buff[1] == xi.effect.DEFENSE_BOOST and buff[2] == 50 and buff[4] == 60 and msg == 194)
        sk.getTP = function() return 3000 end
        skill.onMobWeaponSkill({}, {}, sk, {})
        assert(buff[2] == 70)
        xi.mobskills.mobBuffMove = orig
    end)
end)
