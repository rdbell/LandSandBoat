describe('Kick Out mob skill', function()
    it('requires a target behind the Behemoth and applies Blindness only after processing', function()
        local kick = require('scripts/actions/mobskills/kick_out')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, blind
        local behind = true
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { isBehind = function(_, other, angle) return other == mob and angle == 48 and behind end, takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) blind = { ... } end
        assert(kick.onMobSkillCheck(target, mob, {}) == 0)
        behind = false; assert(kick.onMobSkillCheck(target, mob, {}) == 1)
        behind = true; assert(kick.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.HAND_TO_HAND and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil and blind == nil)
        xi.mobskills.processDamage = function() return true end
        kick.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.HAND_TO_HAND and blind[1] == mob and blind[2] == target and blind[3] == xi.effect.BLINDNESS and blind[4] == 20 and blind[5] == 0 and blind[6] == 120)
    end)
end)
