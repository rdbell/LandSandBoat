require('scripts/actions/mobskills/daybreak')
describe('Daybreak mob skill', function()
    it('uses Light plan and self restore mods', function()
        local skill = require('scripts/actions/mobskills/daybreak')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, hp, mp, tp, mod, anim, localVar = nil, nil, nil, nil, nil, nil, nil, nil
        local mob = {
            getMainLvl = function() return 50 end,
            eraseAllStatusEffect = function() end,
            addHP = function(_, v) hp = v end,
            addMP = function(_, v) mp = v end,
            setMod = function(_, m, v) mod = { m, v } end,
            setTP = function(_, v) tp = v end,
            setLocalVar = function(_, k, v) localVar = { k, v } end,
        }
        local sk = {
            setFinalAnimationSub = function(_, v) anim = v end,
        }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.fTP[1] == 7 and params.element == xi.element.LIGHT)
        assert(hp == 350 and mp == 350 and tp == 0 and anim == 5)
        assert(mod[1] == xi.mod.DMGPHYS and mod[2] == -5000)
        assert(localVar[1] == 'DaybreakUsed' and localVar[2] == 1)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
