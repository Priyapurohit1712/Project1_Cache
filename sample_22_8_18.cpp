#include<stdio.h>
#include<math.h>
#define L2_asso 8
#define L2_asso 16

unsigned long long miss_L2,miss_L3;
char clustivity; //i e or n

struct L2_cache_entry
{
  unsigned long long tag;
  int set;                 //L2 cache
  int counter=-1;
  bool valid=0;
  
}L2_Cache[8192];

struct L3_cache_entry
{
  unsigned long long tag;
  int set;
  int counter=-1;
  bool valid;
  
}L3_Cache[32768];

void invalidateL3(unsigned long long inv_tag,int inv_set)
{
  int line_start=inv_set*16;
  int line_end=(inv_set+1)*16-1;
  int prev_count;

   while(line_start!=line_end)
      {
         if(L3Cache[line_start].tag==inv_tag)
         { 
           prev_count=L3Cache[line_start].counter;
	   L3Cache[line_start].valid=0;
	   L3Cache[line_start].counter=-1; 
	   Maintain_counter(inv_set,line_start,prev_count,16);
	  return;
          }  
            line_start++;
       }
}

void Backinvalidate_L2(unsigned long long inv_tag,int inv_set)
{
  unsigned long long L2_inv_add;
  inv_tag=inv_tag<<11;
  L2_inv_add=inv_tag|inv_set;
  unsigned long long L2_inv_tag=L2_inv_add>>10;
  long mask=pow(2,10)-1;  
  int L2_inv_set=inv_set&mask;
  int line_start=L2_inv_set*8;
  int line_end=(L2_inv_set+1)*8-1;
  int prev_count;
      
      while(line_start!=line_end)
      {
         if(L2Cache[line_start].tag==L2_inv_tag)
         { 
           prev_count=L2Cache[line_start].counter;
	   L2Cache[line_start].valid=0;
	   L2Cache[line_start].counter=-1; 
	   Maintain_counter(L2_inv_set,line_start,prev_count,8);
	  return;
          }  
        line_start++;
       }

  

}

void Updateline_L2(unsigned long long tag,long set,int line)
{
  int prev_counter=L2_Cache[line].counter;
  L2_Cache[line].tag=tag;
  L2_Cache[line].set=set;
  L2_Cache[line].valid=1;
  L2_Cache[line].counter=7;
  Maintain_counter(set,line,prev_counter,L2_asso);

}
void Updateline_L3(unsigned long long tag,long set,int line)
{
  int prev_counter=L3_Cache[line].counter;
  L3_Cache[line].tag=tag;
  L3_Cache[line].set=set;
  L3_Cache[line].valid=1;
  L3_Cache[line].counter=15;
  Maintain_counter(set,line,prev_counter,L3_asso);
}

unsigned long long Findtag_L3(unsigned long long address)
{
   unsigned long long tag=address>>17; 
   return(tag);


}

long Findset_L3(unsigned long long address)
{ 
    long mask=pow(2,17)-pow(2,6)-1;
   address=address>>6;
   long set=address&mask;
   return(set);
}



unsigned long long Findtag_L2(unsigned long long address)
{ 
   
   unsigned long long tag=address>>16; 
   return(tag);

}

long Findset_L2(unsigned long long address)
{ 
   long mask=pow(2,16)-pow(2,6)-1;
   address=address>>6;
   long set=address&mask;
   return(set);
}


void Maintain_counter(int set,int line,int previous_count,int level_asso)
{ int line_start=set*level_asso;
  int line_end=((set+1)*level_asso)-1;
 if(level_asso==8)
  {
      while(line_start!=line_end)
      {  
         
        if((L2_Cache[line_start].counter>previous_count) && (line_start!=line))
          L2_Cache[line_start].counter--;

	  line_start++; 
      }
  }

else if(level_asso==16)
  {
     while(line_start!=line_end)
      {  
         
        if((L3_Cache[line_start].counter>previous_count) && (line_start!=line))
          L3_Cache[line_start].counter--;

	  line_start++; 
      }
  }
  
}
int find_line(int set,unsigned long long tag,int level_asso)   // find line number with minimum counter
{ 
  int line_start=set*level_asso;
  int line_end=(set+1)*level_asso;
  int min_counter;
  int index=line_start;
if(level_asso==8)
  {
      min_counter=L2Cache[line_start].counter;
      
      while(line_start!=line_end)
      {
         if(L2Cache[line_start].counter<min_counter)
         {
           min_counter=L2Cache[line_start].counter;
           index=line_start;
            line_start++;
          }  
       }
   }                                                   
      
else if(level_asso==16)
  {
      min_counter=L3_Cache[line_start].counter;
      while(line_start!=line_end)
      {
         if(L3_Cache[line_start].counter<min_counter)
         {
           min_counter=L3_Cache[line_start].counter;
           index=line_start;
            line_start++;
          } 
        
      }
    
   }
 return(index);
}

