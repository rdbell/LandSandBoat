require('scripts/actions/mobskills/antigravity')
describe('Antigravity mob skill', function()
    it('uses Light magical parameters and damages only after processing succeeds', function()
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, p) params = p; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return false end
        local skill = require('scripts/actions/mobskills/antigravity')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0 and skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.element == xi.element.LIGHT and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1 and damage == nil)
        xi.mobskills.processDamage = function() return true end; skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT)
    end)
end)
