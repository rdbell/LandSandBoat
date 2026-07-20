require('scripts/actions/mobskills/horrid_roar_2')
describe('Horrid Roar 2 mob skill', function()
    it('dispels all and lowers enmity 45', function()
        local skill = require('scripts/actions/mobskills/horrid_roar_2')
        local msg, lower, behind, anim = nil, nil, true, 0
        local mob = {
            getAnimationSub = function() return anim end,
            lowerEnmity = function(_, _, pct) lower = pct end,
        }
        local target = {
            isBehind = function() return behind end,
            dispelAllStatusEffect = function(_, flags)
                assert(flags == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD))
                return 3
            end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        behind = false
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 3)
        assert(msg == xi.msg.basic.DISAPPEAR_NUM and lower == 45)
    end)
end)
