local trollAutomaton = require('scripts/mixins/families/Troll_Automaton')

describe('Troll Automaton mixin', function()
    it('uses the four fixed frame configurations and model skill pools', function()
        local harlequin = xi.mix.trollAutomaton.framePlan(1, 3)
        assert(harlequin.name == 'Harlequin' and harlequin.job == xi.job.RDM and harlequin.modelId == 1977)
        assert(harlequin.magicCool == 27 and harlequin.magicDelay == 3 and harlequin.spellList == 567 and harlequin.magicCasting)
        assert(harlequin.doubleAttack == 0 and harlequin.standbackHp == 0 and harlequin.specialSkill == 0)

        local valoredge = xi.mix.trollAutomaton.framePlan(2, 4)
        assert(valoredge.name == 'Valoredge' and valoredge.job == xi.job.PLD and valoredge.modelId == 1983)
        assert(valoredge.doubleAttack == 15 and valoredge.magicDelay == 4 and valoredge.spellList == 0 and not valoredge.magicCasting)

        local sharpshot = xi.mix.trollAutomaton.framePlan(3, 5)
        assert(sharpshot.name == 'Sharpshot' and sharpshot.job == xi.job.RNG and sharpshot.modelId == 1990)
        assert(sharpshot.standbackHp == 70 and sharpshot.specialCool == 12 and sharpshot.specialSkill == xi.mobSkill.RANGED_ATTACK_1 and sharpshot.rangedAttackRange == 13)

        local stormwalker = xi.mix.trollAutomaton.framePlan(4, 7)
        assert(stormwalker.name == 'Stormwalker' and stormwalker.job == xi.job.RDM and stormwalker.modelId == 1994)
        assert(stormwalker.magicDelay == 7 and stormwalker.spellList == 566 and stormwalker.magicCasting and stormwalker.standbackHp == 70)
        assert(xi.mix.trollAutomaton.framePlan(5, 3) == nil)

        local valoredgeSkills = xi.mix.trollAutomaton.skillPool(1983)
        assert(#valoredgeSkills == 3 and valoredgeSkills[1] == xi.mobSkill.CHIMERA_RIPPER_AUTOMATON and valoredgeSkills[3] == xi.mobSkill.SHIELD_BASH_AUTOMATON)
        local sharpshotSkills = xi.mix.trollAutomaton.skillPool(1990)
        assert(#sharpshotSkills == 2 and sharpshotSkills[1] == xi.mobSkill.SLAPSTICK_AUTOMATON and sharpshotSkills[2] == xi.mobSkill.ARCUBALLISTA_AUTOMATON)
        assert(#xi.mix.trollAutomaton.skillPool(1977) == 1 and #xi.mix.trollAutomaton.skillPool(1994) == 1 and #xi.mix.trollAutomaton.skillPool(0) == 0)
    end)
end)
