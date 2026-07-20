describe('Firespit mob skill', function()
    it('uses three shadows normally, ignores them for the blue Mamool Ja skill, and gates damage', function()
        local firespit = require('scripts/actions/mobskills/firespit')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, skillID = nil, nil, 1733
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getID = function() return skillID end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(firespit.onMobSkillCheck(target, mob, skill) == 0)
        assert(firespit.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 75 and params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.dStatMultiplier == 1)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        skillID = xi.mobSkill.FIRESPIT_BLUE_MAMOOLJA
        assert(firespit.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        xi.mobskills.processDamage = function() return true end
        assert(firespit.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
