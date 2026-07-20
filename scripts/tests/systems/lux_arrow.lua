require('scripts/actions/mobskills/lux_arrow')

describe('Lux Arrow mob skill', function()
    it('uses its captured magical ranged parameters and only applies processed damage', function()
        local luxArrow = require('scripts/actions/mobskills/lux_arrow')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local calls, processed = {}, false
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local skill, action = {}, {}
        xi.mobskills.mobMagicalMove = function(...)
            local args = { ... }
            table.insert(calls, { 'move', args })
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end

        assert(luxArrow.onMobSkillCheck(target, mob, skill) == 0)
        assert(luxArrow.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 77 and params.fTP[1] == 2.5 and params.fTP[2] == 2.5 and params.fTP[3] == 2.5)
        assert(params.element == xi.element.LIGHT and params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and #calls == 2)

        processed = true
        assert(luxArrow.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.RANGED and calls[5][5] == xi.damageType.PIERCING)

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
    end)
end)
