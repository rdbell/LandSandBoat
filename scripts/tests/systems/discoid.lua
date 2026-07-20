require('scripts/actions/mobskills/discoid')

describe('Discoid mob skill', function()
    it('splits its base damage across targets and applies named Chariot overrides', function()
        local discoid = require('scripts/actions/mobskills/discoid')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local name, targets = 'Other', 4
        local mob = { getName = function() return name end }
        local skill = { getTotalTargets = function() return targets end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(discoid.onMobSkillCheck({}, {}, {}) == 0)
        assert(discoid.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 250 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)

        name, targets = 'Pandemonium_Warden', 5
        assert(discoid.onMobWeaponSkill(mob, target, skill, {}) == 123 and params.baseDamage == 2000)
        name = 'Battleclad_Chariot'
        assert(discoid.onMobWeaponSkill(mob, target, skill, {}) == 123 and params.baseDamage == 880)
        name = 'Battledressed_Chariot'
        xi.mobskills.processDamage = function() return true end
        assert(discoid.onMobWeaponSkill(mob, target, skill, {}) == 123 and params.baseDamage == 880)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.NONE)
    end)
end)
