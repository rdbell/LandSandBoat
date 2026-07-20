require('scripts/actions/mobskills/hex_palm')
describe('Hex Palm mob skill', function()
    it('admits broken-weapon forms and drains HP when processed', function()
        local skill = require('scripts/actions/mobskills/hex_palm')
        local magicalMove, processDamage, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, msg, anim = nil, nil, 0
        local mob = { getAnimationSub = function() return anim end, getMainLvl = function() return 50 end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        anim = 1
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=80, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.NONE } end
        xi.mobskills.mobDrainMove = function(_,_,_,amount) return 55 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 80)
        assert(params.baseDamage == 50 and params.skipMagicBonusDiff and msg == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 80)
        assert(msg == 55)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = magicalMove, processDamage, drainMove
    end)
end)
