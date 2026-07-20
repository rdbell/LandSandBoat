require('scripts/actions/mobskills/soulshattering_roar')
describe('Soulshattering Roar mob skill', function()
    it('uses dark magical plan and applies Terror after processing', function()
        local roar = require('scripts/actions/mobskills/soulshattering_roar')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 70 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 300, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(roar.onMobSkillCheck(target, mob, {}) == 0 and roar.onMobWeaponSkill(mob, target, {}, {}) == 300)
        assert(params.baseDamage == 72 and params.fTP[1] == 8 and params.element == xi.element.DARK and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        roar.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 300 and statusParams[3] == xi.effect.TERROR and statusParams[6] == 30)
    end)
end)
