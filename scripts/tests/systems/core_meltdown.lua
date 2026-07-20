require('scripts/actions/mobskills/core_meltdown')
describe('Core Meltdown mob skill', function()
    it('gates NM/form/HP/random and uses Fire elemental plan with finalize death', function()
        local skill = require('scripts/actions/mobskills/core_meltdown')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, hp, roll = nil, nil, 200, 3
        local origRandom = math.random
        math.random = function(a, b)
            if a == 1 and b == 100 then return roll end
            return origRandom(a, b)
        end
        local mob = {
            isMobType = function(_, t) return t == xi.mobType.NOTORIOUS end,
            getAnimationSub = function() return 0 end,
            getHPP = function() return 20 end,
            setHP = function(_, v) hp = v end,
        }
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        mob.isMobType = function() return false end
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        roll = 50
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        roll = 3
        local sk = { getMobHP = function() return 200 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=50, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.ELEMENTAL } end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 50)
        assert(params.baseDamage == 100 and params.element == xi.element.FIRE)
        assert(damage == 50)
        skill.onMobSkillFinalize(mob, sk)
        assert(hp == 0)
        math.random = origRandom
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
