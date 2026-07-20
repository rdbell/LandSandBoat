require('scripts/actions/mobskills/mangle')

describe('Mangle mob skill', function()
    it('allows sword or back-dagger forms and only damages after processing', function()
        local mangle = require('scripts/actions/mobskills/mangle')
        local move = xi.mobskills.mobPhysicalMove
        local process = xi.mobskills.processDamage
        local calls, processed = {}, false
        local animation = 0
        local mob = { getWeaponDmg = function() return 42 end, getAnimationSub = function() return animation end }
        local target = { takeDamage = function(_, ...) table.insert(calls, { 'damage', ... }) end }
        local action, skill = {}, {}
        xi.mobskills.mobPhysicalMove = function(...) local args = { ... }; table.insert(calls, { 'move', args }); return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function(...) table.insert(calls, { 'process', ... }); return processed end

        assert(mangle.onMobSkillCheck(target, mob, skill) == 0)
        animation = 2
        assert(mangle.onMobSkillCheck(target, mob, skill) == 0)
        animation = 1
        assert(mangle.onMobSkillCheck(target, mob, skill) == 1)
        assert(mangle.onMobWeaponSkill(mob, target, skill, action) == 123)
        local params = calls[1][2][5]
        assert(params.baseDamage == 42 and params.numHits == 3 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and #calls == 2)

        processed = true
        assert(mangle.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(calls[5][1] == 'damage' and calls[5][2] == 123 and calls[5][3] == mob and calls[5][4] == xi.attackType.PHYSICAL and calls[5][5] == xi.damageType.SLASHING)

        xi.mobskills.mobPhysicalMove = move
        xi.mobskills.processDamage = process
    end)
end)
