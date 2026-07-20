require('scripts/actions/mobskills/mega_holy')

describe('Mega Holy mob skill', function()
    it('uses its captured Light magical plan and damages only after processing', function()
        local holy = require('scripts/actions/mobskills/mega_holy')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end

        assert(holy.onMobSkillCheck(target, mob, {}) == 0 and holy.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 9 and params.fTP[2] == 9 and params.fTP[3] == 9)
        assert(params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        holy.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT)
    end)
end)
