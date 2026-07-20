require('scripts/actions/mobskills/snow_cloud')
describe('Snow Cloud mob skill', function()
    it('uses ice magical plan and applies Paralysis after processing', function()
        local cloud = require('scripts/actions/mobskills/snow_cloud')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 60, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(cloud.onMobSkillCheck(target, mob, {}) == 0 and cloud.onMobWeaponSkill(mob, target, {}, {}) == 60)
        assert(params.baseDamage == 42 and params.element == xi.element.ICE and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        cloud.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 60 and statusParams[3] == xi.effect.PARALYSIS and statusParams[4] == 30 and statusParams[6] == 120)
    end)
end)
