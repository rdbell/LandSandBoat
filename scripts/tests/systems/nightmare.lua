require('scripts/actions/mobskills/nightmare')
describe('Nightmare mob skill', function()
    it('applies tiered SLEEP_I and Bio after silent Dia/Bio delete', function()
        local skill = require('scripts/actions/mobskills/nightmare')
        local msg, bio, deleted = nil, nil, {}
        local orig = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(mob, target, effect, power, tick, duration, _, _, tier)
            assert(effect == xi.effect.SLEEP_I and power == 1 and duration == 90 and tier == 11)
            return 242
        end
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            delStatusEffectSilent = function(_, e) deleted[#deleted+1] = e end,
            addStatusEffect = function(_, e, opts) bio = { e, opts.power, opts.duration, opts.tick, opts.subPower, opts.tier } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.SLEEP_I)
        assert(msg == 242)
        assert(deleted[1] == xi.effect.DIA and deleted[2] == xi.effect.BIO)
        assert(bio[1] == xi.effect.BIO and bio[2] == 25 and bio[3] == 90 and bio[4] == 3 and bio[5] == 10 and bio[6] == 11)
        xi.mobskills.mobStatusEffectMove = orig
    end)
end)
