require('scripts/actions/mobskills/luminous_lance')

describe('Luminous Lance mob skill', function()
    it('uses its ranged parameters and only damages, animates, and terrifies after processing', function()
        local luminousLance = require('scripts/actions/mobskills/luminous_lance')
        local ranged = xi.mobskills.mobRangedMove
        local process = xi.mobskills.processDamage
        local calls, processed = {}, false
        local mob = { getWeaponDmg = function() return 77 end }
        local target = {
            takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end,
            setAnimationSub = function(_, value) table.insert(calls, { 'animation', value }) end,
            addStatusEffect = function(_, effect, options) table.insert(calls, { 'effect', effect, options }) end,
        }
        local skill, action = {}, {}
        xi.mobskills.mobRangedMove = function(...)
            local args = { ... }
            table.insert(calls, { 'ranged', args })
            return { damage = 123, attackType = 1, damageType = 2 }
        end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end

        assert(luminousLance.onMobSkillCheck(target, mob, skill) == 0)
        assert(luminousLance.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3.0 and params.fTP[2] == 3.0 and params.fTP[3] == 3.0)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.skipParry and params.skipGuard and params.skipBlock and #calls == 2)

        processed = true
        assert(luminousLance.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == 1 and calls[5][5] == 2)
        assert(calls[6][1] == 'animation' and calls[6][2] == 3)
        assert(calls[7][1] == 'effect' and calls[7][2] == xi.effect.TERROR and calls[7][3].duration == 30 and calls[7][3].origin == mob)

        xi.mobskills.mobRangedMove = ranged
        xi.mobskills.processDamage = process
    end)
end)
