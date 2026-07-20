require('scripts/actions/mobskills/laser_shower')

describe('Laser Shower mob skill', function()
    it('rejects targets behind the mob', function()
        local laserShower = require('scripts/actions/mobskills/laser_shower')
        local mob = {}
        local target = { isBehind = function(_, source, angle) assert(source == mob and angle == 48); return true end }
        assert(laserShower.onMobSkillCheck(target, mob, {}) == 1)
        target.isBehind = function() return false end
        assert(laserShower.onMobSkillCheck(target, mob, {}) == 0)
    end)

    it('scales and clamps Light breath damage before gated Defense Down', function()
        local laserShower = require('scripts/actions/mobskills/laser_shower')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = { checkDistance = function() return 5 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) effect = { ... } end

        assert(laserShower.onMobWeaponSkill(mob, target, {}, {}) == 61.5)
        assert(params.percentMultipier == 0.20 and params.damageCap == 1600 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.LIGHT)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.LIGHT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and effect == nil)

        xi.mobskills.processDamage = function() return true end
        assert(laserShower.onMobWeaponSkill(mob, target, {}, {}) == 61.5)
        mob.checkDistance = function() return 0 end
        assert(laserShower.onMobWeaponSkill(mob, target, {}, {}) == 50)
        mob.checkDistance = function() return 200 end
        assert(laserShower.onMobWeaponSkill(mob, target, {}, {}) == 1600)

        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status

        assert(damage[1] == 1600 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.LIGHT)
        assert(effect[1] == mob and effect[2] == target and effect[3] == xi.effect.DEFENSE_DOWN)
        assert(effect[4] == 25 and effect[5] == 0 and effect[6] == 60)
    end)
end)
