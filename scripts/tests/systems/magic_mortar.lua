require('scripts/actions/mobskills/magic_mortar')

describe('Magic Mortar mob skill', function()
    it('uses missing HP for its magical Light damage request and only applies processed damage', function()
        local magicMortar = require('scripts/actions/mobskills/magic_mortar')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local calls, processed = {}, false
        local maxHP = 1801
        local mob = { getMaxHP = function() return maxHP end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local skill, action = { getMobHP = function() return 600 end }, {}
        xi.mobskills.mobMagicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end

        assert(magicMortar.onMobSkillCheck(target, mob, skill) == 0)
        assert(magicMortar.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 1201 / 6 and params.fTP[1] == 1.0 and params.fTP[2] == 1.0 and params.fTP[3] == 1.0)
        assert(params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and #calls == 2)

        processed = true
        assert(magicMortar.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.MAGICAL and calls[5][5] == xi.damageType.LIGHT)

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
    end)
end)
