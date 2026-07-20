require('scripts/actions/mobskills/maats_bash')

describe("Maat's Bash mob skill", function()
    it('uses its physical parameters and only damages and stuns after processing', function()
        local maatsBash = require('scripts/actions/mobskills/maats_bash')
        local move = xi.mobskills.mobPhysicalMove
        local process = xi.mobskills.processDamage
        local status = xi.mobskills.mobStatusEffectMove
        local calls, processed = {}, false
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local skill, action = {}, {}
        xi.mobskills.mobPhysicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end
        xi.mobskills.mobStatusEffectMove = function(...) table.insert(calls, { 'status', ... }) end

        assert(maatsBash.onMobSkillCheck(target, mob, skill) == 0)
        assert(maatsBash.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1.2 and params.fTP[2] == 1.2 and params.fTP[3] == 1.2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and #calls == 2)

        processed = true
        assert(maatsBash.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.PHYSICAL and calls[5][5] == xi.damageType.BLUNT)
        assert(calls[6][1] == 'status' and calls[6][2] == mob and calls[6][3] == target and calls[6][4] == xi.effect.STUN and calls[6][5] == 1 and calls[6][6] == 0 and calls[6][7] == 4)

        xi.mobskills.mobPhysicalMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobStatusEffectMove = status
    end)
end)
