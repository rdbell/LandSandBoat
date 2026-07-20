require('scripts/actions/mobskills/chains_of_rage')
describe('Chains of Rage mob skill', function()
    it('applies TERROR to vulnerable PCs without the light key item', function()
        local skill = require('scripts/actions/mobskills/chains_of_rage')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local status, msg = nil, nil
        local races = {xi.race.GALKA}
        local target = {
            isPC = function() return true end,
            getRace = function() return races[1] end,
            hasKeyItem = function(_, ki) return ki == xi.ki.LIGHT_OF_ALTAIEU end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        -- has KI: no effect
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.TERROR)
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT)
        -- missing KI
        target.hasKeyItem = function() return false end
        msg = nil
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.TERROR)
        assert(status[1] == xi.effect.TERROR and status[2] == 30 and status[4] == 30)
        assert(msg == 242)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
