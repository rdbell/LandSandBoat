require('scripts/actions/mobskills/hoof_volley')
describe('Hoof Volley mob skill', function()
    it('uses physical plan and resets enmity when processed', function()
        local skill = require('scripts/actions/mobskills/hoof_volley')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, reset = nil, nil, false
        local mob = { getWeaponDmg = function() return 50 end, resetEnmity = function() reset = true end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.BLUNT } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 4.0 and not reset)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100 and reset)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
    end)
end)
