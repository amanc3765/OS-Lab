#include "alloc.h"


extern block *freeList, *allocList;
extern char* page;


int main(){


    if(init() == 0){
        printf("\n[init] Page allottment by mmap() succeeded.\n");
    }else{
        printf("\n[init] Page allottment by mmap() failed.\n");
    }

    print_List(freeList,FREE);
    print_List(allocList,ALLOC);
    

    int choice, allocSize, blockIndex;
    char *allocAddress; 
    block* temp;

    while(TRUE){
        
        printf("\n__________Memory Manager___________\n"
           "\n[1] Alloc                      \n"
           "\n[2] Dealloc                    \n");

        printf("\n[MM] Enter your choice: ");
        scanf("%d", &choice);

        switch(choice){
        
            case 1: 
                
                while(TRUE){
                    printf("\n[Alloc] Enter block size in multiples of 8 B : ");                
                    scanf("%d",&allocSize);

                    if(allocSize % 8){
                        printf("\n[Alloc] Block size is not a multiple of 8.\n");
                    }else{
                        break;
                    }
                }

                allocAddress = alloc(allocSize);
                
                if(allocAddress == NULL){
                    printf("\n[Alloc(%d)]: Request denied (Not enough space).\n",allocSize);
                }else{
                    printf("\n[Alloc(%d)]: Request granted at address (%u | %p).\n",allocSize,allocAddress,allocAddress); 
                }

            break;
            
            case 2:

                printf("\n[Dealloc] Enter memory block index: ");
                scanf("%d",&blockIndex);

                temp = allocList;
                while(temp && blockIndex--){
                    temp = temp->next;
                }

                dealloc(temp);

            break;

            default:

                if(cleanup() == 0){
                    printf("\n[cleanup] Unmapping of pages successful.\n\n");
                }else{
                    printf("\n[cleanup] Unmapping of pages failed.\n\n");
                }

                return 0;
        }

        print_List(freeList,FREE);
        print_List(allocList,ALLOC);

        drawMap(freeList);
        drawMap(allocList);

        printf("\n\n\n\n\n\n");
    }

    return 0;
}


void print_List(block* list, int listType){
	
    if(listType == FREE){
        printf("\n_____________Free List_____________\n");
    }else{
        printf("\n__________Allocated List___________\n");
    }
    
    block* temp = list;

	while(temp){
		printf("\n [ (%u | %p) [ %d ] ]--> \n",temp->start,temp->start,temp->size);
		temp = temp->next;
	}

	printf("\nNULL\n");
}


void drawMap(block *list){

    printf("\n\n\n");

    if(list == NULL){

        if(list == freeList){
            draw('|',PAGESIZE);
        }else{
            draw('_',PAGESIZE);
        } 

    }else{

        block* temp = list;

        if(list == freeList){
            draw('|',temp->start - page);
        }else{
            draw('_',temp->start - page);
        }        

        char blockCode = 'a';

        while(temp){

            if(list == freeList){
                draw('_',temp->size);
            }else{
                draw(blockCode++,temp->size);
            } 

            if(temp->next == NULL){
                break;
            }else{
                if(list == freeList){
                    draw('|',temp->next->start - (temp->start + temp->size));
                }else{
                    draw('_',temp->next->start - (temp->start + temp->size));
                } 
                temp = temp->next;
            }           
        }

        if(list == freeList){
            draw('|',(page + PAGESIZE) - (temp->start + temp->size));
        }else{
            draw('_',(page + PAGESIZE) - (temp->start + temp->size));
        } 
    }    
}


void draw(char blockCode, int size){
    
    if(size == 0){
        return;
    }else{
        int space = (size*150)/PAGESIZE;

        while(space--){
            printf("%c",blockCode);
        }
    }    
}