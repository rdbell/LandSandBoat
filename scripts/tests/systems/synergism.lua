require('scripts/actions/mobskills/synergism')
describe('Synergism mob skill', function()
    it('steals HP from nearby targets when cast on self', function()
        local skill = require('scripts/actions/mobskills/synergism')
        local msg, healed, damaged = nil, 0, 0
        local flan = {
            getID = function() return 2 end,
            getHP = function() return 500 end,
            takeDamage = function(_, a) damaged = damaged + a end,
        }
        local mob = {
            getID = function() return 1 end,
            getMaxHP = function() return 1000 end,
        }
        local orig = xi.mobskills.mobHealMove
        xi.mobskills.mobHealMove = function(m, a) healed = a end
        local sk = {
            getTotalTargets = function() return 2 end,
            getMobHPP = function() return 50 end,
            getTargets = function() return { mob, flan } end,
            setMsg = function(_, m) msg = m end,
        }
        assert(skill.onMobSkillCheck(mob, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, mob, sk, {}) == 0)
        assert(damaged == 250 and healed == 250 and msg == xi.msg.basic.NONE)
        xi.mobskills.mobHealMove = orig
    end)
end)
