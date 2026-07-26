#include "test_synth_material_loss_7085.h"

#include "map/synth_material_loss.h"

#include <iostream>

auto runSynthMaterialLoss7085SelfTests() -> bool
{
    using synthmateriallosshelpers::CalculateBreakChance;
    using synthmateriallosshelpers::IngredientOutcome;
    using synthmateriallosshelpers::ResolveIngredient;

    const bool ok = CalculateBreakChance(5, 10, 15, 0) == 20 && CalculateBreakChance(0, 0, 0, 10) == 100 &&
                    CalculateBreakChance(0, 0, 0, -1) == 50 && CalculateBreakChance(-32768, 0, 0, 0) == 100 &&
                    ResolveIngredient(19, 20) == IngredientOutcome::Break &&
                    ResolveIngredient(20, 20) == IngredientOutcome::Save;
    if (!ok)
    {
        std::cerr << "synth material loss 7085 self-test failed\n";
    }
    return ok;
}
