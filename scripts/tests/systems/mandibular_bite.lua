require('scripts/actions/mobskills/mandibular_bite')

describe('Mandibular Bite mob skill', function()
    it('uses its one-hit piercing plan and only damages after processing', function()
        local bite = require('scripts/actions/mobskills/mandibular_bite')
        local move = xi.mobskills.mobPhysicalMove
        local process = xi.mobskills.processDamage
        local calls, processed = {}, false
        local mob = { getWeaponDmg = function() return 42 end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local action, skill = {}, {}
        xi.mobskills.mobPhysicalMove = function(...) local args = { ... }; table.insert(calls, { 'move', args }); return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end

        assert(bite.onMobSkillCheck(target, mob, skill) == 0)
        assert(bite.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 42 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2 and params.attackMultiplier[1] == 1.5 and params.attackMultiplier[2] == 1.5 and params.attackMultiplier[3] == 1.5 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and #calls == 2)

        processed = true
        assert(bite.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.PHYSICAL and calls[5][5] == xi.damageType.PIERCING)

        xi.mobskills.mobPhysicalMove = move
        xi.mobskills.processDamage = process
    end)
end)
