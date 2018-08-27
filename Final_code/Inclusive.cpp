#include<stdbool.h>
#include<stdio.h>
#include<math.h>
#include<assert.h>
#include<stdlib.h>
#define L2_asso 8
#define L3_asso 16

unsigned long long miss_L2,miss_L3;

struct L2_cache_entry
{
  unsigned long long tag;
  int set;                 //L2 cache
  int counter;
  bool valid;
  
}L2_Cache[8192];

struct L3_cache_entry
{
  unsigned long long tag;
  int set;
  int counter;
  bool valid;
  
}L3_Cache[32768];
 

void Initialization()
{
 for(int i=0 ;i<8192;i++)
  {
   L2_Cache[i].tag = -1;
   L2_Cache[i].valid = 0; 
   L2_Cache[i].counter = -1;
  }

   for(int j = 0;j<32768;j++)
   {
	L3_Cache[j].tag = -1;
        L3_Cache[j].valid = 0;
        L3_Cache[j].counter = -1;


   }
}

void Maintain_counter(int set,int line,int previous_count,int level_asso);
void Invalidate_L2_MaintainCounter(int set,int line,int prev_counter);

void invalidateL3(unsigned long long inv_tag,int inv_set)
{
  
  int line_start=inv_set*16;
  int line_end=(inv_set+1)*16-1;
  int prev_count;

   while(line_start<=line_end)
      {
         if(L3_Cache[line_start].tag==inv_tag)
         { 
           prev_count=L3_Cache[line_start].counter;
	   L3_Cache[line_start].valid=0;
	   L3_Cache[line_start].counter=-1; 
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
  int line_end=((L2_inv_set+1)*8)-1;
  int prev_count;
      
      while(line_start<=line_end)
      {
         if(L2_Cache[line_start].tag==L2_inv_tag)
         { 
           prev_count=L2_Cache[line_start].counter;
	   L2_Cache[line_start].valid=0;
	   L2_Cache[line_start].counter=-1; 
	   Invalidate_L2_MaintainCounter(L2_inv_set,line_start,prev_count);
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
    address=address>>6;
    long mask=pow(2,11)-1;
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
  
   address=address>>6;
   long mask=pow(2,10)-1;
   long set=address&mask;
   
   return(set);
}


void Maintain_counter(int set,int line,int previous_count,int level_asso)
{ 
  
  int line_start=set*level_asso;
  int line_end=((set+1)*level_asso)-1;
 if(level_asso==8)
  {
      while(line_start<=line_end)
      {  
         
        if((L2_Cache[line_start].counter>previous_count) && (line_start!=line))
          L2_Cache[line_start].counter--;

	  line_start++; 
      }
  }

else if(level_asso==16)
  {
     while(line_start<=line_end)
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
  int line_end=((set+1)*level_asso) - 1;
  int min_counter;
  int index=line_start;
if(level_asso==8)
  {  
      min_counter=L2_Cache[line_start].counter;
      
      while(line_start<=line_end)
      {   
         if(L2_Cache[line_start].counter<min_counter)
         {
           min_counter=L2_Cache[line_start].counter;
           index=line_start;
            
          }
        line_start++;  
       }
   }                                                   
      
else if(level_asso==16)
  {
      min_counter=L3_Cache[line_start].counter;
      while(line_start<=line_end)
      {
         if(L3_Cache[line_start].counter<min_counter)
         {
           min_counter=L3_Cache[line_start].counter;
           index=line_start;
            
          }
        line_start++; 
        
      }
    
   }
 return(index);
}

bool check_presence(long set,unsigned long long tag,int level_asso)
{
    
    
    int line_start=set*level_asso; 
    int line_end=((set+1)*level_asso)-1;
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
                        if(found_flag) return(true);
 		}
 	 	if(!found_flag)
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
             if(found_flag) return (true);
            }
           if(!found_flag)
           {  
              miss_L3++;
              return (false);
            }               

      }



}



void Invalidate_L2_MaintainCounter(int set,int line,int prev_counter)
{
  
  int line_start=set*L2_asso;
  int line_end=((set+1)*L2_asso)-1;

      while(line_start<=line_end)
      {  
         
        if((L2_Cache[line_start].counter<prev_counter) && (line_start!=line) && (L2_Cache[line_start].counter!=-1) )
          L2_Cache[line_start].counter++;

	  line_start++; 
      }
  
}


int main(int argc, char*argv[])
{ 

  // assert(argc < 2); 
    
    Initialization();
    char iord,type;
   unsigned long long addr;
   unsigned int pc;  
   char input_name[50]; 
   FILE *fp; 
   int numtraces = atoi(argv[2]);
    
   for (int k=0; k<numtraces; k++) 
   {
      sprintf(input_name, "%s_%d", argv[1], k);
      fp = fopen(input_name, "rb");
      assert(fp != NULL);   
 
 
   // unsigned long long k=10000;

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
			int l2_victim=find_line(l2_set,l2_tag,L2_asso); //L2 victim line
           		long l3_set=Findset_L3(addr);
                 	unsigned long long l3_tag=Findtag_L3(addr);

 
			if((L2_Cache[l2_victim].counter==-1) || (L2_Cache[l2_victim].counter==0))    //Empty lines case & Full line case
                        {
                                bool is_present_L3=check_presence(l3_set,l3_tag,L3_asso);  
				
					   if(is_present_L3==true) //bring to L2
					   {    
					      Updateline_L2(l2_tag,l2_set,l2_victim);
                                               

					    }
					    else if(is_present_L3==false) //bring to L2 and L3 both
					     {
						  int l3_victim=find_line(l3_set,l3_tag,L3_asso);

						   if(L3_Cache[l3_victim].counter==-1)  // L3 set is empty so Simply bring in L3 and then in L2
						    {
						      Updateline_L3(l3_tag,l3_set,l3_victim);
						      Updateline_L2(l2_tag,l2_set,l2_victim);
						      
                                                     }

						  if(L3_Cache[l3_victim].counter==0)  //L3 set full,bring in L3,backinvalid in L2,bring new in L2
						   {
						     unsigned long long inv_tag=L3_Cache[l3_victim].tag;
						     int inv_set=L3_Cache[l3_victim].set;
						     Updateline_L3(l3_tag,l3_set,l3_victim);
						     Backinvalidate_L2(inv_tag,inv_set);

                                                       int l2_new_victim=find_line(l2_set,l2_tag,L2_asso);
 						       Updateline_L2(l2_tag,l2_set,l2_new_victim);  // starting from bottom 
                                                   }


				             }


			}

              

         } 

		  
                       
      }

              
       // break;
	// k--; 
    }   

 
   fclose(fp);
}

  printf("L2 Misses = %llu\n",miss_L2);
  printf("L3 Misses = %llu\n",miss_L3);

   return 0;
}
