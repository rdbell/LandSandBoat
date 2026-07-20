require('scripts/actions/mobskills/circle_blade')
describe('Circle Blade mob skill', function()
    it('emits READIES_WS and uses physical plan', function()
        local skill = require('scripts/actions/mobskills/circle_blade')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, msg = nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 50 end,
            messageBasic = function(_, m, p2, p3) msg = { m, p2, p3 } end,
        }
        local target = { takeDamage = function(_, v) damage = v end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(msg[1] == xi.msg.basic.READIES_WS and msg[3] == 38)
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 1.0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
    end)
end)
