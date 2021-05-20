#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>

typedef struct __attribute__ ((packed)) {
    char name[18];
    int type;
    int offset;
    int size;
}sections;

void listeazaDirector(const char* path,int* n){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL) {
        perror("ERROR\nCould not open directory");
        return;
    }
    if(*n==0){
        printf("SUCCESS\n");
        *n=1;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                printf("%s\n", fullPath);
            }
        }
    }
    closedir(dir);
}

void listeazaRecursiv(const char* path, int *n){
    DIR *dir = NULL;
    dir=opendir(path);
    struct dirent *entry = NULL;
    char fullPath[5120];
    struct stat statbuf;

    
    if(dir == NULL) {
        perror("ERROR\nCould not open directory");
        return;
    }
    if(*n==0){
        printf("SUCCESS\n");
        *n=1;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                printf("%s\n", fullPath);
                *n=1;
                if(S_ISDIR(statbuf.st_mode)) {
                    listeazaRecursiv(fullPath,n);
                }
            }
        }
    }
    closedir(dir);
}

void listeazaDupaNume(const char*path, char* nume, int recursiv, int *n){
     DIR *dir = NULL;
    dir=opendir(path);
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
 
    if(dir == NULL) {
        perror("ERROR\nCould not open directory\n");
        return;
    }
    if(*n==0){
        printf("SUCCESS\n");
        *n=1;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                if(strstr(entry->d_name,nume)==entry->d_name){
                    printf("%s\n", fullPath);
                }
                if(S_ISDIR(statbuf.st_mode) && recursiv==1) {
                    listeazaDupaNume(fullPath,nume,recursiv,n);
                }
            }
        }
    }
    closedir(dir);
}

void listeazaDupaPermisiune(const char*path, int permision, int recursiv, int *n){
    DIR *dir = NULL;
    dir=opendir(path);
    struct dirent *entry = NULL;
    char fullPath[5120];
    struct stat statbuf;
 
    if(dir == NULL) {
        perror("ERROR\nCould not open directory");
        return;
    }
    if(*n==0){
        printf("SUCCESS\n");
        *n=1;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                if((statbuf.st_mode&0777)==permision){
                    printf("%s\n",fullPath);
                }
                if(S_ISDIR(statbuf.st_mode) && recursiv==1) {
                    listeazaDupaPermisiune(fullPath,permision,recursiv,n);
                }
            }
        }
    }
    closedir(dir);
}

void verificaFisier(char* path){
    int fd=open(path,O_RDONLY);
    if(fd==-1){
        printf("ERROR\ninput error\n");
        return;
    }
    off_t size=lseek(fd,0,SEEK_END);
    lseek(fd,size-1,SEEK_SET);
    char c=0;
    if(read(fd,&c,1)!=1){
        printf("ERROR\ncitire incorecta\n");
        return ;
    }
    if(c=='g'){
        unsigned headerSize=0;
        lseek(fd,size-3,SEEK_SET);
        if(read(fd,&headerSize,2)!=2){
            printf("ERROR\neroare la citire\n ");
            return;
        }
        lseek(fd,size-headerSize,SEEK_SET);
        int version=0;
        if(read(fd,&version,4)!=4){
            printf("ERROR\neroare la citire\n");
            return;
        }
        if(version<49 || version>113){
            printf("ERROR\nwrong version\n");
            return;
        }
        unsigned numberOf=0;
        if(read(fd,&numberOf,1)!=1){
            printf("ERROR\neroare la citire\n");
            return;
        }
        if(numberOf<8 || numberOf>11){
            printf("ERROR\nwrong sect_nr");
            return;
        }
        sections* sectiuniFisier=(sections*)malloc(numberOf*sizeof(sections));
        for(int i=0; i<numberOf; i++){
            if(read(fd,(sectiuniFisier+i),sizeof(sections))!=sizeof(sections)){
                printf("ERROR\neroare la citire\n");
                free(sectiuniFisier);
                return;
            }
            if(!((sectiuniFisier+i)->type==69||(sectiuniFisier+i)->type==30||(sectiuniFisier+i)->type==75||(sectiuniFisier+i)->type==23||(sectiuniFisier+i)->type==33||(sectiuniFisier+i)->type==98||(sectiuniFisier+i)->type==58)){
                printf("ERROR\nwrong sect_types\n");
                free(sectiuniFisier);
                return;
            }
        }
        printf("SUCCESS\nversion=%d\nnr_sections=%d\n",version,numberOf);
        for(int j=0; j<numberOf;j++){
            char name[19];
            memcpy(name,(sectiuniFisier+j)->name,18);
            name[18]=0;
            printf("section%d: %s %d %d\n",(j+1),name,(sectiuniFisier+j)->type,(sectiuniFisier+j)->size);
        }
        free(sectiuniFisier);
    }else{
        printf("ERROR\nwrong magic\n");
        return ;
    }

}

