#include "test_automaton_attachment_policy_2716.h"
#include "map/entities/automaton_attachment_policy.h"
#include <iostream>
auto runAutomatonAttachmentPolicy2716SelfTests()->bool{std::array<uint8,12>a{};a[0]=1;a[11]=255;bool ok=automatonattachmenthelpers::HasAttachment(a,1)&&automatonattachmenthelpers::HasAttachment(a,255)&&!automatonattachmenthelpers::HasAttachment(a,2)&&automatonattachmenthelpers::HasAttachment(a,0);if(!ok)std::cerr<<"automaton attachment 2716 failed\n";return ok;}
