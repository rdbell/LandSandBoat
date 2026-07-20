describe('Flash Nova mob skill', function()
    it('uses its MND-scaled Light plan and applies Flash only after processing', function()
        local nova = require('scripts/actions/mobskills/flash_nova')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, flash = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) flash = { ... } end
        assert(nova.onMobSkillCheck(target, mob, {}) == 0 and nova.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3 and params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and params.dStatMultiplier == 1 and params.dStatAttackerMod == xi.mod.MND and params.dStatDefenderMod == xi.mod.MND and damage == nil and flash == nil)
        xi.mobskills.processDamage = function() return true end
        nova.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT)
        assert(flash[1] == xi.effect.FLASH and flash[2] == 1 and flash[3] == 0 and flash[4] == 15)
    end)
end)
