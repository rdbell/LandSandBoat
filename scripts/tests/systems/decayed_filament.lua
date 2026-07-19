require('scripts/actions/mobskills/decayed_filament')

describe('Decayed Filament mob skill', function()
    it('uses its Water magical plan and applies Poison only after damage processing', function()
        local params, damage, poison = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobMagicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) poison = { ... } end

        local filament = require('scripts/actions/mobskills/decayed_filament')
        assert(filament.onMobSkillCheck({}, {}, {}) == 0)
        assert(filament.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 1 and params.fTP[2] == 1.5 and params.fTP[3] == 2)
        assert(params.element == xi.element.WATER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WATER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2 and damage == nil and poison == nil)

        xi.mobskills.processDamage = function() return true end
        assert(filament.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WATER)
        assert(poison[3] == xi.effect.POISON and poison[4] == 18 and poison[5] == 3 and poison[6] == 180)
    end)
end)
