require('scripts/actions/mobskills/pyric_blast')
describe('Pyric Blast mob skill', function()
    it('requires animation sub 0, uses fire breath plan, and may re-enable NO_TURN for Tinnin', function()
        local blast = require('scripts/actions/mobskills/pyric_blast')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local animSub, pool, behavior, params, damage, statusParams, setBehavior = 0, 0, 0, nil, nil, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getPool = function() return pool end,
            getBehavior = function() return behavior end,
            setBehavior = function(_, value) setBehavior = value end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub = 1; assert(blast.onMobSkillCheck(target, mob, {}) == 1)
        animSub = 0; assert(blast.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return true end
        pool, behavior = 0, 0
        assert(blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.01 and params.element == xi.element.FIRE)
        assert(statusParams[3] == xi.effect.PLAGUE and statusParams[4] == 5 and setBehavior == nil)
        pool, behavior = xi.mobPool.TINNIN, 0
        blast.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(setBehavior == bit.bor(0, xi.behavior.NO_TURN) and damage[1] == 123)
    end)
end)
