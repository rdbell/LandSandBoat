require('scripts/actions/mobskills/omniscience')

describe('Omniscience mob skill', function()
    it('uses its Dark magical plan and applies TP-scaled Magic Attack Down only after processing', function()
        local skillObj = require('scripts/actions/mobskills/omniscience')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, mad = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1500 end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) mad = { ... } end
        assert(skillObj.onMobSkillCheck(target, mob, skill) == 0 and skillObj.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.dStatMultiplier == 2)
        assert(params.dStatAttackerMod == xi.mod.MND and params.dStatDefenderMod == xi.mod.MND)
        assert(params.element == xi.element.DARK and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and mad == nil)
        xi.mobskills.processDamage = function() return true end
        skillObj.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and mad[3] == xi.effect.MAGIC_ATK_DOWN and mad[4] == 10 and mad[5] == 0 and mad[6] == 90)
    end)
end)
