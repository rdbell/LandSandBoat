require('scripts/actions/mobskills/vulcan_shot')
describe('Vulcan Shot mob skill', function()
    it('uses dark fTP 9 plan and always applies defense down', function()
        local skill = require('scripts/actions/mobskills/vulcan_shot')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 9 and params.element == xi.element.DARK and damage == nil)
        assert(statusParams[3] == xi.effect.DEFENSE_DOWN and statusParams[4] == 50 and statusParams[6] == 80)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
    end)
end)
