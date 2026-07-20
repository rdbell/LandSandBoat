describe('Great Bleat mob skill',function()
 it('forwards its Max HP Down status message and returns Max HP Down',function()
  local bleat=require('scripts/actions/mobskills/great_bleat');local status=xi.mobskills.mobStatusEffectMove;local args,message=nil,nil;local skill={setMsg=function(_,v)message=v end};xi.mobskills.mobStatusEffectMove=function(...)args={...};return 456 end
  assert(bleat.onMobSkillCheck({}, {}, skill)==0 and bleat.onMobWeaponSkill({}, {}, skill,{})==xi.effect.MAX_HP_DOWN);xi.mobskills.mobStatusEffectMove=status;assert(message==456 and args[3]==xi.effect.MAX_HP_DOWN and args[4]==75 and args[5]==0 and args[6]==120)
 end)
end)
