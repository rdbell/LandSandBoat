require('scripts/actions/mobskills/sandspin')
describe('Sandspin mob skill', function()
    it('uses earth magical plan and applies Accuracy Down after processing', function()
        local spin = require('scripts/actions/mobskills/sandspin')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 88, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(spin.onMobSkillCheck(target, mob, {}) == 0 and spin.onMobWeaponSkill(mob, target, {}, {}) == 88)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.element == xi.element.EARTH and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        spin.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 88 and statusParams[3] == xi.effect.ACCURACY_DOWN and statusParams[4] == 25 and statusParams[6] == 180)
    end)
end)