bool check_presence(long set,unsigned long long tag,int level_asso)
{
    int line_start=set*level_asso;
    int line_end=(set+1)*level_asso-1;
    bool found_flag=false;
    int prev_counter;
    if(level_asso==8)
    {
            	for(int i=line_start;i<=line_end;i++)
           	 { 
             		if(L2_Cache[i].tag==tag && L2_Cache[i].valid==1)
              		{ 
                	  prev_counter=L2_Cache[i].counter;    //L2 Present case handled
	       	          L2_Cache[i].counter=7;
			  L2_Cache[i].valid=1;
                          Maintain_counter(set,i,prev_counter,L2_asso);
                          found_flag=true;
                         }
                        if(found_flag==true) return(true);
 		}
 	 	if(found==false)
		{ 
 	  	  miss_L2++;
          	  return(false);
		}

      }      

           
 else if(level_asso==16)
    {
        
            for(int i=line_start;i<=line_end;i++)
            { 
             if(L3_Cache[i].tag==tag && L3_Cache[i].valid==1)
              { 
                prev_counter=L3_Cache[i].counter;    //L3 Present case handled
	        L3_Cache[i].counter=15;
		L3_Cache[i].valid=1;
                Maintain_counter(set,i,prev_counter,L3_asso); // L3 present maintained handled
                found_flag=true;   //changes required as per ass0 8
              }
             if(found_flag==true) return (true);
            }
           if(found_flag==false)
           {  
              miss_L3++;
              return (false);
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
               long l2_set=Findset_L2(addr);
               unsigned long long l2_tag=Findtag_L2(addr);
               bool is_present_L2=check_presence(l2_set,l2_tag,L2_asso);
    		if(is_present_L2==false)  // Empty line OR Full set
                { 
			int l2_victim=find_line(set,tag,L2_asso); // handle according to clustivity //L2 victim line
 			long l3_set=Findset_L3(addr);
                 	unsigned long long l3_tag=Findtag_L3(addr);    

 
			if(L2_Cache[l2_victim].counter==-1)    //Empty lines case
                        {
                                bool is_present_L3=check_presence(l3_set,l3_tag,L3_asso);
				
				if(clustivity=='i')
				{
					if(is_present_L3==true) //bring to L2
					{
						Updateline_L2(l2_tag,l2_set,l2_victim);

					}
					else if(is_present_L3==false) //bring to L2 and L3 both
					{
						int l3_victim=find_line(set,tag,L3_asso);

						if(L3[l3_victim].counter==-1)  // L3 set is empty so Simply bring in L3 and then in L2
						{
						  Updateline_L3(l3_tag,l3_set,l3_victim);
						 // Maintain_counter(set,l3_victim,prev_counter,L3_asso);

						  Updateline_L2(l2_tag,l2_set,l2_victim);
						 // Maintain_counter(set,l2_victim,prev_counter,L2_asso);
                                                }

						if(L3[l3_victim].counter==0)  //L3 set full,bring in L3,backinvalid in L2,bring new in L2
						{
						  unsigned long long inv_tag=L3_Cache[l3_victim].tag;
						  int inv_set=L3_Cache[l3_victim].set;
						  Updateline_L3(l3_tag,l3_set,l3_victim);
						  //Maintain_counter(set,l3_victim,prev_counter,L3_asso);
                                                  Backinvalidate_L2(inv_tag,inv_set);


		
						  Updateline_L2(l2_tag,l2_set,l2_victim);             // Assuming the victim and invalidated
						  //Maintain_counter(set,l2_victim,prev_counter,L2_asso);//cache line are not from same set.
                                                }


				          }
 				}

                              else if(clustivity=='e')
                               {
					if(is_present_L3==true) //bring to L2 and invalidate in L3
					{       int Invalid_L3line= invalidateL3(l3_set,l3_tag);

						Updateline_L2(l2_tag,l2_set,l2_victim);
						Maintain_counter(set,l2_victim,prev_counter,L2_asso);

					}				



				}
				else(clustivity=='n')
				{

				}

			}
			
			else if(L2[line_number].counter==0)    //Full set case
			{


			}
			

         } 

		  
                       
                 }

              
           }

         // Process the entry
      }
      fclose(fp);
      printf("Done reading file %d!\n", k);
}
