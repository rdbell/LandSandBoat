describe('Hexagon Belt mob skill',function()
 it('replaces existing Defense Boost then applies its fixed buff',function()
  local belt=require('scripts/actions/mobskills/hexagon_belt');local buff=xi.mobskills.mobBuffMove;local deleted,call,message=nil,nil,nil;local active=true;local mob={hasStatusEffect=function(_,effect)return active and effect==xi.effect.DEFENSE_BOOST end,delStatusEffect=function(_,effect)deleted=effect end};local target={};local skill={setMsg=function(_,value)message=value end};xi.mobskills.mobBuffMove=function(...)call={...};return 456 end
  assert(belt.onMobSkillCheck(target,mob,skill)==0 and belt.onMobWeaponSkill(mob,target,skill,{})==xi.effect.DEFENSE_BOOST);xi.mobskills.mobBuffMove=buff;assert(deleted==xi.effect.DEFENSE_BOOST and call[1]==mob and call[2]==xi.effect.DEFENSE_BOOST and call[3]==21 and call[4]==0 and call[5]==180 and message==456)
 end)
end)
