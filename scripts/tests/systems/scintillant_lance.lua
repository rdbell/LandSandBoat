require('scripts/actions/mobskills/scintillant_lance')
describe('Scintillant Lance mob skill', function()
    it('uses light magical plan and damages only after processing', function()
        local lance = require('scripts/actions/mobskills/scintillant_lance')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 60 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 150, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(lance.onMobSkillCheck(target, mob, {}) == 0 and lance.onMobWeaponSkill(mob, target, {}, {}) == 150)
        assert(params.baseDamage == 62 and params.fTP[1] == 3.75 and params.element == xi.element.LIGHT and damage == nil)
        xi.mobskills.processDamage = function() return true end
        lance.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 150)
    end)
end)
