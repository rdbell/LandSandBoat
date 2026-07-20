require('scripts/actions/mobskills/maelstrom')

describe('Maelstrom mob skill', function()
    it('uses Water magical parameters and only damages and lowers STR after processing', function()
        local maelstrom = require('scripts/actions/mobskills/maelstrom')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local status = xi.mobskills.mobStatusEffectMove
        local calls, processed = {}, false
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local skill, action = {}, {}
        xi.mobskills.mobMagicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end
        xi.mobskills.mobStatusEffectMove = function(...) table.insert(calls, { 'status', ... }) end

        assert(maelstrom.onMobSkillCheck(target, mob, skill) == 0)
        assert(maelstrom.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 77 and params.fTP[1] == 2.0 and params.fTP[2] == 2.5 and params.fTP[3] == 3.0)
        assert(params.element == xi.element.WATER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WATER and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and #calls == 2)

        processed = true
        assert(maelstrom.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.MAGICAL and calls[5][5] == xi.damageType.WATER)
        assert(calls[6][1] == 'status' and calls[6][2] == mob and calls[6][3] == target and calls[6][4] == xi.effect.STR_DOWN and calls[6][5] == 10 and calls[6][6] == 9 and calls[6][7] == 180)

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobStatusEffectMove = status
    end)
end)
