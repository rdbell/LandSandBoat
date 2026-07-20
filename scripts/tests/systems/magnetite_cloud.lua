require('scripts/actions/mobskills/magnetite_cloud')

describe('Magnetite Cloud mob skill', function()
    it('uses its Earth breath request and applies damage plus Weight only after processing', function()
        local magnetiteCloud = require('scripts/actions/mobskills/magnetite_cloud')
        local move = xi.mobskills.mobBreathMove
        local process = xi.mobskills.processDamage
        local status = xi.mobskills.mobStatusEffectMove
        local calls, processed = {}, false
        local mob, skill, action = {}, {}, {}
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        xi.mobskills.mobBreathMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end
        xi.mobskills.mobStatusEffectMove = function(...) table.insert(calls, { 'status', ... }) end

        assert(magnetiteCloud.onMobSkillCheck(target, mob, skill) == 0)
        assert(magnetiteCloud.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.percentMultipier == 0.15 and params.damageCap == 509 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0 and params.resistStat == xi.mod.INT)
        assert(params.element == xi.element.EARTH and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.EARTH and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and #calls == 2)

        processed = true
        assert(magnetiteCloud.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.BREATH and calls[5][5] == xi.damageType.EARTH)
        assert(calls[6][1] == 'status' and calls[6][2] == mob and calls[6][3] == target and calls[6][4] == xi.effect.WEIGHT and calls[6][5] == 75 and calls[6][6] == 0 and calls[6][7] == 60)

        xi.mobskills.mobBreathMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobStatusEffectMove = status
    end)
end)
