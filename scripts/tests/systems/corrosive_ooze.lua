require('scripts/actions/mobskills/corrosive_ooze')
describe('Corrosive Ooze mob skill', function()
    it('uses Water plan and Attack/Defense Down when processed', function()
        local skill = require('scripts/actions/mobskills/corrosive_ooze')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, statuses = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=80, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.WATER } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) statuses[#statuses+1]={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.element == xi.element.WATER and #statuses == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(damage == 80 and statuses[1][1] == xi.effect.ATTACK_DOWN and statuses[2][1] == xi.effect.DEFENSE_DOWN)
        assert(statuses[1][2] == 33 and statuses[1][4] == 120)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
