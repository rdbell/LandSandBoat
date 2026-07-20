require('scripts/actions/mobskills/vitriolic_barrage')
describe('Vitriolic Barrage mob skill', function()
    it('splits 1000 base by total targets and poisons after processing', function()
        local skill = require('scripts/actions/mobskills/vitriolic_barrage')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = {}
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local sk = { getTotalTargets = function() return 4 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.baseDamage == 250 and params.skipDamageAdjustment and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.POISON and statusParams[4] == 18 and statusParams[5] == 3 and statusParams[6] == 180)
    end)
end)
