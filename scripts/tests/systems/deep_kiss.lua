require('scripts/actions/mobskills/deep_kiss')
describe('Deep Kiss mob skill', function()
    it('steals one effect by default and all for Phoedme', function()
        local skill = require('scripts/actions/mobskills/deep_kiss')
        local drainMove = xi.mobskills.mobDrainStatusEffectMove
        local msg, pool = nil, 0
        local mob = {
            getPool = function() return pool end,
            stealStatusEffect = function() return 0 end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobDrainStatusEffectMove = function() return xi.msg.basic.EFFECT_DRAINED end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 1)
        assert(msg == xi.msg.basic.EFFECT_DRAINED)
        pool = xi.mobPool.PHOEDME
        local left = 2
        mob.stealStatusEffect = function()
            if left > 0 then left = left - 1; return 1 end
            return 0
        end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 2)
        xi.mobskills.mobDrainStatusEffectMove = drainMove
    end)
end)
