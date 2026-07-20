describe('Flashbulb mob skill', function()
    it('adds enmity only for trusts and always applies Flash with its host message', function()
        local bulb = require('scripts/actions/mobskills/flashbulb')
        local status = xi.mobskills.mobStatusEffectMove
        local enmity, effect, message = nil, nil, nil
        local trusted = { isTrust = function() return true end }
        local ordinary = { isTrust = function() return false end }
        local target = { addEnmity = function(_, ...) enmity = { ... } end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) effect = { ... }; return 123 end
        assert(bulb.onMobSkillCheck(target, trusted, skill) == 0 and bulb.onMobWeaponSkill(trusted, target, skill, nil) == xi.effect.FLASH)
        assert(enmity[1] == trusted and enmity[2] == 180 and enmity[3] == 1280 and effect[1] == xi.effect.FLASH and effect[2] == 0 and effect[3] == 0 and effect[4] == 15 and message == 123)
        enmity = nil
        assert(bulb.onMobWeaponSkill(ordinary, target, skill, nil) == xi.effect.FLASH and enmity == nil)
        xi.mobskills.mobStatusEffectMove = status
    end)
end)
