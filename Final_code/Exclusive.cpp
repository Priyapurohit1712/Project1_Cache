#include<stdio.h>
#include<math.h>
#include<assert.h>
#include<stdbool.h>
#define L2_asso 8
#define L3_asso 16

unsigned long long miss_L2,miss_L3;
unsigned long long HIT_L2,HIT_L3;


struct L2_cache_entry
{
  unsigned long long tag=-1;
  int set;               
  int counter=-1;
  bool valid=0;
  
}L2_Cache[8192];

struct L3_cache_entry
{
  unsigned long long tag=-1;
  int set;
  int counter=-1;
  bool valid=0;
  
}L3_Cache[32768];

void Maintain_counter(int set,int line,int previous_count,int level_asso);
int find_line(int set,unsigned long long tag,int level_asso);
void Invalidate_L3(int set,int line,int prev_counter);


unsigned long long recalculate_addr(unsigned long long tag,long set)
{

unsigned long long addr;
tag=tag<<10;
addr=tag|set;
return(addr);

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

void Insert_L3(unsigned long long L3_evict_addr)
{
   long mask=pow(2,11)-1;
   long set= L3_evict_addr&mask;
   unsigned long long tag=L3_evict_addr>>11;
   int line_num=find_line(set,tag,L3_asso);
   int prev_count=L3_Cache[line_num].counter;
   L3_Cache[line_num].tag=tag;
   L3_Cache[line_num].set=set;
   L3_Cache[line_num].valid=1;
   L3_Cache[line_num].counter=15;
   Maintain_counter(set,line_num,prev_count,L3_asso);


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
int find_line(int set,unsigned long long tag,int level_asso)  
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
              		{ //printf("hit in L2");
                	  prev_counter=L2_Cache[i].counter;   
	       	          L2_Cache[i].counter=7;
			  L2_Cache[i].valid=1;
                          Maintain_counter(set,i,prev_counter,L2_asso);
                          found_flag=true;
			  HIT_L2++;
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
             if(L3_Cache[i].tag==tag && L3_Cache[i].valid==1)   //hit in l3 , bring to l2 and invalidate in l3
              { 
                //printf("hit in L3"); 
                prev_counter=L3_Cache[i].counter;
	        L3_Cache[i].counter=-1;
		L3_Cache[i].valid=0;                     
                found_flag=true;  
		HIT_L3++;
                Invalidate_L3(set,i,prev_counter);
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



void Invalidate_L3(int set,int line,int prev_counter)
{
  
  int line_start=set*L3_asso;
  int line_end=((set+1)*L3_asso)-1;

      while(line_start<=line_end)
      {  
         
        if((L3_Cache[line_start].counter<prev_counter) && (line_start!=line) && (L3_Cache[line_start].counter!=-1) )
          L3_Cache[line_start].counter++;

	  line_start++; 
      }
  
}


int main (int argc, char *argv[])
{

 int numtraces = atoi(argv[2]);
 char input_name[50];
 FILE *fp;
 for (int k=0; k<numtraces; k++) 
 {
      sprintf(input_name, "%s_%d", argv[1], k);
      fp = fopen(input_name, "rb"); 
      char iord,type;
      unsigned long long addr;
      unsigned int pc;
      assert(fp!= NULL);
      while  (!feof(fp)) {
         fread(&iord, sizeof(char), 1, fp);
         fread(&type, sizeof(char), 1, fp);
         fread(&addr, sizeof(unsigned long long), 1, fp);
         fread(&pc, sizeof(unsigned), 1, fp);

          
  	  if(type!=0)
          {
               long l2_set=Findset_L2(addr);
               unsigned long long l2_tag=Findtag_L2(addr);
               bool is_present_L2=check_presence(l2_set,l2_tag,L2_asso);
    		if(is_present_L2==false)
                { 

 			long l3_set=Findset_L3(addr);
                 	unsigned long long l3_tag=Findtag_L3(addr); 
                        int l2_victim=find_line(l2_set,l2_tag,L2_asso);  

 
			if(L2_Cache[l2_victim].counter==-1)    //Empty lines case l2
                        {
                                bool is_present_L3=check_presence(l3_set,l3_tag,L3_asso);    //L3 present case handled by invalidating in L3
                                Updateline_L2(l2_tag,l2_set,l2_victim);                            //L3 present case --> bring to L2 now
                                                                                           //L3 absent case--> just bring to L2 
                        }

                        else if(L2_Cache[l2_victim].counter==0)            //L2 full case--> the one which is evicted from L2 needs to be 
                                                                           //invalidated in L3
         		{

                           unsigned long long L3_evict_addr=recalculate_addr(L2_Cache[l2_victim].tag,L2_Cache[l2_victim].set);
                           bool is_present_L3=check_presence(l3_set,l3_tag,L3_asso); // L3 present case handled by invalidating
				
					   if(is_present_L3==true)
					   {  
						Updateline_L2(l2_tag,l2_set,l2_victim);    //bring to L2 insert evicted from L2 in L3
                                                Insert_L3(L3_evict_addr);
                                               
					    }
					    else if(is_present_L3==false) //bring to L2 and insert evicted in l3
					   {
                                                Updateline_L2(l2_tag,l2_set,l2_victim);
                                                Insert_L3(L3_evict_addr);
                                               
					     }
                        }
		}
			 } 
		   } fclose(fp);  //while
	
  }  //for


        printf("L2 Misses = %llu \n",miss_L2);
	printf("L3 Misses = %llu \n",miss_L3);
	printf("L2 HITS = %llu \n",HIT_L2);
	printf("L3 HITS = %llu \n",HIT_L3);
	return 0;
}//main

