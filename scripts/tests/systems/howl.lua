describe('Howl mob skill',function()
 it('applies Warcry to its target, forwards the buff message, and returns Warcry',function()
  local howl=require('scripts/actions/mobskills/howl');local buff=xi.mobskills.mobBuffMove;local call,message=nil,nil;local mob,target={},{};local skill={setMsg=function(_,value)message=value end};xi.mobskills.mobBuffMove=function(...)call={...};return 456 end
  assert(howl.onMobSkillCheck(target,mob,skill)==0 and howl.onMobWeaponSkill(mob,target,skill,{})==xi.effect.WARCRY);xi.mobskills.mobBuffMove=buff;assert(call[1]==target and call[2]==xi.effect.WARCRY and call[3]==25 and call[4]==0 and call[5]==180 and message==456)
 end)
end)
