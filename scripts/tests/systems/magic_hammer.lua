require('scripts/actions/mobskills/magic_hammer')

describe('Magic Hammer mob skill', function()
    it('uses its magical parameters and drains 10 percent of processed damage as MP', function()
        local magicHammer = require('scripts/actions/mobskills/magic_hammer')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local drain = xi.mobskills.mobDrainMove
        local calls, processed = {}, false
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local skill, action = {}, {}
        xi.mobskills.mobMagicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end
        xi.mobskills.mobDrainMove = function(...) table.insert(calls, { 'drain', ... }) end

        assert(magicHammer.onMobSkillCheck(target, mob, skill) == 0)
        assert(magicHammer.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 75 and params.fTP[1] == 2.0 and params.fTP[2] == 2.0 and params.fTP[3] == 2.0 and params.dStatMultiplier == 1)
        assert(params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and #calls == 2)

        processed = true
        assert(magicHammer.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.MAGICAL and calls[5][5] == xi.damageType.LIGHT)
        assert(calls[6][1] == 'drain' and calls[6][2] == mob and calls[6][3] == target and calls[6][4] == xi.mobskills.drainType.MP and calls[6][5] == 12.3)

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobDrainMove = drain
    end)
end)
