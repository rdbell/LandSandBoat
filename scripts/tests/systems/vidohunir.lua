require('scripts/actions/mobskills/vidohunir')
describe('Vidohunir mob skill', function()
    it('uses dark magical plan and TP-scaled magic def down after processing', function()
        local skill = require('scripts/actions/mobskills/vidohunir')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local sk = { getTP = function() return 1000 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.fTP[1] == 1.75 and params.dStatMultiplier == 2 and params.element == xi.element.DARK and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.MAGIC_DEF_DOWN and statusParams[4] == 10 and statusParams[6] == 60)
    end)
end)
