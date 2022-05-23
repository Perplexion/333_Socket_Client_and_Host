#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>


int logged=0; //login flag
char currentDir[50]; //keep track of the current directory; used in ls command
int createF(){

    FILE *upf;
    char usern[]="avuu:1234:/source/icsi333/avuu";
    upf=fopen("upf.txt","w");
    int wr=fputs(usern,upf);
    if(wr==EOF){
        printf("not written\n");
    }

}
int cd(char *cd[], int fide){
    char ok[]="OK";
    char fail[]="CD fail";
    if(strcmp("CD",cd[0])==0){
        int change=chdir(cd[1]);
        if(change==-1){             //send fail message on failure
            send(fide,fail,strlen(fail),0);

        }
        else{
            strcpy(currentDir,cd[1]); //change currentDir
            send(fide,ok,strlen(ok),0); //send OK
        }
    }


}
int mk_dir(char *mk[], int fide){
    char ok[]="OK";
    if(strcmp("MKDIR",mk[0])==0){
        mkdir(mk[1],S_IRWXU); //makes directory and gives user read,write,execution permissions
        send(fide,ok,strlen(ok),0);
    }

}

int ls(char *lcom[],int fide){ //read through the current directory with readdir
    if(strcmp("LS",lcom[0])==0){
        DIR *d;
        struct dirent *dir;

        struct stat sb;
        char fileinfo[150];
        char ok[]="OK";

        char time[30];
        d=opendir(currentDir);
        if(d!=NULL){
            dir=readdir(d);
            while(dir!=NULL){ 
                
                memset(fileinfo,'\0',sizeof(fileinfo)); //clear out the string

                stat(dir->d_name,&sb);
                strcpy(fileinfo,dir->d_name); //add in filename

                strcat(fileinfo," ");   //space in between info

                int siz=(int)sb.st_size;
                char sizz[20];              
                sprintf(sizz,"%d",siz);  //int to string

                strcat(fileinfo,sizz);  //add in file size

                strcat(fileinfo," ");

                
                
                strcpy(time,ctime(&sb.st_mtime));
                
                strcat(fileinfo,time);      //add in last mod date

                send(fide,fileinfo,strlen(fileinfo),0); //send filename, size, and date

                
                dir=readdir(d); //read the next entry
            }
            send(fide,ok,strlen(ok),0); //"OK"



        }


    }

}

int login(char *plog[]){
    if(strcmp("LOGIN",plog[0])==0){
        char s[100];
        char fline[3][50];
        char *p;
        FILE *fp;
        fp=fopen("upf.txt","r");
        while (fscanf(fp,"%s",s)!=EOF) //read the file line
        {
            int c=0;
            while(p!=NULL){   //takes the s string and splits it into strings 
                    
                p=strtok(s,":");  
                strcpy(fline[c],p);    //store within string array; strcpy overwrites whatever is stored
                c++;
            }

            if(strcmp(plog[1],fline[0])==0){ //username compare
                if(strcmp(plog[2],fline[1])==0){   //password check
                    chdir(fline[2]); //change the directory to "filepath"

                    

                    strcpy(currentDir,fline[2]); //set global current directory to filepath
                    logged=1;       //user now logged in
                    return 0;
                }
                
                    
                
                
            }


        }
        return -1; //EOF returns -1
        


    }
    else{
        return 1;
    }
    

}


