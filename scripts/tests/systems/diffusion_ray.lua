require('scripts/actions/mobskills/diffusion_ray')

describe('Diffusion Ray mob skill', function()
    it('uses its MND-based Light magical plan and damages only after processing', function()
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end

        local diffusionRay = require('scripts/actions/mobskills/diffusion_ray')
        assert(diffusionRay.onMobSkillCheck({}, {}, {}) == 0)
        assert(diffusionRay.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 5 and params.fTP[2] == 5 and params.fTP[3] == 5)
        assert(params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1.5)
        assert(params.dStatAttackerMod == xi.mod.MND and params.dStatDefenderMod == xi.mod.MND and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(diffusionRay.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT)
    end)
end)
