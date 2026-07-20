require('scripts/actions/mobskills/dream_shroud')

describe('Dream Shroud mob skill', function()
    it('replaces both magic boosts with complementary Vana\'diel-hour powers', function()
        local dreamShroud = require('scripts/actions/mobskills/dream_shroud')
        local buffMove = xi.mobskills.mobBuffMove
        local deleted, buffs, message = {}, {}, nil
        local mob = {
            delStatusEffect = function(_, effect)
                table.insert(deleted, effect)
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            table.insert(buffs, { ... })
        end

        local hour = 12
        stub('VanadielHour', function() return hour end)
        assert(dreamShroud.onMobSkillCheck({}, {}, {}) == 0)
        assert(dreamShroud.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.MAGIC_ATK_BOOST)
        assert(deleted[1] == xi.effect.MAGIC_ATK_BOOST and deleted[2] == xi.effect.MAGIC_DEF_BOOST)
        assert(buffs[1][2] == xi.effect.MAGIC_ATK_BOOST and buffs[1][3] == 1 and buffs[1][4] == 0 and buffs[1][5] == 180)
        assert(buffs[2][2] == xi.effect.MAGIC_DEF_BOOST and buffs[2][3] == 13 and buffs[2][4] == 0 and buffs[2][5] == 180)
        assert(message == xi.msg.basic.SKILL_RECEIVES_MAB_MDB)

        deleted, buffs = {}, {}
        hour = 0
        assert(dreamShroud.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.MAGIC_ATK_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(deleted[1] == xi.effect.MAGIC_ATK_BOOST and deleted[2] == xi.effect.MAGIC_DEF_BOOST)
        assert(buffs[1][2] == xi.effect.MAGIC_ATK_BOOST and buffs[1][3] == 13 and buffs[1][4] == 0 and buffs[1][5] == 180)
        assert(buffs[2][2] == xi.effect.MAGIC_DEF_BOOST and buffs[2][3] == 1 and buffs[2][4] == 0 and buffs[2][5] == 180)
    end)
end)
