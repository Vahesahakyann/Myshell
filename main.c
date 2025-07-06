#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<string.h>
#include<signal.h>
//this is queue written in builtin file
//pwd-0
//cd-1
//exit-2 
//set -3
//unset -4
//echo -5
//history-6 
//help-7
#define COUNT_BUILTIN 8
#define LENGHT_BUILTIN 20
#define COMMAND_SIZE 500
#define PARAMETER_COUNT 10
#define PARAMETER_LENGHT 20
#define SET_MAX_COUNT 50
#define SET_MAX_LENGHT 30
#define MAX_HISTORY_COUNT 201
#define MAX_HISTORY_LENGHT 50
char** create2dim(int rows, int columns);//creating 2dim array
void del2dim(char** arr, int rows);//deleting 2dim array
char** tokenizer(char* arr,int *tokencount, char** token2dimarr);//in this program this function is used to do tokenizing command line
void isbuiltin( char **buildinlist, char **firsttoken,int *isbuiltin);//this function is for checking whether command is builtin or not
void myhelp(char** arr);//for help command
int myset(char **setkeybuf,char **setvaluebuf,char **tokens,int *setkeyvalue,int *setvaluevalue,int tokenscount);//setting set variable
int myunset(char **setkeybuf,char **setvaluebuf,char **tokens,int *setkeyvalue,int *setvaluevalue,int tokenscount);//to unset set variable
void mycd(char** commandline,char* currentdirectory,char *temporarydirectorypath,int tokencount);//immitating cd command
void myecho(char** tokens,int tokencounts,char** setkeybuf,char**setvaluebuf,int keycount);//for echo
void myhistory(char **historybuffer,int historycount, int nexthistory);//for demonstrating history
int saveinhistory(char **historybuffer,int historycount, int nexthistory,FILE *history);//to save command in history
void mypwd(int tokencount,char* currentdirectory);//pwd
void myexit(int *state);//exit


