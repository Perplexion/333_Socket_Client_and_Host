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

int sendrec(int sock, char *sen ){
    char *rec;
    send(sock,sen,strlen(sen),0); //send command

    recv(sock,rec,strlen(rec),0); //receive response
    printf("%s\n",rec);             //print response


}

int ls(){


                DIR *d;
                struct dirent *dir;

                struct stat sb;
                char fileinfo[150];
                

                char time[30];
                d=opendir("./"); //open current directory
                if(d!=NULL){
                        dir=readdir(d); //read first entry
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

                        printf("%s \n",fileinfo); //print file info

                        
                        dir=readdir(d); //read the next entry
                    }
                



                }

}

int main(int argc, char *argv[]){


        int sockfd, numbytes, rv;
        struct addrinfo hints, *servinfo, *p;

        memset(&hints,0,sizeof(hints));
        hints.ai_family=AF_UNSPEC;
        hints.ai_socktype=SOCK_STREAM;
        rv=getaddrinfo(argv[1],argv[2],&hints,&servinfo); //address and port from user

        for(p=servinfo;p!=NULL;p=p->ai_next){
            if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){continue;}
            if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1){close(sockfd);continue;}
            break;


        }

        freeaddrinfo(servinfo);
        char s[100];        //sending string
        char arg1[30];
        char l[150];      //receiving string
        char *pl;    //delim string s by " "
       
        while(1){
            memset(s,'\0',sizeof(s)); //clears out s
            memset(l,'\0',sizeof(l)); //clear out l

            printf("enter a command:  \n"); //prompt user input
            scanf("%s",s);

            
            
            

            pl=strtok(s," ");  
            strcpy(arg1,pl);    //store 1st command within arg1
            if(strcmp("LOGIN",arg1)==0){
                sendrec(sockfd,s); //send, receive, print

                /*
                send(sockfd,s,strlen(s),0); //send command

                recv(sockfd,l,strlen(l),0); //receive response
                printf("%s\n",l);
                */
            }
            
            if(strcmp("LS",arg1)==0){
                send(sockfd,s,strlen(s),0); //send command
                recv(sockfd,l,strlen(l),0); //receive response

                while(strcmp("OK",l)!=0){ //keep receiving unti an "OK"
                    printf("%s\n",l);
                    memset(l,'\0',sizeof(l)); //clears out l
                    recv(sockfd,l,strlen(l),0); //receive response

                }
                 printf("%s\n",l);


            }

            if(strcmp("LOCALCD",arg1)==0){
                pl=strtok(s," "); //gets the directory name
                int change=chdir(pl);
                if(change==-1){
                    printf("localCD fail\n");
                }

            }

            if(strcmp("LOCALLS",arg1)==0){
               ls(); //run the ls function


            }



            if(strcmp("CD",arg1)==0){
                sendrec(sockfd,s);

                /*
                send(sockfd,s,strlen(s),0); //send command

                recv(sockfd,l,strlen(l),0); //receive response
                printf("%s\n",l);           //"OK" or "Failed"
                */

            }
            if(strcmp("MKDIR",arg1)==0){
                sendrec(sockfd,s);

                
                /*
                send(sockfd,s,strlen(s),0); //send command

                recv(sockfd,l,strlen(l),0); //receive response
                printf("%s\n",l);
                */

            }
           

        }


    



}