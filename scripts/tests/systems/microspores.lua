require('scripts/actions/mobskills/microspores')
describe('Microspores mob skill', function()
    it('transfers one random removable ailment', function()
        local skill = require('scripts/actions/mobskills/microspores')
        local msg, actions = nil, {}
        local has = { [xi.effect.POISON] = { id = xi.effect.POISON }, [xi.effect.SLOW] = { id = xi.effect.SLOW } }
        local origShuffle = utils.shuffle
        utils.shuffle = function(t)
            -- put POISON first
            return { xi.effect.POISON, xi.effect.SLOW }
        end
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
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 1)
        assert(msg == xi.msg.basic.NONE)
        assert(actions[1][1] == 'copy' and actions[1][2] == xi.effect.POISON)
        assert(actions[2][1] == 'del' and actions[2][2] == xi.effect.POISON)
        utils.shuffle = origShuffle
    end)
end)
