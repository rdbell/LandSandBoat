require('scripts/actions/mobskills/barrier_tusk')

describe('Barrier Tusk mob skill', function()
    it('requires an intact tusk and applies magic defense before defense', function()
        local barrierTusk = require('scripts/actions/mobskills/barrier_tusk')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buffs, message = {}, nil
        local mob = { getAnimationSub = function() return 0 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            table.insert(buffs, { target, effect, power, tick, duration })
            return #buffs + 100
        end

        assert(barrierTusk.onMobSkillCheck(nil, mob, skill) == 0)
        mob.getAnimationSub = function() return 1 end
        assert(barrierTusk.onMobSkillCheck(nil, mob, skill) == 1)
        assert(barrierTusk.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = originalBuffMove

        assert(#buffs == 2 and buffs[1][1] == mob and buffs[2][1] == mob)
        assert(buffs[1][2] == xi.effect.MAGIC_DEF_BOOST and buffs[2][2] == xi.effect.DEFENSE_BOOST)
        assert(buffs[1][3] == 30 and buffs[1][4] == 0 and buffs[1][5] == 90)
        assert(buffs[2][3] == 30 and buffs[2][4] == 0 and buffs[2][5] == 90 and message == 102)
    end)
end)
