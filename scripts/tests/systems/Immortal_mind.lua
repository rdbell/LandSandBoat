require('scripts/actions/mobskills/Immortal_mind')
describe('Immortal Mind mob skill', function()
    it('stacks MAB/MDB powers by +10 and messages first buff', function()
        local skill = require('scripts/actions/mobskills/Immortal_mind')
        local buffMove = xi.mobskills.mobBuffMove
        local buffs, message = {}, nil
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            table.insert(buffs, { effect, power, tick, duration })
            return 100 + #buffs
        end
        local mob = {
            getStatusEffect = function(_, effect)
                if effect == xi.effect.MAGIC_ATK_BOOST then
                    return { getPower = function() return 15 end }
                end
                return nil
            end,
        }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.MAGIC_ATK_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(#buffs == 2)
        assert(buffs[1][1] == xi.effect.MAGIC_ATK_BOOST and buffs[1][2] == 25 and buffs[1][4] == 180)
        assert(buffs[2][1] == xi.effect.MAGIC_DEF_BOOST and buffs[2][2] == 10 and buffs[2][4] == 180)
        assert(message == 101)
    end)
end)
