describe('Kartstrahl mob skill', function()
    it('uses its Ice plan and applies Sleep I only after processing', function()
        local kartstrahl = require('scripts/actions/mobskills/kartstrahl')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, sleep
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) sleep = { ... } end
        assert(kartstrahl.onMobSkillCheck(target, mob, {}) == 0 and kartstrahl.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2 and params.element == xi.element.ICE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.ICE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and sleep == nil)
        xi.mobskills.processDamage = function() return true end
        kartstrahl.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.ICE and sleep[1] == mob and sleep[2] == target and sleep[3] == xi.effect.SLEEP_I and sleep[4] == 1 and sleep[5] == 0 and sleep[6] == 90)
    end)
end)
