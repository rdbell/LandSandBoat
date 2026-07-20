require('scripts/actions/mobskills/searing_light')
describe('Searing Light mob skill', function()
    it('uses light magical plan and damages only after processing', function()
        local light = require('scripts/actions/mobskills/searing_light')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 70 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 300, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(light.onMobSkillCheck(target, mob, {}) == 0 and light.onMobWeaponSkill(mob, target, {}, {}) == 300)
        assert(params.baseDamage == 72 and params.fTP[1] == 9 and params.element == xi.element.LIGHT and params.dStatMultiplier == 1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        light.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 300)
    end)
end)
