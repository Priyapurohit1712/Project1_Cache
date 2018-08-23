#include<stdio.h>
#include<math.h>

unsigned long long miss_L2,miss_L3;

struct L2_cache_entry
{
  unsigned long long tag;
  int set;
  int counter=-1;
  bool valid;
  
}L2_Cache[8192];

int find_victim(int l2_set,unsigned long long l2_tag);
{  int line_start=set*8;
  int line_end=(set+1)*8-1;
  int min_counter=L2Cache[line_start].counter;
  int index=line_start;
  while(line_start!=line_end)
  {
     if(L2Cache[line_start].counter<min_counter)
     {
        min_counter=L2Cache[line_start].counter;
        index=line_start;
        line_start++;
      } 
    if(min_counter==-1)
    {
      L2Cache[index].tag=l2_tag;
      L2Cache[index].set=l2_set;
      L2Cache[index].counter=7;
      Maintain_counter();
      
      return(-1)
     }
     return(line_start);
   }
}
void evict(int victim_line_number,unsigned long long L3_addr,unsigned long long l2_tag,long l2_set)
{   miss_L2++;
    unsigned long long  tag=L2_Cache[victim_line_number].tag;  
    long  set=L2_Cache[victim_line_number].set;
    tag=tag<<10;
    unsigned long long address_L3=tag|set;
    L2_Cache[victim_line_number].tag=l2_tag;
    L2_Cache[victim_line_number].set=l2_set;
    L2_Cache[victim_line_number].counter=7;
    Maintain_counter();
   //L3_Cache_lookup(); // Call L3 with replaced address if exclusive and with current addr if inclusive

}


void Maintain_counter(int set,int line)
{ int line_start=set*8;
  int line_end=(set+1)*8-1;
  while(line_start!=line_end)
  {  if(line_start!=line)
     { if(L2_Cache[line_start].counter!=-1)
        {  
          L2_Cache[line_start].counter--;   // handle counter 0 case not done
          line_start++;
        }
     }
  }
  
}
unsigned long long Find_tag_L2(unsigned long long address)
{  unsigned long long mask=(pow(2,48))-1;
   address=address>>16;
   unsigned long long tag=address&mask; 
   return(tag);

}

long Find_set_L2(unsigned long long address)
{ 
   long mask=pow(2,16)-pow(2,6)-1;
   address=address>>6;
   long set=address&mask;
   return(set);
}

bool check_presence_L2(long l2_set,unsigned long long l2_tag)
{
    int line_start=set*8;
    int line_end=(set+1)*8-1;
    bool found_flag=false;
    for(int i=line_start;i<=line_end;i++)
    { 
      if(L2_Cache[i].tag==l2_tag)
       { L2_Cache[i].counter=7;
         Maintain_counter(l2_set,i);
         found_flag=true;
        }
      if(found_flag==true) return(true);
    }
 if(found_flag==false)
{
   int victim_line_number=find_victim(l2_set,l2_tag);
    if(victim_line_number==-1)
   {miss_L2++;
    return(true);}  
}               // Find victim and update resp tag
 else return(false);
}

}

 int main (int argc, char *argv[]) 
{


      int numtraces = atoi(argv[2]);
      for (int k=0; k<numtraces; k++) {
      sprintf(input_name, "%s_%d", argv[1], k);
      fp = fopen(input_name, "rb");
      assert(fp != NULL);

      while (!feof(fp)) {
         fread(&iord, sizeof(char), 1, fp);
         fread(&type, sizeof(char), 1, fp);
         fread(&addr, sizeof(unsigned long long), 1, fp);
         fread(&pc, sizeof(unsigned), 1, fp);
          
  	  if(type!=0)
          {
               long l2_set=Find_set_L2(addr);
               unsigned long long l2_tag=Find_tag_L2(addr);
               bool is_present_L2=check_presence_L2(long l2_set,unsigned long long l2_tag);
    		if(is_present_L2==false)
                { 
                       int victim_line_number=find_victim(l2_set,l2_tag); 
  		       evict(victim_line_number,addr,l2_tag,l2_set);

                 }

              
           }

         // Process the entry
      }
      fclose(fp);
      printf("Done reading file %d!\n", k);
}


























}
