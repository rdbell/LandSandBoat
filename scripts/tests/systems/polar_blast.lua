require('scripts/actions/mobskills/polar_blast')

describe('Polar Blast mob skill', function()
    it('allows animation sub <= 1, uses ice breath plan, and may re-enable NO_TURN for Tinnin', function()
        local blast = require('scripts/actions/mobskills/polar_blast')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local animSub, pool, behavior, params, damage, statusParams, setBehavior = 0, 0, 0, nil, nil, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getPool = function() return pool end,
            getBehavior = function() return behavior end,
            setBehavior = function(_, value) setBehavior = value end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub = 2
        assert(blast.onMobSkillCheck(target, mob, {}) == 1)
        animSub = 1
        assert(blast.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.ICE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return true end
        pool, behavior, animSub = 0, 0, 0
        assert(blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.01 and params.damageCap == 700 and params.element == xi.element.ICE)
        assert(statusParams[3] == xi.effect.PARALYSIS and statusParams[4] == 15 and setBehavior == nil)
        pool, behavior, animSub = xi.mobPool.TINNIN, 0, 1
        blast.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(setBehavior == bit.bor(0, xi.behavior.NO_TURN))
        assert(damage[1] == 123)
    end)
end)
