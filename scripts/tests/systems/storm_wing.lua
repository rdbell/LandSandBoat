require('scripts/actions/mobskills/storm_wing')
describe('Storm Wing mob skill', function()
    it('uses wind magical plan and applies Silence after processing', function()
        local wing = require('scripts/actions/mobskills/storm_wing')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(wing.onMobSkillCheck(target, mob, {}) == 0 and wing.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[1] == 3 and params.element == xi.element.WIND and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        wing.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.SILENCE and statusParams[6] == 120)
    end)
end)
