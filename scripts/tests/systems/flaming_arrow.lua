describe('Flaming Arrow mob skill', function()
    it('applies its physical and surviving-target hybrid Fire damage and returns their total', function()
        local arrow = require('scripts/actions/mobskills/flaming_arrow')
        local rangedMove, processDamage = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damages = nil, {}
        local processed, hp = false, 100
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damages[#damages + 1] = { ... } end, getHP = function() return hp end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value) params = value; return { damage = 123, hybridDamage = 45, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING, hybridAttackType = xi.attackType.MAGICAL, hybridDamageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return processed end
        assert(arrow.onMobSkillCheck(target, mob, {}) == 0 and arrow.onMobWeaponSkill(mob, target, {}, {}) == 0)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.skipParry and params.skipGuard and params.skipBlock and params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING)
        assert(params.hybridSkill and params.hybridSkillElement == xi.element.FIRE and params.hybridAttackType == xi.attackType.MAGICAL and params.hybridDamageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and #damages == 0)
        processed = true
        assert(arrow.onMobWeaponSkill(mob, target, {}, {}) == 168 and #damages == 2)
        assert(damages[1][1] == 123 and damages[1][2] == mob and damages[1][3] == xi.attackType.RANGED and damages[1][4] == xi.damageType.PIERCING)
        assert(damages[2][1] == 45 and damages[2][2] == mob and damages[2][3] == xi.attackType.MAGICAL and damages[2][4] == xi.damageType.FIRE)
        hp, damages = 0, {}
        assert(arrow.onMobWeaponSkill(mob, target, {}, {}) == 123 and #damages == 1)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = rangedMove, processDamage
    end)
end)
