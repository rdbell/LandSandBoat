describe('Gloeosuccus mob skill',function()
 it('forwards its Slow status message and returns Slow',function()
  local gloeosuccus=require('scripts/actions/mobskills/gloeosuccus');local status=xi.mobskills.mobStatusEffectMove;local args,message=nil,nil;local skill={setMsg=function(_,v)message=v end};xi.mobskills.mobStatusEffectMove=function(...)args={...};return 456 end
  assert(gloeosuccus.onMobSkillCheck({}, {}, skill)==0 and gloeosuccus.onMobWeaponSkill({}, {}, skill,{})==xi.effect.SLOW)
  xi.mobskills.mobStatusEffectMove=status;assert(message==456 and args[3]==xi.effect.SLOW and args[4]==1250 and args[5]==0 and args[6]==180)
 end)
end)
