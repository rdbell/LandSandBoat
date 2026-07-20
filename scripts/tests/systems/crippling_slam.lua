require('scripts/actions/mobskills/crippling_slam')
describe('Crippling Slam mob skill', function()
    it('admits front targets and applies TP-scaled Paralysis when processed', function()
        local skill = require('scripts/actions/mobskills/crippling_slam')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local front = false
        local mob = { getWeaponDmg = function() return 50 end }
        local target = {
            isInfront = function() return front end,
            takeDamage = function(_, v) damage = v end,
        }
        local sk = { getTP = function() return 1000 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        front = true
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.SLASHING } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.fTP[1] == 4.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(damage == 100 and status[1] == xi.effect.PARALYSIS and status[2] == 50)
        local expected = xi.mobskills.calculateDuration(1000, 30, 60)
        assert(status[4] == expected)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
