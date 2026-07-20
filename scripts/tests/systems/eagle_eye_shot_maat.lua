require('scripts/actions/mobskills/eagle_eye_shot_maat')

describe('Eagle Eye Shot Maat mob skill', function()
    it('uses its lower-damage ranged physical plan and applies damage only after processing', function()
        local eagleEyeShot = require('scripts/actions/mobskills/eagle_eye_shot_maat')
        local rangedMove, processDamage = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        assert(eagleEyeShot.onMobSkillCheck({}, {}, {}) == 0)
        assert(eagleEyeShot.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 6 and params.fTP[2] == 6 and params.fTP[3] == 6)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.skipParry and params.skipGuard and params.skipBlock and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(eagleEyeShot.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = rangedMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
