/*
Devroop Banerjee
V00837868
CSC360 - A4
Reference:- https://stackoverflow.com/questions/14062910/clearest-way-to-read-and-print-txt-file-lines-in-c
*/

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    directory_entry_t db;
    int  i, *ii;
    char *imagename = NULL;
    char *filename  = NULL;
    FILE *f;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--file") == 0 && i+1 < argc) {
            filename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL || filename == NULL) {
        fprintf(stderr, "usage: catuvfs --image <imagename> " \
            "--file <filename in image>");
        exit(1);
    }

    //open disk image
    if((f = fopen(imagename, "r")) == NULL){
        fprintf(stderr, "unable to open %s\n", imagename);
        exit(1);
    }

    //read contents into sb
    fread(&sb, sizeof(superblock_entry_t), 1, f);

    //go to where the root directory starts
    int offset = htons(sb.block_size) + (htonl(sb.fat_blocks) * htons(sb.block_size));
    fseek(f, offset, SEEK_SET);
    int count = (htons(sb.block_size) * htonl(sb.dir_blocks))/64;

    //read from root directory
    while(count > 0){
        fread(&db, sizeof(directory_entry_t), 1, f);
        //file not found
        if((count == 1) && (strcmp(db.filename, filename) != 0)){
            printf("File not found!\n");
            exit(1);

        //file found so copy to stdout
        }else if(strcmp(db.filename, filename) == 0){
            printf("File found!\n");
            /*offset = (db.start_block * sb.block_size);
            fseek(f, offset, SEEK_SET);
            //printf("%d\n", htonl(offset));
            fread(ii, sizeof(short), 1, offset);
            fwrite(stdout, sizeof(short), 1, ii);
            exit(1);*/
        }
        count--;
    }
    return 0;
}
