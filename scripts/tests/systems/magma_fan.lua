require('scripts/actions/mobskills/magma_fan')

describe('Magma Fan mob skill', function()
    it('uses its captured breath request and only applies processed damage', function()
        local magmaFan = require('scripts/actions/mobskills/magma_fan')
        local move = xi.mobskills.mobBreathMove
        local process = xi.mobskills.processDamage
        local calls, processed = {}, false
        local mob, skill, action = {}, {}, {}
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        xi.mobskills.mobBreathMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end

        assert(magmaFan.onMobSkillCheck(target, mob, skill) == 0)
        assert(magmaFan.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.percentMultipier == 0.0833 and params.damageCap == 600 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0 and params.resistStat == xi.mod.INT)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.WATER and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and #calls == 2)

        processed = true
        assert(magmaFan.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.BREATH and calls[5][5] == xi.damageType.WATER)

        xi.mobskills.mobBreathMove = move
        xi.mobskills.processDamage = process
    end)
end)