sections* verificaFisierPtLinie(char* path){
    int fd=open(path,O_RDONLY);
    if(fd==-1){
        return NULL;
    }
    off_t size=lseek(fd,0,SEEK_END);
    lseek(fd,size-1,SEEK_SET);
    char c=0;
    if(read(fd,&c,1)!=1){
        return NULL;
    }
    if(c=='g'){
        unsigned headerSize=0;
        lseek(fd,size-3,SEEK_SET);
        if(read(fd,&headerSize,2)!=2){
            return NULL;
        }
        lseek(fd,size-headerSize,SEEK_SET);
        int version=0;
        if(read(fd,&version,4)!=4){
            return NULL;
        }
        if(version<49 || version>113){
            return NULL;
        }
        unsigned numberOf=0;
        if(read(fd,&numberOf,1)!=1){
            return NULL;
        }
        if(numberOf<8 || numberOf>11){
            return NULL;
        }
        sections* sectiuniFisier=(sections*)calloc((numberOf+1),sizeof(sections));
        for(int i=0; i<numberOf; i++){
            if(read(fd,(sectiuniFisier+i),sizeof(sections))!=sizeof(sections)){
                free(sectiuniFisier);
                return NULL;
            }
            if(!((sectiuniFisier+i)->type==69||(sectiuniFisier+i)->type==30||(sectiuniFisier+i)->type==75||(sectiuniFisier+i)->type==23||(sectiuniFisier+i)->type==33||(sectiuniFisier+i)->type==98||(sectiuniFisier+i)->type==58)){
                free(sectiuniFisier);
                return NULL;
            }
        }
        (sectiuniFisier+numberOf)->size=-1;
        sections* a=sectiuniFisier;
        free(sectiuniFisier);
        return a;
    }else{
        return NULL;;
    }

}

void cautaLinie(char* path, int section, int line){
    sections* secFisier=verificaFisierPtLinie(path);
    int fd=open(path,O_RDONLY);
    if(secFisier==NULL || fd==-1){
        printf("ERROR\ninvalid file\n");
        return;
    }
    int d=0;
    while((secFisier+d)->size!=-1){
        d++;
    }
    if(section>d){
        printf("ERROR\ninvalid section\n");
        return;
    }
    sections sectiuneCautata=*(secFisier+section-1);
    lseek(fd,sectiuneCautata.offset+sectiuneCautata.size-1,SEEK_SET);
    char* string=(char*)calloc(sectiuneCautata.size,sizeof(char));
    char c=0;
    int j=0;
    for(int i=sectiuneCautata.offset+sectiuneCautata.size-1; i>=0; i--){
        lseek(fd,i,SEEK_SET);
        if(read(fd,&c,1)!=1){
            free(string);
            printf("ERROR\neroare la citire\n");
            return;
        }
        if((c&0xFF)==0x0A){
            i--;
            lseek(fd,i,SEEK_SET);
            if(read(fd,&c,1)!=1){
                free(string);
                printf("ERROR\neroare la citire\n");
                return;
            }
            if((c&0xFF)==0x0D){
                line--;
                if(line==0){
                    break;
                }
            }
            i--;
            lseek(fd,i,SEEK_SET);
            if(read(fd,&c,1)!=1){
                free(string);
                printf("ERROR\neroare la citire\n");
                return;
            }
        }
        if(line==1){
            *(string+j)=c;
            j++;
        }
    }
    if(line>1){
        free(string);
        printf("ERROR\ninvalid line\n");
        return;
    }
    printf("SUCCESS\n%s\n",string);
    free(string);

}

void afisareFile(const char* path, int* n){
    DIR *dir = NULL;
    dir=opendir(path);
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
    if(dir == NULL) {
        perror("ERROR\ninvalid directory path");
        return;
    }
    if(*n==0){
        printf("SUCCESS\n");
        *n=1;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                if(S_ISDIR(statbuf.st_mode)) {
                    afisareFile(fullPath,n);
                }
                if(S_ISREG(statbuf.st_mode)){
                    sections* secFisier=verificaFisierPtLinie(fullPath);
                    if(secFisier!=NULL){
                    int d=0;
                    int ok=0;
                    while((secFisier+d)->size!=-1){
                        if((secFisier+d)->size >1124){
                            ok=1;
                            break;
                        }
                        d++;
                    }
                    if(ok==0){
                        printf("%s\n", fullPath);
                    }
                    }
                }
            }
        }
    }
    closedir(dir);

}

