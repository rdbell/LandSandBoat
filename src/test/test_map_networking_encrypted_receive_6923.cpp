#include "test_map_networking_encrypted_receive_6923.h"

#include "map/map_networking_encrypted_receive.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking encrypted receive 6923 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingEncryptedReceive6923SelfTests() -> bool
{
    using mapnetworkinghelpers::IncomingDecryptionPlan;
    using mapnetworkingencryptedreceivehelpers::MakePlan;

    bool ok = true;

    auto plan = MakePlan(true, true, true);
    ok = expect(plan.decryption == IncomingDecryptionPlan::UsePrimary && plan.decryptCount == 0 && !plan.clearOutput && plan.markCurrentKeyDecryption,
                "primary decrypt wins and marks current key") &&
         ok;

    plan = MakePlan(false, true, true);
    ok = expect(plan.decryption == IncomingDecryptionPlan::UsePrevious && plan.decryptCount == 1 && !plan.clearOutput && !plan.markCurrentKeyDecryption,
                "pending zone previous decrypt succeeds without current mark") &&
         ok;

    plan = MakePlan(false, false, true);
    ok = expect(plan.decryption == IncomingDecryptionPlan::Reject && plan.decryptCount == -1 && plan.clearOutput && !plan.markCurrentKeyDecryption,
                "previous key is rejected outside pending zone") &&
         ok;

    return ok;
}
