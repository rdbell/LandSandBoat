require('scripts/actions/mobskills/malediction')

describe('Malediction mob skill', function()
    it('uses its magical drain request and only drains or sets a message after processing', function()
        local malediction = require('scripts/actions/mobskills/malediction')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local drain = xi.mobskills.mobDrainMove
        local calls, processed, message = {}, false, nil
        local mob = { getMainLvl = function() return 75 end }
        local target, action = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end
        xi.mobskills.mobDrainMove = function(...) table.insert(calls, { 'drain', ... }); return 456 end

        assert(malediction.onMobSkillCheck(target, mob, skill) == 0)
        assert(malediction.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 75 and params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.skipMagicBonusDiff and #calls == 2 and message == nil)

        processed = true
        assert(malediction.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'drain' and calls[5][2] == mob and calls[5][3] == target and calls[5][4] == xi.mobskills.drainType.HP and calls[5][5] == 123 and message == 456)

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobDrainMove = drain
    end)
end)
