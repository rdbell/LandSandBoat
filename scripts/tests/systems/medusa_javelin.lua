require('scripts/actions/mobskills/medusa_javelin')

describe('Medusa Javelin mob skill', function()
    it('uses its critical piercing plan and petrifies only after processing', function()
        local javelin = require('scripts/actions/mobskills/medusa_javelin')
        local move, process, status, random = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random
        local calls, processed = {}, false
        local mob = { getWeaponDmg = function() return 42 end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local action, skill = {}, {}
        xi.mobskills.mobPhysicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function(...)
            table.insert(calls, { 'process', ... })
            return processed
        end
        xi.mobskills.mobStatusEffectMove = function(...) table.insert(calls, { 'status', ... }) end
        math.random = function(low, high) assert(low == 30 and high == 60); return 45 end

        assert(javelin.onMobSkillCheck(target, mob, skill) == 0)
        assert(javelin.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 42 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.canCrit and params.criticalChance[1] == .10 and params.criticalChance[2] == .20 and params.criticalChance[3] == .25)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and #calls == 2)

        processed = true
        assert(javelin.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.PHYSICAL and calls[5][5] == xi.damageType.PIERCING)
        assert(calls[6][1] == 'status' and calls[6][2] == mob and calls[6][3] == target and calls[6][4] == xi.effect.PETRIFICATION and calls[6][5] == 1 and calls[6][6] == 0 and calls[6][7] == 45)

        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = move, process, status, random
    end)
end)
