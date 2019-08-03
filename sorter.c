#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <libexif/exif-data.h>

typedef struct 
{
    int id;
    char filename[255];
    time_t birthtime;
    
} fileentry;

typedef struct id_time_t

{   
    int id;
    time_t interval;
    /* data */
} id_time_t;


time_t my_strptime(char timestring[30], struct tm *time)
{
    time_t utime;
    /* assumes YYYY-MM-DD HH:MM:SS format from EXIF */
char cyear[4], cmon[3], cday[3], chour[3], cmin[3], csec[3];
strncpy(cyear, &timestring[0], 4);
cyear[4]='\n';
strncpy(cmon, &timestring[5], 2);
cmon[2]='\n';
strncpy(cday, &timestring[8], 2);
cday[2]='\n';
strncpy(chour, &timestring[11], 2);
chour[2]='\n';
strncpy(cmin, &timestring[14], 2);
cmin[2]='\n';
strncpy(csec, &timestring[17], 2);
csec[2]='\n';

time->tm_year = (int) strtol(cyear, NULL, 10)-1900;
time->tm_mon= (int) strtol(cmon, NULL, 10) - 1;
time->tm_mday = (int) strtol(cday, NULL, 10);
time->tm_hour = (int) strtol(chour, NULL, 10);
time->tm_min = (int) strtol(cmin, NULL, 10);
time->tm_sec = (int) strtol(csec, NULL, 10);
time->tm_isdst = 1;

//printf("%04d=%02d=%02d %02d'%02d'%02d", time->tm_year, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

utime = mktime(time);
return utime;
}

time_t exif_timestamp(ExifData *d)
{
char buf[1024];
struct tm exiftime;
time_t utime;

    /* See if this tag exists */
    ExifEntry *entry = exif_content_get_entry(d->ifd[EXIF_IFD_EXIF],EXIF_TAG_DATE_TIME_ORIGINAL);
    if (entry) {
        
        /* Get the contents of the tag in human-readable form */
        exif_entry_get_value(entry, buf, sizeof(buf));

        /* Don't bother printing it if it's entirely blank */
        //trim_spaces(buf);
        if (*buf) {
    //       printf("%s\n",  buf);
           
           utime = my_strptime(buf, &exiftime);

              
               // printf("%04d=%02d=%02d %02d'%02d'%02d\n", exiftime.tm_year, exiftime.tm_mon+1, exiftime.tm_mday, exiftime.tm_hour, exiftime.tm_min, exiftime.tm_sec);
                 // utime = mktime(&exiftime);
             //   printf("%d", utime);
                return utime;           
        }
    else return 0;
    }
}

static void show_tag(ExifData *d, ExifIfd ifd, ExifTag tag)
{
    /* See if this tag exists */
    ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);
    if (entry) {
        char buf[1024];

        /* Get the contents of the tag in human-readable form */
        exif_entry_get_value(entry, buf, sizeof(buf));

        /* Don't bother printing it if it's entirely blank */
        //trim_spaces(buf);
        if (*buf) {
           printf("%s\n",  buf);
            // exif_tag_get_name_in_ifd(tag,ifd)
        }
    }
}

fileentry* filelist_prepare(char *path)
{
struct stat filedata;
DIR *fold;
struct dirent *dir;
ExifData *exif1;
struct tm *ftime; 
time_t origtime;
char name[255], fullname[255];
fileentry* filelist = (fileentry*)calloc(1000,sizeof(fileentry));
int i=0;
fold = opendir(path);
if (fold) 
    {
        while ((dir = readdir(fold)) != NULL)
            {   
                
                // printf("%s", dir->d_name);
                strcpy(name, dir->d_name);
                strcpy(fullname, path);
                if (fullname[strlen(fullname)-1] != "/")
                        {
                            strcat(fullname,"/");
                        }
                strcat(fullname,name);
             //   printf("%s\n",fullname);
                exif1 = exif_data_new_from_file(fullname);
                if (exif1)
                    {
                        origtime = exif_timestamp(exif1);
                        // show_tag(exif1, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL);
                    }
                else
                {
                   origtime = 0;
                   // printf("No EXIF or file not found ");
                }
                
              //  printf("%s ", name);
                stat(name,&filedata);
                ftime = localtime(&origtime);
             //  printf(" %02d:%02d\n", ftime->tm_hour,ftime->tm_min);
                // printf("7890");
                filelist[i].id = i;
               strcpy(filelist[i].filename, name);
            //    filelist[i].birthtime = filedata.st_birthtime;
                filelist[i].birthtime = origtime;
           //     printf(" %d %s\n",i,filelist[i].filename);
               
                              i++; 
            }
        closedir(fold);
    }

return filelist;
}

void datelist_prepare(fileentry *filelist, struct tm *days[180])
{

}

void catalog_create(char *path, char *datelist)
{

}

void copy_files(char *path, char *filelist, char *datelist)
{

}

int compare_filelist (const void * a, const void * b)
{

  fileentry *fil1 = (fileentry *)a;
  fileentry *fil2 = (fileentry *)b;

  return ( fil1->birthtime - fil2->birthtime );
}

int compare_intervals (const void * a, const void * b)
{
    id_time_t *time1 = (id_time_t *)a;
    id_time_t *time2 = (id_time_t *)b;

    return(time2->interval - time1->interval);
}


int main()
{

fileentry *filelist1;
id_time_t *intervals = (id_time_t*)calloc(1000,sizeof(id_time_t));
struct tm *days[180];
int k, filecount;
struct tm *ltime;

filelist1 = filelist_prepare("D:/Archive/Photo/Korea"); // prepare list of files with dates

if (filelist1 == NULL) return -1;

k=0;
while (filelist1[k].id == k) k++;
filecount = k-1;
printf("Number of the files found: %d\n", filecount+1);

qsort(filelist1,filecount,sizeof(fileentry), compare_filelist);

for (k=1;k<filecount;k++)
     {
         intervals[k].id = k;
         intervals[k].interval = filelist1[k].birthtime - filelist1[k-1].birthtime;
     }

qsort(intervals,filecount,sizeof(id_time_t), compare_intervals);

 for (k=0;k<filecount+1;k++)
     {
        printf("%d %d %s ",k,filelist1[k].id,filelist1[k].filename);
        ltime = localtime(&filelist1[k].birthtime);
        printf("%02d:%02d %02d-%02d-%04d\n",ltime->tm_hour, ltime->tm_min, ltime->tm_mday, 1+ltime->tm_mon, 1900 + ltime->tm_year);
     }

for (k=1;k<filecount;k++)
     {
         ltime = gmtime(&intervals[k].interval);
        printf("%02d:%02d:%02d %02d\n",ltime->tm_hour, ltime->tm_min,  ltime->tm_sec, ltime->tm_mday-1);
     }

datelist_prepare(filelist1, days); // prepare list of dates

catalog_create(".","dates.txt"); // prepare catalog and subcatalogs for files to copy

copy_files(".","mylist.txt", "dates.txt"); // copying files by date

free(filelist1);
free(intervals);

//my_strptime("2018-05-12 16:33:23", ltime);
//printf("New %04d=%02d=%02d %02d'%02d'%02d\n", ltime->tm_year+1900, ltime->tm_mon+1, ltime->tm_mday, ltime->tm_hour, ltime->tm_min, ltime->tm_sec);
}