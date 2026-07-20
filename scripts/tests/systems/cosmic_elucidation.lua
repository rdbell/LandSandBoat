require('scripts/actions/mobskills/cosmic_elucidation')
describe('Cosmic Elucidation mob skill', function()
    it('rejects skill check and uses special Light plan with skillchain message', function()
        local skill = require('scripts/actions/mobskills/cosmic_elucidation')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, msg = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=500, attackType=xi.attackType.SPECIAL, damageType=xi.damageType.ELEMENTAL } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 500)
        assert(params.fTP[1] == 14 and params.attackType == xi.attackType.SPECIAL and msg == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 500)
        assert(damage == 500 and msg == xi.msg.basic.SKILLCHAIN_COSMIC_ELUCIDATION)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
