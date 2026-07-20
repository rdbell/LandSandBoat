require('scripts/actions/mobskills/horrid_roar_3')
describe('Horrid Roar 3 mob skill', function()
    it('gates special buffs and lowers enmity 70', function()
        local skill = require('scripts/actions/mobskills/horrid_roar_3')
        local msg, lower, effects = nil, nil, {}
        local mob = {
            hasStatusEffect = function(_, e) return effects[e] == true end,
            getAnimationSub = function() return 0 end,
            lowerEnmity = function(_, _, pct) lower = pct end,
        }
        local target = {
            isBehind = function() return false end,
            dispelAllStatusEffect = function() return 2 end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        effects[xi.effect.MIGHTY_STRIKES] = true
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        effects = {}
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 2)
        assert(msg == xi.msg.basic.DISAPPEAR_NUM and lower == 70)
    end)
end)
