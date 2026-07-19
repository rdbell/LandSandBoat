require('scripts/actions/mobskills/aerial_blast')

describe('Aerial Blast mob skill', function()
    it('uses its Wind magical plan and damages only after processing', function()
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end

        local blast = require('scripts/actions/mobskills/aerial_blast')
        assert(blast.onMobSkillCheck({}, {}, {}) == 0)
        assert(blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 9 and params.fTP[2] == 9 and params.fTP[3] == 9)
        assert(params.element == xi.element.WIND and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WIND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 2 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WIND)
    end)
end)
