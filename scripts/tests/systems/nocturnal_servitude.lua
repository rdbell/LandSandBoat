require('scripts/actions/mobskills/nocturnal_servitude')
describe('Nocturnal Servitude mob skill', function()
    it('charms PCs with bat costume on success and misses non-PCs', function()
        local skill = require('scripts/actions/mobskills/nocturnal_servitude')
        local msg, charmed, costume = nil, false, nil
        local orig = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function() return xi.msg.basic.SKILL_ENFEEB_IS end
        local mob = { charm = function() charmed = true end }
        local target = {
            isPC = function() return true end,
            addStatusEffect = function(_, e, opts) costume = { e, opts.power, opts.duration } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.CHARM_I)
        assert(msg == xi.msg.basic.SKILL_ENFEEB_IS and charmed)
        assert(costume[1] == xi.effect.COSTUME and costume[2] == 256 and costume[3] == 60)
        target.isPC = function() return false end
        msg = nil
        skill.onMobWeaponSkill(mob, target, sk, {})
        assert(msg == xi.msg.basic.SKILL_MISS)
        xi.mobskills.mobStatusEffectMove = orig
    end)
end)
