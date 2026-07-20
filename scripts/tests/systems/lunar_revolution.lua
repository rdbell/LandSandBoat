require('scripts/actions/mobskills/lunar_revolution')

describe('Lunar Revolution mob skill', function()
    it('uses the captured physical parameters and only applies processed damage', function()
        local lunarRevolution = require('scripts/actions/mobskills/lunar_revolution')
        local move = xi.mobskills.mobPhysicalMove
        local process = xi.mobskills.processDamage
        local calls, processed = {}, false
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local skill, action = {}, {}
        xi.mobskills.mobPhysicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end

        assert(lunarRevolution.onMobSkillCheck(target, mob, skill) == 0)
        assert(lunarRevolution.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2.0 and params.fTP[2] == 2.0 and params.fTP[3] == 2.0)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and #calls == 2)

        processed = true
        assert(lunarRevolution.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.PHYSICAL and calls[5][5] == xi.damageType.SLASHING)

        xi.mobskills.mobPhysicalMove = move
        xi.mobskills.processDamage = process
    end)
end)
