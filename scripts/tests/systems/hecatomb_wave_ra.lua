require('scripts/actions/mobskills/hecatomb_wave_ra')
describe('Hecatomb Wave RA mob skill', function()
    it('uses Wind breath plan and Blindness when processed', function()
        local skill = require('scripts/actions/mobskills/hecatomb_wave_ra')
        local breathMove, processDamage, statusMove = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local origRandom = math.random
        math.random = function(a, b)
            if a == 60 and b == 120 then return 90 end
            return origRandom(a, b)
        end
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobBreathMove = function(_,_,_,_,v) params=v; return { damage=50, attackType=xi.attackType.BREATH, damageType=xi.damageType.WIND } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 50)
        assert(params.percentMultipier == 0.0476 and params.damageCap == 260 and status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 50)
        assert(damage == 50 and status[1] == xi.effect.BLINDNESS and status[2] == 30 and status[4] == 90)
        math.random = origRandom
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = breathMove, processDamage, statusMove
    end)
end)
