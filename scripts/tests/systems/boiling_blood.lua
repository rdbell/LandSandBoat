require('scripts/actions/mobskills/boiling_blood')

describe('Boiling Blood mob skill', function()
    it('allows use, applies Haste then Berserk, emits NONE, and returns zero', function()
        local boilingBlood = require('scripts/actions/mobskills/boiling_blood')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buffs, message = {}, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            table.insert(buffs, { target, effect, power, tick, duration })
        end
        assert(boilingBlood.onMobSkillCheck(nil, mob, skill) == 0)
        assert(boilingBlood.onMobWeaponSkill(mob, nil, skill, nil) == 0)
        xi.mobskills.mobBuffMove = originalBuffMove
        assert(#buffs == 2 and buffs[1][1] == mob and buffs[2][1] == mob)
        assert(buffs[1][2] == xi.effect.HASTE and buffs[1][3] == 2500 and buffs[1][4] == 0 and buffs[1][5] == 180)
        assert(buffs[2][2] == xi.effect.BERSERK and buffs[2][3] == 50 and buffs[2][4] == 0 and buffs[2][5] == 180)
        assert(message == xi.msg.basic.NONE)
    end)
end)