int main(int argc, char *argv[]){

    int sockfd, new_fd, yes=1, rv;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    char string[100];
    createF();  //create user file with password and path
    
        chdir(argv[1]); //change the working directory
        printf("%s\n", getcwd(string, 100)); //prints absolute path name

        
        memset(&hints,0,sizeof(hints));
        hints.ai_family=AF_UNSPEC;
        hints.ai_socktype=SOCK_STREAM;
        hints.ai_flags=AI_PASSIVE; //my ip
        rv=getaddrinfo(NULL,argv[2],&hints,&servinfo); //use port 1033 


        for(p=servinfo;p!=NULL;p=p->ai_next){
            if(p->ai_addr->sa_family==AF_INET){
                struct sockaddr_in *in=(struct sockaddr_in *)p->ai_addr;
                inet_ntop(AF_INET, &(in->sin_addr),string,1000);
            }
            else{
                struct sockaddr_in6 *in=(struct sockaddr_in6 *)p->ai_addr;
                inet_ntop(AF_INET6, &(in->sin6_addr),string,1000);
            }
           // printf("%s\n",string);      //address would be 0.0.0.0

            if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){continue;}
            if(bind(sockfd, p->ai_addr,p->ai_addrlen)==-1){
                close(sockfd); continue;
            }
            break;


        }
        
        int n;
        char ok[]="ok";
        char badlog[]="Wrong username or password";
        char notlogged[]="not logged in";
        freeaddrinfo(servinfo);
        if(listen(sockfd, 3)==-1){exit(1);}
        while(1){
            sin_size=sizeof(their_addr);
            new_fd=accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
           

            while(1){
                char l[100];
                char *pch;
                char *args[4]; //array of string commands



                n=recv(new_fd,l,strlen(l),0);   //message in "l"
                l[n]='\0';
                printf("%s \n",l); 

                
                int c=0;
                while(pch!=NULL){   //takes the l string and splits it into strings separated by " "
                    
                    pch=strtok(l," ");  
                    strcpy(args[c],pch);    //store within string array args
                    c++;
                }

                if(logged==0){ //if the user is not logged in
                  int lo=login(args);
                  
                  if(lo==-1){
                      send(new_fd,badlog,strlen(badlog),0); //send incorrect credentials message
                  }
                  if(lo==1){
                      send(new_fd,notlogged,strlen(notlogged),0); //if the first arg is not login


                  }
                }
                else if(logged==1){           //prevents the user from using any other command if not logged in
                   
                    login(args); //check for another login command


                    
                    ls(args,new_fd);    //checks for ls command; takes the args and the new fd
                    mk_dir(args,new_fd); //checks for MKDIR
                    cd(args,new_fd);    //checks for CD


                    if(strcmp("PUSH",args[0])==0){  //if the first argument is PUSH
                                char fn[100];                   //filename
                                if(strchr(args[1],'"')!=NULL){

                                    pch=strtok(args[1],"\"");
                                    strcpy(fn,pch);
                                    
                                }
                                if(strcmp("fileSize",args[2])==0){

                                    pch=strtok(args[3],"<>");   //delims "<,>"
                                    int fsize=atoi(pch);        //gets filesize
                                    FILE *fp;
                                    int reci;
                                    char buff[1024];
                                    fp=fopen(fn,"w");
                                    fseek(fp,fsize,SEEK_SET);   //set file size
                                    
                                    while(1){
                                        reci=recv(new_fd,buff,1024,0);
                                        if(reci<=0){
                                            break;
                                        }
                                        fprintf(fp,"%s",buff);
                                        memset(buff,'\0',sizeof(buff)); //reset the buffer to null

                                    }
                                send(new_fd,ok,strlen(ok),0); 

                                }
                                


                                


                            }

                            if(strcmp("PULL",args[0])==0){
                                if(strchr(args[1],'"')!=NULL){
                                    char fn[100];
                                    pch=strtok(args[1],"\"");
                                    strcpy(fn,pch);




                                    FILE *fp;
                                    fp=fopen(fn,"r");
                                    int sen;

                                    struct stat st;
                                    stat(fn,&st);
                                    
                                    int fisize=(int)st.st_size;
                                
                                    char si[20];
                                    sprintf(si,"%d",fisize);       //  itoa(fisize,si,10);


                                    char mes[]="Size: ";
                                    strcat(mes,si);
                                    
                                    send(new_fd,mes,strlen(mes),0); //"Size: #"
                                    
                                    char data[1024]={0};
                                    while(fgets(data,1024,fp)!=NULL){
                                        sen=send(new_fd,data,sizeof(data),0);
                                        if(sen==-1){
                                            printf("file send error \n");
                                            exit(1);
                                        }
                                        memset(data,'\0',sizeof(data));
                                    }
                                    send(new_fd,ok,strlen(ok),0); 

                                }


                            }


                }

               


            }
            
            
        }


}

