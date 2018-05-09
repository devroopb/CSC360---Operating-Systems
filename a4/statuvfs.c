/*
  Devroop Banerjee
  V00837868
  CSC360 A4
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"

int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    int  i;
    char *imagename = NULL;
    FILE  *f;
    int   *fat_data = malloc(sizeof(int));

    for (i = 1; i < argc; i++){
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL){
        fprintf(stderr, "usage: statuvfs --image <imagename>\n");
        exit(1);
    }

    //open disk image
    if((f = fopen(imagename, "r")) == NULL){
        fprintf(stderr, "unable to open %s\n", imagename);
        exit(1);
    }

    //read contents into sb
    fread(&sb, sizeof(superblock_entry_t), 1, f);

    //remove the "IMAGES/" part from imagename
    imagename = strtok(imagename, "/");
    imagename = strtok(NULL, "");

    //calculate free, resv, alloc
    int offset = htons(sb.block_size);
    fseek(f, offset, SEEK_SET);
    int count = htonl(sb.fat_start) * htons(sb.block_size) * 4;

    int allocated  = 0;
    int free_space = 0;
    int reserved = 0;

    while(count > 0){
        fread(fat_data, 4, 1, f);
        if(htonl(*fat_data) == FAT_AVAILABLE){
            free_space++;
        }else if(htonl(*fat_data) == FAT_RESERVED){
            reserved++;
        }else if(htonl(*fat_data) == FAT_LASTBLOCK){
            allocated++;
        }else{
            allocated++;
        }
        count--;
    }
    free_space = htonl(sb.num_blocks) - reserved - allocated;

    //prints everything
    printf("%.8s (%s)\n",sb.magic, imagename);
    printf("\n-------------------------------------------------------\n");
    printf("  Bsz\tBcnt\tFATst\tFATcnt\tDIRst\tDIRcnt\n  %hu\t%hu\t%hu\t%hu\t%hu\t%hu\n",
        htons(sb.block_size),
        htonl(sb.num_blocks),
        htonl(sb.fat_start),
        htonl(sb.fat_blocks),
        htonl(sb.dir_start),
        htonl(sb.dir_blocks)
    );
    printf("\n-------------------------------------------------------\n");
    printf("  Free\tResv\tAlloc\n  %d\t%d\t%d\n",
        free_space,
        reserved,
        allocated
        );
    return 0;
}
