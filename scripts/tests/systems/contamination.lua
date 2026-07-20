require('scripts/actions/mobskills/contamination')
describe('Contamination mob skill', function()
    it('transfers removable ailments from mob to target', function()
        local skill = require('scripts/actions/mobskills/contamination')
        local msg, actions = nil, {}
        local has = {
            [xi.effect.POISON] = { id = xi.effect.POISON },
            [xi.effect.SLOW] = { id = xi.effect.SLOW },
        }
        local mob = {
            hasStatusEffect = function(_, e) return has[e] ~= nil end,
            getStatusEffect = function(_, e) return has[e] end,
            delStatusEffect = function(_, e)
                actions[#actions+1] = { 'del', e }
                has[e] = nil
            end,
        }
        local target = {
            copyStatusEffect = function(_, effect) actions[#actions+1] = { 'copy', effect.id } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 2)
        assert(msg == xi.msg.basic.NONE)
        assert(#actions == 4)
    end)
end)
