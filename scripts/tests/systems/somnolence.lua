require('scripts/actions/mobskills/somnolence')
describe('Somnolence mob skill', function()
    it('uses dark magical plan and applies Weight after processing', function()
        local skillmod = require('scripts/actions/mobskills/somnolence')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skillmod.onMobSkillCheck(target, mob, {}) == 0 and skillmod.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.baseDamage == 52 and params.dStatMultiplier == 1.5 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skillmod.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 80 and statusParams[3] == xi.effect.WEIGHT and statusParams[4] == 25 and statusParams[6] == 90)
    end)
end)
