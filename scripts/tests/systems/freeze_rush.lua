describe('Freeze Rush mob skill', function()
    it('uses its shadow-ignoring magical Ice plan and damages only after processing', function()
        local rush = require('scripts/actions/mobskills/freeze_rush')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE } end
        xi.mobskills.processDamage = function() return false end
        assert(rush.onMobSkillCheck(target, mob, {}) == 0 and rush.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2 and params.element == xi.element.ICE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.ICE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        rush.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.ICE)
    end)
end)