int main()
{
  int state=1;//condition number for while loop
  //opening buildin file and read to 2dimentional array 
  char **arrbuildin=create2dim(COUNT_BUILTIN,LENGHT_BUILTIN);//buffer to copy names of builtin.txt
  FILE *filebuildin=fopen("/home/cyberfox/Desktop/project/builtin.txt","r");//opening file in which contain the builin command names
   
  if(filebuildin==NULL)
   {
    perror("file doesn't open:filebuildin");
    exit(EXIT_FAILURE);
   }
   char *isbuiltinread=malloc(200);//temporary placeholder for builtin
   if(isbuiltinread==NULL)
   {
    perror("");
    exit(EXIT_FAILURE);
   }
   else
   {
     int sizecount=0;// this variable is for while loop
     fread(isbuiltinread,199,1,filebuildin);//reading form file where builtins are hold
     char *builtinplace;
     builtinplace=strtok(isbuiltinread," ");
      while(builtinplace!=NULL)//tokenizing the builtin commands
     {
      strcpy(arrbuildin[sizecount],builtinplace);
      builtinplace=strtok(NULL," ");
      sizecount++;
     }
    }
    
   //history
   FILE *history=fopen("/home/cyberfox/Desktop/project/history.txt","r+");//opening history file
    if(history==NULL)
    {
        perror("error openning history file");
        exit(EXIT_FAILURE);
    }
    char **historybuffer=create2dim(MAX_HISTORY_COUNT,MAX_HISTORY_LENGHT);// this is dynamic buffer for history file 
    if(historybuffer==NULL)
    {
        exit(EXIT_FAILURE);  
    }
     char bin[1];
     
    fscanf(history,"%d",historybuffer[0]);
    fseek(history,1,SEEK_CUR);

    for(int i=1;i<101;i++)
    {
        fgets(historybuffer[i],MAX_HISTORY_LENGHT-1,history);
    }
    
    
    /////////////////////////////////////////////
    /*set is developed with 2 files 1.setkey and 2.setvalue, */
   FILE *setkeyfile=fopen("/home/cyberfox/Desktop/project/setkey","r+");//
   FILE *setvaluefile=fopen("/home/cyberfox/Desktop/project/setvalue","r+");
   if(setkeyfile==NULL ||setvaluefile==NULL)
   {
    perror("error oepening files for set\n");
    exit(EXIT_FAILURE);
   }
   ////
   char currentdirectory[150];//buffer to keep pwd
   char temporarydirectorypath[150];//
   strcpy(currentdirectory,"~");

   //////////////////////////////////////////////
   char ** setkeybuf=create2dim(SET_MAX_COUNT,SET_MAX_LENGHT);//creating buffers for set variables
   char **setvaluebuf=create2dim(SET_MAX_COUNT,SET_MAX_LENGHT);
   if(setkeybuf==NULL||setvaluebuf==NULL)
   {
    perror("error creating buffer for set command\n");
    exit(EXIT_FAILURE);
   }
   int keycount=0;
   int valuecount=0;
   while(!fscanf(setkeyfile,"%s",setkeybuf[keycount]))
   {
    keycount++;
   }
   keycount++;
   while(!fscanf(setvaluefile,"%s",setvaluebuf[valuecount]))
   {
    valuecount++;
   }
   valuecount++;
   //char pointer command is for user input , for command line , which i want to tokenize in 2 dim array
   char *command=calloc(COMMAND_SIZE,sizeof(char));
   char **tokenbuffer=create2dim(PARAMETER_COUNT,PARAMETER_LENGHT);
   int nexthistory=historybuffer[0][0]+1;;//to configure index next history to be placed
    while(state==1)
    {
        fputs("mysh>",stdout);
        command=fgets(command,COMMAND_SIZE-1,stdin);//user to input the command
        if(command==NULL)
        {
            perror("problem with writiing command\n");
            exit(EXIT_FAILURE);
        }
        if(command[0]!='\n')
        {
       
        strcpy(historybuffer[nexthistory],command);//putting command in history buffer
        nexthistory++;//incrementing the number of history
        }
        char* removenewline=strchr(command,'\n'); //removing \n got from fgets and change to \0
        *removenewline='\0';
        if(nexthistory==200)
        {
            fseek(history,0,SEEK_SET);
            if(fprintf(history,"%d",100)<0)
            {
                perror("error\n");
                return -1;
            }
            fseek(history,1,SEEK_CUR);
            for(int i=100,j=0;i<200;i++,j++)
            {
                strcpy(historybuffer[j],historybuffer[i]);
            }
            nexthistory=100;
        }
    
        
       //tokenizing the command line
        int tokencount=0;
        char** tokens=tokenizer(command,&tokencount,tokenbuffer);
       ////
       char *argv[PARAMETER_COUNT+1];
       for(int i=0;i<tokencount;i++)
       {
        argv[i]=tokens[i];
       }
       argv[tokencount]=NULL;
       
       ////
       strcpy(arrbuildin[7],"help");
       ////detecting if it is builtinn
       int detectbuiltin=-1;
       isbuiltin(arrbuildin,tokens,&detectbuiltin);
       ////
         switch (detectbuiltin)
         {
            case 0: //pwd
            mypwd(tokencount,currentdirectory);
            break;
            case 1: //cd
            mycd(tokens,currentdirectory,temporarydirectorypath,tokencount);
            break;
            case 2: //exit
            myexit(&state);
            break;
            case 3: //set
            myset(setkeybuf,setvaluebuf,tokens,&keycount,&valuecount,tokencount);
            break;
            case 4: //unset
            myunset(setkeybuf,setvaluebuf,tokens,&keycount,&valuecount,tokencount);
            break;
            case 5: //echo
            myecho(tokens,tokencount,setkeybuf,setvaluebuf,keycount);
            break;
            case 6://history
            myhistory(historybuffer,historybuffer[0][0],nexthistory);
            break;
            case 7:
            myhelp(tokens);
            break;
            default:
            pid_t pid=fork();
            wait(NULL);
            if(pid==0)
            {
             execvp(argv[0],argv);
             exit(0);
            }
           
         };

     }
//cleaning the dynamic memory and FILE objeckts     
saveinhistory(historybuffer,historybuffer[0][0],nexthistory,history);
fflush(history);
del2dim(historybuffer,MAX_HISTORY_COUNT);
del2dim(setkeybuf,SET_MAX_COUNT);
del2dim(setvaluebuf,SET_MAX_COUNT);
del2dim(tokenbuffer, PARAMETER_COUNT);
del2dim(arrbuildin,COUNT_BUILTIN);
free(command);
free(isbuiltinread);
return 0;
}
char** create2dim(int rows, int columns) 
{
    char** arr2dim=(char**)malloc(rows*sizeof(char*));
    if(arr2dim==NULL)
    {
        perror("dynamic memory for buildin list is failed");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<rows;i++)
    {
        arr2dim[i]=malloc(columns*(sizeof(char)));
        if(arr2dim[i]==NULL)
        {
            perror("dynamic memory for buildin list is failed");
            exit(EXIT_FAILURE); 
        }
    }
    return arr2dim;
}
void del2dim(char** arr, int rows)
{
    for(int i=0;i<rows;i++)
    {
        free(arr[i]);
    }
    free(arr);
}
char** tokenizer(char* arr,int *tokencount,char** token2dimarr)
{   int counttok=0;
    char *tmp=strtok(arr," ");
    while(tmp!=NULL)
    {
        strcpy(token2dimarr[counttok],tmp);
        
        counttok++;
        tmp=strtok(NULL," ");
       
    }
    *tokencount=counttok;
    
    return token2dimarr;

}
void isbuiltin( char **buildinlist, char **firsttoken,int *isbuiltin)
{
    for(int i=0;i<COUNT_BUILTIN;i++)
        {
            if(!strcmp(buildinlist[i],firsttoken[0]))
            {
                *isbuiltin=i;
                break;
            }
            
        }
        
}
void myhelp(char** arr)
{  
         if(!strcmp(arr[1],"pwd"))
        {
        printf("Retrieves the shell's known path for PWD and prints it. If you keep PWD in the dictionary, this is just looking up PWD and showing it.\n");
        }
         else if(!strcmp(arr[1],"cd"))
        {
        fputs("Changes the shell's idea of the current directory. You can check if the path  is absolute or relative, then apply logic to update PWD on success\n",stdout);
        }
        else if(!strcmp(arr[1],"echo"))
        {
            fputs("Prints text, performing variable expansion on tokens that start with $. For example, echo hello $USER would replace $USER with that dictionary entry's value.\n",stdout);
        }
        else if(!strcmp(arr[1],"unset"))
        {
            fputs("Removes a named variable from your dictionary, so future expansions of $NAME produce empty or an error\n",stdout);
        }
        else if(!strcmp(arr[1],"set"))
        {
            fputs("Allows creating or updating variables. If the user types set NAME=VALUE, you add or modify that variable in your dictionary.\n",stdout);
        }
        else if(!strcmp(arr[1],"history"))
        {
            fputs("Displays previously entered commands. The shell internally stores up to 500 lines in a circular or ring buffer. By default",stdout);
        }
        else
        {
            perror("wrong builtin arguments for help command");
            return;
        }
       
}
void myexit(int *state)
{   
    *state=0;
    printf("End of shell session:thank you\n");
}
int myset(char **setkeybuf,char **setvaluebuf,char **tokens,int *setkeyvalue,int *setvaluevalue,int tokenscount)
{

 if(tokenscount==1)
 {
    for(int i=0;i<*setkeyvalue;i++)
    {
        printf("%s=%s\n",setkeybuf[i],setvaluebuf[i]);
    }
    return 0;
 } 


 if(tokenscount==2)
 {
  char *equalptr=strchr(tokens[1],61);
  if(equalptr==NULL)
  {
    for(int i=0;i<*setkeyvalue;i++)
    {
        if(!strcmp(setkeybuf[i],tokens[1]))
        {
            setvaluebuf[i][0]='\0';
            break;
        }
    }
    return 0;
  }
  if(equalptr!=NULL)
  {
    *equalptr='\0';
    for(int i=0;i<*setkeyvalue;i++)
    {
        if(!strcmp(setkeybuf[i],tokens[1]))
        {
            strcpy(setvaluebuf[i],equalptr+1);
        }
       
    }
    setkeybuf[*setkeyvalue]=tokens[1];
    strcpy(setvaluebuf[*setkeyvalue],equalptr+1);
    *setkeyvalue+=1;
    *setvaluevalue+=1;
    

  }
  
 }
 return 0;

}
int myunset(char **setkeybuf,char **setvaluebuf,char **tokens,int *setkeyvalue,int *setvaluevalue,int tokenscount)
{ 
    int count=-1;
    if(tokenscount>2)
    {
        printf("too much arguments in unset command\n");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<*setkeyvalue;i++)
    {
        if(!strcmp(setkeybuf[i],tokens[1]))
        {
         count=i;
         break;
        }
    }
    if(count==-1)
    {
        printf("there is no variable key in set list\n");
        return -1;
    }
    for(int i=count;i<*setkeyvalue-1 ;i++)
    {
        strcpy(setkeybuf[i],setkeybuf[i+1]);
        strcpy(setvaluebuf[i],setvaluebuf[i+1]);
    }
    setkeybuf[*setkeyvalue-1][0]='\0';
    setvaluebuf[*setkeyvalue-1][0]='\0';
    *setkeyvalue-=1;
    *setvaluevalue-=1;
    return 0;
}