int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("65781\n");
        }
    }

    if(argc<=5){
        int m=0;
        if(strcmp(argv[1],"list")==0){
            if(argc==3){
                if(strstr(argv[2],"path=")!=NULL){
                    int n=strlen(argv[2]);
                    char* path=(char*)calloc(n-4,sizeof(char));
                    strncpy(path,argv[2]+5,n-5);
                    listeazaDirector(path,&m);
                    free(path);
                }else{
                    printf("ERROR\n invalid input\n");
                    return 2;
                }
            }
            if(argc==4){
                if(strstr(argv[2],"path=")!=NULL){
                    char* path=argv[2]+5;
                    if(strstr(argv[3],"recursive")!=NULL){
                         listeazaRecursiv(path,&m);
                    }else{
                        if(strstr(argv[3],"name_starts_with=")!=NULL){
                            char* name=argv[3]+17;
                            listeazaDupaNume(path,name,0,&m);
                        }else{
                            if(strstr(argv[3],"permissions=")!=NULL)  {
                                char* name=argv[3]+12;
                                double permision=0;
                                for(int i=0; i<9; i++){
                                    double f=1.0;
                                   if(*(name+i)=='-' || *(name+i)=='x' || *(name+i)=='r' || *(name+i)=='w'){
                                       if(*(name+i)!='-'){
                                           for(int j=0; j<8-i; j++){
                                               f*=2;
                                           }
                                           permision+=f;
                                       }

                                   }else{
                                       printf("ERROR\nformat gresit\n");
                                       return 1;
                                   } 
                                }
                                listeazaDupaPermisiune(path,permision,0,&m);
                            }
                            else
                            {
                                printf("ERROR\n invalid input");
                                return 3;
                            }
                            
                        }
                    }
                }else{
                    char* path=argv[3]+5;
                    if(strstr(argv[2],"recursive")!=NULL){
                        listeazaRecursiv(path,&m);
                    }else{
                        if(strstr(argv[2],"name_starts_with=")!=NULL){
                            char* name=argv[2]+17;
                            listeazaDupaNume(path,name,0,&m);
                        }else{
                            if(strstr(argv[2],"permissions=")!=NULL)  {
                                char* name=argv[2]+12;
                                int permision=0;
                                for(int i=0; i<9; i++){
                                    double f=1.0;
                                   if(*(name+i)=='-' || *(name+i)=='x' || *(name+i)=='r' || *(name+i)=='w'){
                                       if(*(name+i)!='-'){
                                           for(int j=0; j<8-i; j++){
                                               f*=2;
                                           }
                                           permision+=f;
                                       }

                                   }else{
                                       printf("ERROR\nformat gresit/n");
                                       return 1;
                                   } 
                                }
                                listeazaDupaPermisiune(path,permision,0,&m);
                            }
                            else
                            {
                                printf("ERROR\n invalid input");
                                return 2;
                            }
                        }
                    }                   
                }
            }
        }
    }

    if(argc<=3){
        if(strcmp(argv[1],"parse")==0){
            if(strstr(argv[2],"path=")!=NULL){
                char* path=argv[2]+5;
                verificaFisier(path);
            }
        }
    }

    if(argc<=5){
        if(strcmp(argv[1],"extract")==0){
            if(strstr(argv[2],"path=")!=NULL){
                char* path=argv[2]+5;
                if(strstr(argv[3],"section=")!=NULL){
                    char* section=argv[3]+8;
                    int sectionNumber=atoi(section);
                    if(strstr(argv[4],"line=")!=NULL){
                        char* line=argv[4]+5;
                        int lineNumber=atoi(line);
                        cautaLinie(path,sectionNumber,lineNumber);
                    }else{
                        printf("ERROR\ninvalid input\n");
                        return 1;
                    }
                }else{
                    printf("ERROR\ninvalid input\n");
                    return 1;
                }
            }else{
                printf("ERROR\ninvalid input\n");
                return 1;
            }
        }
    }

    if(argc<=3){
        if(strcmp(argv[1],"findall")==0){
            if(strstr(argv[2],"path=")!=NULL){
                char* path=argv[2]+5;
                int n=0;
                afisareFile(path,&n);
            }
        }
    }
    
    return 0;
}