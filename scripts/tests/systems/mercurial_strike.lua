require('scripts/actions/mobskills/mercurial_strike')
describe('Mercurial Strike mob skill', function()
    it('picks patterned damage and stores localVar', function()
        local skill = require('scripts/actions/mobskills/mercurial_strike')
        local damage, localVar = nil, nil
        local origEntry = utils.randomEntry
        utils.randomEntry = function(t)
            assert(#t == 10 and t[1] == 111 and t[10] == 1111)
            return 555
        end
        local mob = {
            setLocalVar = function(_, k, v) localVar = { k, v } end,
        }
        local target = {
            takeDamage = function(_, v, _, atk, dmg)
                damage = { v, atk, dmg }
            end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 555)
        assert(localVar[1] == 'MERCURIAL_STRIKE_DAMAGE' and localVar[2] == 555)
        assert(damage[1] == 555 and damage[2] == xi.attackType.PHYSICAL and damage[3] == xi.damageType.BLUNT)
        utils.randomEntry = origEntry
    end)
end)
