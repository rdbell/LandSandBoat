require('scripts/actions/mobskills/cackle')
describe('Cackle mob skill', function()
    it('reports first successful magic down effect', function()
        local skill = require('scripts/actions/mobskills/cackle')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local attempts, msg = {}, nil
        local target = {
            hasStatusEffect = function(_, e) return e == xi.effect.MAGIC_ATK_DOWN end,
        }
        local sk = {
            setMsg = function(_, m) msg = m end,
            getMsg = function() return msg end,
        }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            attempts[#attempts + 1] = { effect, power, tick, duration }
            if effect == xi.effect.MAGIC_ACC_DOWN then
                return xi.msg.basic.SKILL_ENFEEB_IS
            end
            return xi.msg.basic.SKILL_MISS
        end
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.MAGIC_ACC_DOWN)
        assert(#attempts == 2)
        assert(attempts[1][1] == xi.effect.MAGIC_ACC_DOWN and attempts[1][2] == 50 and attempts[1][4] == 60)
        assert(msg == xi.msg.basic.SKILL_ENFEEB)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
