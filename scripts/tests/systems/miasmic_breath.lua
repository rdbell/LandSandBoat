require('scripts/actions/mobskills/miasmic_breath')
describe('Miasmic Breath mob skill', function()
    it('sets HIT_DMG for MIASMIC_BREATH_2 and poisons when processed', function()
        local skill = require('scripts/actions/mobskills/miasmic_breath')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, status, sid = nil, nil, nil, 0
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        local sk = { getID = function() return sid end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=80, attackType=xi.attackType.BREATH, damageType=xi.damageType.DARK } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 80)
        assert(params.primaryMessage == nil and status == nil)
        sid = xi.mobSkill.MIASMIC_BREATH_2
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 80)
        assert(params.primaryMessage == xi.msg.basic.HIT_DMG)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 80)
        assert(damage == 80 and status[1] == xi.effect.POISON and status[2] == 50)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
    end)
end)