void myecho(char** tokens,int tokencounts,char** setkeybuf,char**setvaluebuf,int keycount)
{
    for(int i=1;i<tokencounts;i++)
    {
        if(tokens[i][0]=='$')
        {
            for(int j=0;j<keycount;j++)
            {
                if(!strcmp(tokens[i]+1,setkeybuf[j]))
                {
                    printf("%s ",setvaluebuf[j]);
                }
            }
        }
        else
        {
            printf("%s ",tokens[i]);
        }
    }
    printf("\n");
    
}
void myhistory(char **historybuffer,int historycount, int nexthistory)
{
    if(nexthistory<101)
    {
        for(int i=1;i<nexthistory;i++)
        {
            fputs(historybuffer[i],stdout);
        }
    }
    else
    {
      for(int i=nexthistory-100;i<nexthistory;i++)
      {
        fputs(historybuffer[i],stdout);
      }
    }
}
int saveinhistory(char **historybuffer,int historycount, int nexthistory,FILE *history)
{  
    int bin;
    //fseek(history,0,SEEK_SET);
    //fprintf(history,"%d",nexthistory-1);
    //fseek(history,1,SEEK_CUR);

    if(nexthistory<101)
    {
        fseek(history,0,SEEK_SET);
        fprintf(history,"%d",nexthistory-1);
        fseek(history,1,SEEK_CUR);
        for(int i=1;i<nexthistory;i++)
        {
            fputs(historybuffer[i],history);
            
        }
    }
    else
    {
        fseek(history,0,SEEK_SET);
        fprintf(history,"%d",100);
        fseek(history,1,SEEK_CUR);
      for(int i=nexthistory-100;i<nexthistory;i++)
      {
        fputs(historybuffer[i],history);
      }
    }
    return 0;
}
void mypwd(int tokencount,char* currentdirectory)
{
    if(tokencount>1)
    {
        printf("more arguments than it should be\n");
    }
    else
    {
       printf("%s\n",currentdirectory);
    }
}
void mycd(char** commandline,char* currentdirectory,char *temporarydirectorypath,int tokencount)

{   
    if(tokencount==1)
    {
        strcpy(currentdirectory,"~");
        return;
    }
    else if(tokencount>2)
    {
        printf("error:check argument count\n");
        return;
    }
    if(commandline[1][0]=='/')
    {
        strcpy(temporarydirectorypath,commandline[1]);
    }
    else
    {   
        strcpy(temporarydirectorypath,currentdirectory);
        strcat(temporarydirectorypath,"/");
        strcat(temporarydirectorypath,commandline[1]);
    }
    int fd_directory=open(temporarydirectorypath,__O_DIRECTORY);
    if(fd_directory==-1)
    {
      printf("error:invalid path\n");
      return;

    }
    else
    {
        strcpy(currentdirectory,temporarydirectorypath);
        close(fd_directory);
    }
}