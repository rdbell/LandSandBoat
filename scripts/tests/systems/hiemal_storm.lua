require('scripts/actions/mobskills/hiemal_storm')
describe('Hiemal Storm mob skill', function()
    it('uses Ice breath plan with conal adjustment', function()
        local skill = require('scripts/actions/mobskills/hiemal_storm')
        local breathMove, processDamage = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local conal = utils.conalDamageAdjustment
        local params, damage, adjusted = nil, nil, nil
        utils.conalDamageAdjustment = function(_, _, _, dmg, factor)
            adjusted = { dmg, factor }
            return math.floor(dmg * factor)
        end
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobBreathMove = function(_,_,_,_,v) params=v; return { damage=1000, attackType=xi.attackType.BREATH, damageType=xi.damageType.ICE } end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 900)
        assert(params.percentMultipier == 0.382 and params.damageCap == 1300)
        assert(adjusted[1] == 1000 and adjusted[2] == 0.9 and damage == 900)
        utils.conalDamageAdjustment = conal
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = breathMove, processDamage
    end)
end)
