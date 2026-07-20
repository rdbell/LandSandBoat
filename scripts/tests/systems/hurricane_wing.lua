require('scripts/actions/mobskills/hurricane_wing')
describe('Hurricane Wing mob skill', function()
    it('admits front targets and blinds with Nidhogg power', function()
        local skill = require('scripts/actions/mobskills/hurricane_wing')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, status, front, anim, pool = nil, nil, nil, false, 0, 0
        local mob = {
            getAnimationSub = function() return anim end,
            getMainLvl = function() return 50 end,
            getPool = function() return pool end,
        }
        local target = {
            isInfront = function() return front end,
            takeDamage = function(_, v) damage = v end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        front = true
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.WIND } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d} end
        xi.mobskills.processDamage = function() return true end
        pool = xi.mobPool.NIDHOGG
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.250 and params.fTP[3] == 4.000)
        assert(status[1] == xi.effect.BLINDNESS and status[2] == 160 and status[4] == 30)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
    end)
end)
