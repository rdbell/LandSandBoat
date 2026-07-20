describe('Freezebite mob skill', function()
    it('uses its magical Ice dStat plan and damages only after processing', function()
        local bite = require('scripts/actions/mobskills/freezebite')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE } end
        xi.mobskills.processDamage = function() return false end
        assert(bite.onMobSkillCheck(target, mob, {}) == 0 and bite.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 1 and params.fTP[2] == 1.5 and params.fTP[3] == 3 and params.element == xi.element.ICE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.ICE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1 and params.dStatAttackerMod == xi.mod.INT and params.dStatDefenderMod == xi.mod.INT and damage == nil)
        xi.mobskills.processDamage = function() return true end
        bite.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.ICE)
    end)
end)
