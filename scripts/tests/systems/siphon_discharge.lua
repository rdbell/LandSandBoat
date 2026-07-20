require('scripts/actions/mobskills/siphon_discharge')
describe('Siphon Discharge mob skill', function()
    it('uses magical plan and damages only after processing', function()
        local skillmod = require('scripts/actions/mobskills/siphon_discharge')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skillmod.onMobSkillCheck(target, mob, {}) == 0 and skillmod.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.baseDamage == 52 and params.element == xi.element.WATER and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skillmod.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 80)
    end)
end)
