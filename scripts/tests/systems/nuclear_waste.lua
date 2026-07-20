require('scripts/actions/mobskills/nuclear_waste')
describe('Nuclear Waste mob skill', function()
    it('applies ELEMENTALRES_DOWN when resist >= 0.25', function()
        local skill = require('scripts/actions/mobskills/nuclear_waste')
        local msg, effect, localVar = nil, nil, nil
        local orig = xi.combat.magicHitRate.calculateResistRate
        xi.combat.magicHitRate.calculateResistRate = function() return 0.5 end
        local mob = { setLocalVar = function(_, k, v) localVar = { k, v } end }
        local target = {
            addStatusEffect = function(_, e, opts) effect = { e, opts.power, opts.duration } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.ELEMENTALRES_DOWN)
        assert(localVar[1] == 'nuclearWaste' and localVar[2] == 1)
        assert(effect[1] == xi.effect.ELEMENTALRES_DOWN and effect[2] == 50 and effect[3] == 60)
        assert(msg == xi.msg.basic.NONE)
        xi.combat.magicHitRate.calculateResistRate = function() return 0.1 end
        effect, msg = nil, nil
        skill.onMobWeaponSkill(mob, target, sk, {})
        assert(msg == xi.msg.basic.SKILL_MISS and effect == nil)
        xi.combat.magicHitRate.calculateResistRate = orig
    end)
end)
