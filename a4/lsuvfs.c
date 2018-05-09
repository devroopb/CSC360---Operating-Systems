/*
  Devroop Banerjee
  V00837868
  CSC360 A4
*/

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"


char *month_to_string(short m) {
    switch(m) {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "?!?";
    }
}


void unpack_datetime(unsigned char *time, short *year, short *month,
    short *day, short *hour, short *minute, short *second)
{
    assert(time != NULL);

    memcpy(year, time, 2);
    *year = htons(*year);

    *month = (unsigned short)(time[2]);
    *day = (unsigned short)(time[3]);
    *hour = (unsigned short)(time[4]);
    *minute = (unsigned short)(time[5]);
    *second = (unsigned short)(time[6]);
}


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    directory_entry_t db;
    int  i;
    char *imagename = NULL;
    FILE *f;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL)
    {
        fprintf(stderr, "usage: lsuvfs --image <imagename>\n");
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
    short year, month, day, hour, minute, second;

    //read and print from root directory
    while(count > 0){
        fread(&db, sizeof(directory_entry_t), 1, f);

        //for files and directories
        if((db.status == 0x1) || (db.status == 0x2)){
            //use the function given to retrieve the time when data is unpacked
            unpack_datetime(db.create_time,
                            &year,
                            &month,
                            &day,
                            &hour,
                            &minute,
                            &second
                            );

            printf("%8d  %hi-%s-%hi  %hi:%hi:%hi  %s",
                    htonl(db.file_size),
                    year,
                    month_to_string(month),
                    day,
                    hour,
                    minute,
                    second,
                    db.filename
                    );

            if(db.status == 0x2){
                printf("\t(directory)");
            }

            printf("\n");
        }else{
            //for available do nothing
        }
        count--;
    }

    return 0;
}
