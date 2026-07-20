require('scripts/actions/mobskills/malevolent_blessing')

describe('Malevolent Blessing mob skill', function()
    it('uses its Dark magical request and only damages or curses after processing', function()
        local blessing = require('scripts/actions/mobskills/malevolent_blessing')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local status = xi.mobskills.mobStatusEffectMove
        local calls, processed = {}, false
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local action, skill = {}, {}
        xi.mobskills.mobMagicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end
        xi.mobskills.mobStatusEffectMove = function(...) table.insert(calls, { 'status', ... }) end

        assert(blessing.onMobSkillCheck(target, mob, skill) == 0)
        assert(blessing.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 77 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2 and #calls == 2)

        processed = true
        assert(blessing.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.MAGICAL and calls[5][5] == xi.damageType.DARK)
        assert(calls[6][1] == 'status' and calls[6][2] == mob and calls[6][3] == target and calls[6][4] == xi.effect.CURSE_I and calls[6][5] == 35 and calls[6][6] == 0 and calls[6][7] == 45)

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobStatusEffectMove = status
    end)
end)
