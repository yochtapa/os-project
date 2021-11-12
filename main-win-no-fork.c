#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct node* nodeptr;
typedef struct node
{
   char data[100];
   nodeptr link;
}node;

nodeptr head;

typedef struct tree* treeptr;
typedef struct tree
{
    char type;
    int permission[3];
    char user[20];
    char group[20];
    int time[4];
    char name[20];
    treeptr lchild,rsibling,lsibling,previous;
}tree;

treeptr root,user,current;

typedef struct bin* binptr;
typedef struct bin
{
    treeptr trash;
    binptr link;
}bin;

binptr binhead;

void logIn();
void runLinux();
void giveTime(int *x);
void savepre(FILE *fp,treeptr t);
void save();
void loadpre(FILE *fp,treeptr *t);
void load();
nodeptr input(char *x, nodeptr y, char *z);
void freeLinkedList();
void printCurrentState();
void pwd(treeptr cur,treeptr rot);
void cd();
void makedir();
void removedir();
void remov();
void ls();
void cat();
void tre();
void find();
void dividePermission(int x);

int main()
{
    logIn();
    runLinux();
}

void giveTime(int *x)
{
    time_t timer;
    struct tm *now;
    timer = time(NULL);
    now = localtime(&timer);

    x[0] = now->tm_mon + 1;
    x[1] = now->tm_mday;
    x[2] = now->tm_hour;
    x[3] = now->tm_min;
}

void savepre(FILE *fp,treeptr t)
{
    if(t!=NULL)
    {
        fwrite(t,sizeof(tree),1,fp);
        savepre(fp,t->lchild);
        savepre(fp,t->rsibling);
    }
}

void save()
{
    FILE *fp = fopen("data.bin","wb");
    savepre(fp,root);
    fclose(fp);
}

void loadpre(FILE *fp,treeptr *t)
{
    if(*t!=NULL)
    {
        if((*t)->lchild != NULL)
        {
            (*t)->lchild = (treeptr)malloc(sizeof(tree));
            fread((*t)->lchild,sizeof(tree),1,fp);
            (*t)->lchild->previous = (*t);
            loadpre(fp,&((*t)->lchild));
        }
        if((*t)->rsibling != NULL)
        {
            (*t)->rsibling=(treeptr)malloc(sizeof(tree));
            fread((*t)->rsibling, sizeof(tree), 1, fp);
            (*t)->rsibling->lsibling=(*t);
            (*t)->rsibling->previous=(*t)->previous;
            loadpre(fp,&((*t)->rsibling));
        }
    }

}

void load()
{
    root = (treeptr)malloc(sizeof(tree));
    FILE *fp = fopen("data.bin","rb");
    fread(root,sizeof(tree),1,fp);
    loadpre(fp,&root);
    fclose(fp);
}

void logIn()
{
    FILE *fp = fopen("data.bin","a+b");

    fseek(fp,0,SEEK_END);
    if(ftell(fp) == 0)
    {
        printf("****************************************************\n");
        printf("* Hello, Welcome to Mini Linux for the first time. *\n");
        printf("****************************************************\n");
        root = (treeptr)malloc(sizeof(tree));
        root->type = 'd';
        root->permission[0] = 7;
        root->permission[1] = 5;
        root->permission[2] = 5;
        strcpy(root->user,"root");
        strcpy(root->group,"root");
        giveTime(root->time);
        strcpy(root->name,"root");
        root->lchild = root->lsibling = root->rsibling = root->previous = NULL;

        char username[20];
        printf("input user : ");
        gets(username);

        treeptr newnode = (treeptr)malloc(sizeof(tree));
        newnode->type = 'd';
        newnode->permission[0] = 7;
        newnode->permission[1] = 5;
        newnode->permission[2] = 5;
        strcpy(newnode->user,username);
        strcpy(newnode->group,username);
        giveTime(newnode->time);
        strcpy(newnode->name,username);
        newnode->lchild = newnode->lsibling = newnode->rsibling = newnode->previous = NULL;

        root->lchild = newnode;
        newnode->previous = root;
        user = newnode;
        current = user;
        save();
    }
    else
    {
        printf("**************************************\n");
        printf("* Hello, Welcome back to Mini Linux. *\n");
        printf("**************************************\n");
        load();

        int suc = 0;

        while(!suc)
        {
            printf("user list :\n");
            treeptr temp = root->lchild;
            while(temp!=NULL)
            {
                printf("%s\n",temp->name);
                temp = temp->rsibling;
            }

            char username[20];
            printf("\nchoose user : ");
            gets(username);

            if(strcmp(username,"plus")) //user추가 x
            {
                 temp = root->lchild;
                 while(temp!=NULL) // user가 있는지 없는지 검사
                 {
                     if(!strcmp(temp->name,username)) //user가 있다면 suc = 1
                     {
                         suc = 1;
                         break;
                     }

                     temp = temp->rsibling;
                 }


                 if(suc) //유저 네임이 맞다면
                 {
                     //비밀번호 검사

                     user = temp;
                     current = user;
                     break;
                 }
                 else
                 {
                     printf("No user\n");
                 }

            }
            else // user추가
            {
                printf("input user : ");
                gets(username);
                treeptr newnode = (treeptr)malloc(sizeof(tree));
                newnode->type = 'd';
                newnode->permission[0] = 7;
                newnode->permission[1] = 5;
                newnode->permission[2] = 5;
                strcpy(newnode->user,username);
                strcpy(newnode->group,username);
                giveTime(newnode->time);
                strcpy(newnode->name,username);
                newnode->lchild = newnode->lsibling = newnode->rsibling = newnode->previous = NULL;

                temp = root->lchild;
                while(temp->rsibling!=NULL)
                {
                    temp = temp->rsibling;
                }

                newnode->previous = temp->previous;
                newnode->lsibling = temp;
                temp->rsibling = newnode;
                printf("\n");
            }

        }

         save();
    }

    fclose(fp);
    printf("\n");
    printf("*******************\n");
    printf("* Log in success. *\n");
    printf("*******************\n\n");

    printf("welcome, %s\n",user->name);
}

void runLinux()
{
    int end = 0;

   //type until end==1
    while(end!=1)
    {

        printCurrentState();

        char inputd[100];
        gets(inputd);
        head = input(inputd,head," ");
        //input instruction & make instruction Linked List

          if(head == NULL)
          {
             printf("error\n");
          }
          else if(!strcmp(head->data, "end"))
          {
             end++;
          }
          else if(!strcmp(head->data, "pwd"))
          {
             pwd(current,root);
          }
          else if(!strcmp(head->data, "cd"))
          {
            cd();
          }
          else if(!strcmp(head->data, "mkdir"))
          {
             makedir();
          }
          else if(!strcmp(head->data, "rm"))
          {
             remov();
          }
          else if(!strcmp(head->data, "rmdir"))
          {
             removedir();
          }
          else if(!strcmp(head->data, "ls"))
          {
             ls();
          }
          else if(!strcmp(head->data, "cat"))
          {
             cat();
          }
          else if(!strcmp(head->data, "tree"))
          {
             tre();
          }
          else if(!strcmp(head->data, "find"))
          {
             find();
          }
          else
          {
             printf("wrong\n");
          }

          save();
          //free linked list
          freeLinkedList();
        }
}

nodeptr input(char *x, nodeptr y, char *z) //complete
{
	char *token = strtok(x, z);
	nodeptr cur;

	while(token!=NULL)
	{
		nodeptr newnode = (nodeptr)malloc(sizeof(node));
		strcpy(newnode->data,token);
		newnode->link = NULL;

		if(y == NULL)
		{
			y = newnode;
			cur = y;
		}
		else
		{
			cur->link = newnode;
			cur = newnode;
		}

		token = strtok(NULL, z);
	}

	return y;
}

void freeLinkedList() //complete
{
   while(head!=NULL)
   {
      nodeptr pre,cur;
      pre = head;
      cur = pre->link;

      if(cur!=NULL)
      {
         while(cur->link!=NULL)
         {
            pre = pre->link;
            cur = pre->link;
         }
         pre->link = NULL;
         free(cur);
      }
      else
      {
         head = NULL;
      }

   }
}

void printCurrentState()
{
    if(current == root)
    {
        printf("%s@%s:/$ ",current->user,current->group);
    }
    else if(current == user)
    {
        printf("%s@%s:~$ ",current->user,current->group);
    }
    else
    {
        printf("%s@%s:",current->user,current->group);
        treeptr temptree = user;
        treeptr tempcur = current;
        while(temptree != current)
        {
            tempcur = current;
            while(tempcur->previous!=temptree)
            {
                tempcur = tempcur->previous;
            }
            if(temptree == user)
            {
                printf("~");
            }
            else
            {
                printf("/%s",temptree->name);
            }

            temptree = tempcur;
        }
        printf("/%s$ ",current->name);
    }
}

void pwd(treeptr cur,treeptr rot)
{
    treeptr temptree = rot;
    treeptr tempcur = cur;
    while(temptree != cur)
    {
        tempcur = cur;
        while(tempcur->previous!=temptree)
        {
            tempcur = tempcur->previous;
        }
        if(temptree != root)
        {
            printf("/%s",temptree->name);
        }
        else if(temptree == root && tempcur == NULL)
        {
            printf("/");
        }

        temptree = tempcur;
    }
    printf("/%s\n",cur->name);
}

void cd()
{

    if(head->link == NULL)
    {
        current = user;
        return;
    }

    nodeptr temp = head->link;
    char svdata[100];
    strcpy(svdata,temp->data);

    if(temp->data[0] == '/') //절대 경로
    {
        char *token = strtok(temp->data,"/");
        treeptr tempcur = root;
        treeptr tempuser = user;
        char *temppath;

        if(token == NULL) // path : /
        {
            current = root;
            user = root;
            return;
        }

        treeptr tempcur2 = tempcur->lchild;
        if(tempcur2 == NULL)
        {
            printf("bash: cd: %s: no file or directory\n",svdata);
            return;
        }
        else
        {
            while(tempcur2!=NULL)
            {
                if(!strcmp(token,tempcur2->name))
                {
                    break;
                }
                tempcur2 = tempcur2->rsibling;
            }

            if(tempcur2==NULL)
            {
                printf("bash: cd: %s: no file or directory\n",svdata);
                return;
            }
            else
            {
                tempcur = tempcur2;
            }

        }
        token = strtok(NULL,"/");

        tempuser = tempcur2;


        while(token!=NULL)
        {
            tempcur2 = tempcur->lchild;
            if(tempcur2 == NULL)
            {
                printf("bash: cd: %s: no file or directory\n",svdata);
                return;
            }
            else
            {
                while(tempcur2!=NULL)
                {
                    if(!strcmp(token,tempcur2->name) && tempcur2->type == 'd')
                    {
                        break;
                    }
                    tempcur2 = tempcur2->rsibling;
                }

                if(tempcur2==NULL)
                {
                    printf("bash: cd: %s: no file or directory\n",svdata);
                    return;
                }
                else
                {
                    tempcur = tempcur2;
                }

            }
            token = strtok(NULL,"/");
        }

        if(tempcur->type == 'd')
        {
            current = tempcur;
            user = tempuser;
        }
        else
        {
            printf("bash: cd: not directory\n");
            return;
        }





    }
    else //상대 경로
    {
        char *token = strtok(temp->data,"/");
        treeptr tempcur = current;
        treeptr tempuser = user;
        while(token!=NULL)
        {
            if(!strcmp(token,"."))
            {

            }
            else if(!strcmp(token,".."))
            {
                if(tempcur == root)
                {
                    tempcur = tempcur;
                }
                else
                {
                    if(tempcur->previous == root)
                    {
                        tempuser = root;
                    }
                    tempcur = tempcur->previous;
                }

            }
            else
            {
                treeptr tempcur2 = tempcur->lchild;
                if(tempcur2 == NULL)
                {
                    printf("bash: cd: %s: no file or directory\n",svdata);
                    return;
                }

                while(tempcur2!=NULL)
                {
                    if(!strcmp(tempcur2->name,token)&&tempcur2->type=='d')
                    {
                        break;
                    }
                    tempcur2 = tempcur2->rsibling;
                }

                if(tempcur2 == NULL)
                {
                    printf("bash: cd: %s: no file or directory\n",svdata);
                    return;
                }
                else
                {
                    if(tempcur == root)
                    {
                        tempuser = tempcur2;
                        tempcur = tempuser;
                    }
                    else
                    {
                        tempcur = tempcur2;
                        tempuser = tempuser;
                    }

                }



            }

            token = strtok(NULL,"/");
        }

        if(tempcur->type == 'd')
        {
            current = tempcur;
            user = tempuser;
        }
        else
        {
            printf("bash: cd: not directory\n");
            return;
        }
    }


}


void makedirectory(nodeptr temp,char *permission)
{

    char svdata[50];
    strcpy(svdata,temp->data);

            if(temp->data[0] == '/') //절대 경로
            {
                char *token = strtok(temp->data,"/");

                if(token == NULL) // path : /
                {
                    printf("mkdir : '%s' can't make directory: deny permission\n",svdata);
                    return;
                }

                // path : /js
                treeptr temptree = root->lchild;
                while(temptree!=NULL)
                {
                    if(!strcmp(temptree->name,token) && temptree->type == 'd')
                    {
                        break;
                    }
                    temptree = temptree->rsibling;
                }

                if(temptree == NULL)
                {
                    printf("mkdir: %s wrong directory\n",svdata);
                    return;
                }


                token = strtok(NULL,"/");

                if(token == NULL) // path : /js //유저 추가하는 것
                {
                    printf("mkdir : '%s' can't make directory: deny permission\n",svdata);
                    return;
                }

                 //path : /jinseob/js //temptree = user
                char *temppath;
                while(token!=NULL)
                {
                    temppath = token;
                    token = strtok(NULL,"/");

                    if(temptree->lchild == NULL)
                    {
                        if(token!=NULL)
                        {
                            printf("mkdir: can't make '%s': no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            //printf("make directory1 %s/%s\n",temptree->name,temppath);
                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                            newtree->type = 'd';
                            strcpy(newtree->name,temppath);
                            strcpy(newtree->user,temptree->user);
                            strcpy(newtree->group,temptree->group);
                            giveTime(newtree->time);
                            newtree->permission[0] = permission[0] - '0';
                            newtree->permission[1] = permission[1] - '0';
                            newtree->permission[2] = permission[2] - '0';
                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                            newtree->previous = temptree;
                            temptree->lchild = newtree;
                        }
                    }
                    else
                    {
                        treeptr temptree2 = temptree->lchild;
                        while(temptree2!=NULL)
                        {
                            if(!strcmp(temptree2->name,temppath) && temptree2->type == 'd')
                            {
                                if(token != NULL)
                                {
                                    temptree = temptree2;
                                    break;
                                }
                                else
                                {
                                    printf("mkdir :can't make %s: already have file\n",svdata);
                                    return;
                                }
                            }
                            temptree2 = temptree2 ->rsibling;
                        }

                        if(temptree2 == NULL)
                        {
                            //printf("make directory2 %s/",temptree->name);
                            temptree2 = temptree->lchild;

                            while(temptree2->rsibling!=NULL)
                            {
                                temptree2 = temptree2->rsibling;
                            }

                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                            newtree->type = 'd';
                            strcpy(newtree->name,temppath);
                            strcpy(newtree->user,temptree->user);
                            strcpy(newtree->group,temptree->group);
                            giveTime(newtree->time);
                            newtree->permission[0] = permission[0] - '0';
                            newtree->permission[1] = permission[1] - '0';
                            newtree->permission[2] = permission[2] - '0';
                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                            newtree->previous = temptree;
                            newtree->lsibling = temptree2;
                            temptree2->rsibling = newtree;

                        }

                    }


                }
                //save();

            }
            else //상대 경로
            {
                treeptr tempcur = current;
                treeptr tempuser = user;
                char *token = strtok(temp->data,"/");
                char *temppath;

                temppath = token;
                token = strtok(NULL,"/");

                if(!strcmp(temppath,"."))
                {

                }
                else if(!strcmp(temppath,".."))
                {
                    tempcur = tempcur->previous;
                }
                else
                {
                    treeptr tempcur2 = tempcur->lchild;
                    if(tempcur2 == NULL && token == NULL)
                    {
                        //printf("make directory 1 %s/%s\n",tempcur->name,temppath);
                        treeptr newtree = (treeptr)malloc(sizeof(tree));
                        newtree->type = 'd';
                        strcpy(newtree->name,temppath);
                        strcpy(newtree->user,tempcur->user);
                        strcpy(newtree->group,tempcur->group);
                        giveTime(newtree->time);
                        newtree->permission[0] = permission[0] - '0';
                        newtree->permission[1] = permission[1] - '0';
                        newtree->permission[2] = permission[2] - '0';
                        newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                        newtree->previous = tempcur;
                        tempcur->lchild = newtree;
                        return;
                    }

                    while(tempcur2!=NULL)
                    {
                        if(!strcmp(tempcur2->name,temppath) && tempcur2->type == 'd')
                        {
                            break;
                        }

                        tempcur2 = tempcur2->rsibling;
                    }

                    if(tempcur2 == NULL)
                    {
                        if(token==NULL)
                        {
                            //printf("make directory2 %s/",tempcur->name);

                            tempcur2 = tempcur->lchild;

                            if(tempcur2 == NULL)
                            {
                                printf("mkdir: can't make '%s': no file or directory\n",svdata);
                                return;
                            }

                            while(tempcur2->rsibling!=NULL)
                            {
                                tempcur2 = tempcur2->rsibling;
                            }


                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                            newtree->type = 'd';
                            strcpy(newtree->name,temppath);
                            strcpy(newtree->user,tempcur->user);
                            strcpy(newtree->group,tempcur->group);
                            giveTime(newtree->time);
                            newtree->permission[0] = permission[0] - '0';
                            newtree->permission[1] = permission[1] - '0';
                            newtree->permission[2] = permission[2] - '0';
                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                            newtree->previous = tempcur;
                            newtree->lsibling = tempcur2;
                            tempcur2->rsibling = newtree;

                            return;
                        }
                        else
                        {
                            if(tempcur == root)
                            {
                                tempcur = tempcur2;
                                tempuser = tempcur2;
                            }
                            else
                            {
                                tempcur = tempcur2;
                                tempuser = tempuser;
                            }
                        }

                    }
                    else
                    {
                        if(token == NULL) // tempcur2는 NULL, 더이상 token도 없음
                        {
                            printf("mkdir :can't make %s: already have file\n",svdata);
                            return;
                        }
                        else
                        {
                            if(tempcur == root)
                            {
                                tempcur = tempcur2;
                                tempuser = tempcur2;
                            }
                            else
                            {
                                tempcur = tempcur2;
                                tempuser = tempuser;
                            }
                        }
                    }


                }


                while(token!=NULL)
                {
                    if(tempcur == NULL)
                    {
                        printf("mkdir: can't make '%s': no file or directory\n",svdata);
                        return;
                    }
                    temppath = token;
                    token = strtok(NULL,"/");

                    if(token!=NULL)
                    {
                        if(!strcmp(temppath,"."))
                        {

                        }
                        else if(!strcmp(temppath,".."))
                        {
                            if(tempcur == root)
                            {
                                tempcur = tempcur;
                            }
                            else
                            {
                                tempcur = tempcur->previous;
                            }

                        }
                        else
                        {
                            treeptr tempcur2 = tempcur->lchild;

                            if(tempcur2 == NULL)
                            {
                                printf("mkdir: can't make '%s': no file or directory\n",svdata);
                                return;
                            }

                            while(tempcur2!=NULL)
                            {
                                if(!strcmp(tempcur2->name,temppath) && tempcur2->type == 'd')
                                {
                                    break;
                                }
                                tempcur2 = tempcur2->rsibling;
                            }


                            if(tempcur2!=NULL)
                            {
                                if(tempcur == root)
                                {
                                    tempcur = tempcur2;
                                    tempuser = tempcur2;
                                }
                                else
                                {
                                    if(tempcur == root)
                                    {
                                        tempcur = tempcur2;
                                        tempuser = tempcur2;
                                    }
                                    else
                                    {
                                        tempcur = tempcur2;
                                        tempuser = tempuser;
                                    }
                                    tempcur = tempcur2;
                                    tempuser = tempuser;
                                }

                            }
                            else
                            {
                                printf("mkdir: can't make '%s': no file or directory\n",svdata);
                                return;
                            }


                        }
                    }
                    else //token == NULL
                    {
                        if(!strcmp(temppath,"."))
                        {
                            printf("mkdir :can't make %s: already have file\n",svdata);
                            return;
                        }
                        else if(!strcmp(temppath,".."))
                        {
                            printf("mkdir :can't make %s: already have file\n",svdata);
                            return;
                        }
                        else
                        {
                            treeptr tempcur2 = tempcur->lchild;

                            if(tempcur2 == NULL)
                            {
                                //printf("make directory1 %s/%s\n",tempcur->name,temppath);
                                treeptr newtree = (treeptr)malloc(sizeof(tree));
                                newtree->type = 'd';
                                strcpy(newtree->name,temppath);
                                strcpy(newtree->user,tempcur->user);
                                strcpy(newtree->group,tempcur->group);
                                giveTime(newtree->time);
                                newtree->permission[0] = permission[0] - '0';
                                newtree->permission[1] = permission[1] - '0';
                                newtree->permission[2] = permission[2] - '0';
                                newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                                newtree->previous = tempcur;
                                tempcur->lchild = newtree;

                                return;
                            }
                            else
                            {
                                while(tempcur2!=NULL)
                                {
                                    if(!strcmp(tempcur2->name,temppath) && tempcur2->type == 'd')
                                    {
                                        printf("mkdir :can't make %s: already have file\n",svdata);
                                        return;
                                    }
                                    tempcur2 = tempcur2->rsibling;
                                }
                               //printf("make directory 2\n");
                            tempcur2 = tempcur->lchild;

                            if(tempcur2 == NULL)
                            {
                                printf("mkdir: can't make '%s': no file or directory\n",svdata);
                                return;
                            }

                            while(tempcur2->rsibling!=NULL)
                            {
                                tempcur2 = tempcur2->rsibling;
                            }


                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                            newtree->type = 'd';
                            strcpy(newtree->name,temppath);
                            strcpy(newtree->user,tempcur->user);
                            strcpy(newtree->group,tempcur->group);
                            giveTime(newtree->time);
                            newtree->permission[0] = permission[0] - '0';
                            newtree->permission[1] = permission[1] - '0';
                            newtree->permission[2] = permission[2] - '0';
                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                            newtree->previous = tempcur;
                            newtree->lsibling = tempcur2;
                            tempcur2->rsibling = newtree;

                                return;
                            }
                        }
                    }


                }
            }

}

void makedirectory_p(nodeptr temp,char *permission)
{
    char svdata[100];
            strcpy(svdata,temp->data);
            if(temp->data[0] == '/') //절대 경로
            {
                char *token = strtok(temp->data,"/");

                if(token == NULL)
                {
                    printf("mkdir: '%s' can't make directory: deny access\n",svdata);
                    return;
                }

                treeptr tempcur = root->lchild;
                if(tempcur == NULL)
                {
                    printf("mkdir: '%s' can't make directory: deny access\n",svdata);
                    return;
                }

                while(tempcur!=NULL)
                {
                    if(!strcmp(tempcur->name,token))
                    {
                        break;
                    }
                    tempcur = tempcur->rsibling;
                }

                if(tempcur == NULL)
                {
                    printf("mkdir: '%s' can't make directory: deny access\n",svdata);
                    return;
                }


                token = strtok(NULL,"/");
                while(token!=NULL) //맨 처음 tempcur = user
                {

                    if(tempcur->lchild == NULL)
                    {
                        //printf("make directory 1\n");
                        treeptr newtree = (treeptr)malloc(sizeof(tree));
                        newtree->type = 'd';
                        strcpy(newtree->name,token);
                        strcpy(newtree->user,tempcur->user);
                        strcpy(newtree->group,tempcur->group);
                        giveTime(newtree->time);
                        newtree->permission[0] = permission[0] - '0';
                        newtree->permission[1] = permission[1] - '0';
                        newtree->permission[2] = permission[2] - '0';
                        newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;
                        newtree->previous = tempcur;
                        tempcur->lchild = newtree;
                        tempcur = tempcur->lchild;
                    }
                    else
                    {
                        //printf("make directory 2 %s/%s\n",tempcur->name,token);
                        treeptr tempcur2 = tempcur->lchild;
                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(tempcur2->name,token) && tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2!=NULL)
                        {
                            printf("mkdir :can't make %s: already have file\n",svdata);
                            return;
                        }
                        else
                        {
                            tempcur2 = tempcur->lchild;
                            while(tempcur2->rsibling!=NULL)
                            {
                                tempcur2=tempcur2->rsibling;
                            }

                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                            newtree->type = 'd';
                            strcpy(newtree->name,token);
                            strcpy(newtree->user,tempcur->user);
                            strcpy(newtree->group,tempcur->group);
                            giveTime(newtree->time);
                            newtree->permission[0] = permission[0] - '0';
                            newtree->permission[1] = permission[1] - '0';
                            newtree->permission[2] = permission[2] - '0';
                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                            newtree->previous = tempcur;
                            newtree->lsibling = tempcur2;
                            tempcur2->rsibling = newtree;
                            tempcur = tempcur2->rsibling;

                        }

                    }

                    token = strtok(NULL,"/");
                }



            }
            else //상대 경로
            {
                char *token = strtok(temp->data,"/");
                treeptr tempcur = current;
                treeptr tempuser = user;

                while(token!=NULL)
                {
                    if(!strcmp(token,"."))
                    {

                    }
                    else if(!strcmp(token,".."))
                    {
                        if(tempcur == root)
                        {
                            tempcur = tempcur;
                        }
                        else
                        {
                            tempcur = tempcur->previous;
                        }
                    }
                    else
                    {
                        if(tempcur->lchild == NULL)
                        {
                            //printf("make directory 1\n");
                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                            newtree->type = 'd';
                            strcpy(newtree->name,token);
                            strcpy(newtree->user,tempcur->user);
                            strcpy(newtree->group,tempcur->group);
                            giveTime(newtree->time);
                            newtree->permission[0] = permission[0] - '0';
                            newtree->permission[1] = permission[1] - '0';
                            newtree->permission[2] = permission[2] - '0';
                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;
                            newtree->previous = tempcur;
                            tempcur->lchild = newtree;
                            tempcur = tempcur->lchild;
                        }
                        else
                        {
                            //printf("make directory 2 %s/%s\n",tempcur->name,token);
                            treeptr tempcur2 = tempcur->lchild;
                            while(tempcur2!=NULL)
                            {
                                if(!strcmp(tempcur2->name,token) && tempcur2->type == 'd')
                                {
                                    break;
                                }
                                tempcur2 = tempcur2->rsibling;
                            }

                            if(tempcur2!=NULL)
                            {
                                tempcur = tempcur2;
                            }
                            else
                            {
                                tempcur2 = tempcur->lchild;
                                while(tempcur2->rsibling!=NULL)
                                {
                                    tempcur2=tempcur2->rsibling;
                                }

                                treeptr newtree = (treeptr)malloc(sizeof(tree));
                                newtree->type = 'd';
                                strcpy(newtree->name,token);
                                strcpy(newtree->user,tempcur->user);
                                strcpy(newtree->group,tempcur->group);
                                giveTime(newtree->time);
                                newtree->permission[0] = permission[0] - '0';
                                newtree->permission[1] = permission[1] - '0';
                                newtree->permission[2] = permission[2] - '0';
                                newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                                newtree->previous = tempcur;
                                newtree->lsibling = tempcur2;
                                tempcur2->rsibling = newtree;
                                tempcur = tempcur2->rsibling;

                            }

                        }
                    }

                    token = strtok(NULL,"/");

                }


            }
}

void makedir()
{
    nodeptr pre = head;
    nodeptr cur = head->link;
    nodeptr temp;
    int option_p = 0;
    int option_m = 0;
    char permission[4];

    pid_t pid[255];

    if(cur == NULL)
    {
        printf("mkdir : wrong instruction\n");
        return;
    }
    else
    {
        while(cur!=NULL)
        {
            if(!strcmp(cur->data,"-m"))
            {
                option_m = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                    strcpy(permission,cur->data);

                    if(cur->link!=NULL)
                    {
                        pre->link = cur->link;
                        free(cur);
                        cur = pre->link;
                    }
                    else
                    {
                        pre->link = NULL;
                        free(cur);
                        break;
                    }
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    printf("mkdir : need more factor\n");
                    return;
                }
            }

            if(!strcmp(cur->data,"-p"))
            {
                option_p = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                    if(!strcmp(cur->data,"-m"))
                    {
                        option_m = 1;
                        if(cur->link!=NULL)
                        {
                            pre->link = cur->link;
                            free(cur);
                            cur = pre->link;
                            strcpy(permission,cur->data);

                            if(cur->link!=NULL)
                            {
                                pre->link = cur->link;
                                free(cur);
                                cur = pre->link;
                            }
                            else
                            {
                                pre->link = NULL;
                                free(cur);
                                break;
                            }
                        }
                        else
                        {
                            pre->link = NULL;
                            free(cur);
                            printf("mkdir : need more factor\n");
                            return;
                        }
                    }
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }

            pre = pre->link;
            cur = pre->link;
        }
    }

    temp = head->link;
    if(temp == NULL)
    {
        printf("mkdir: wrong direction\n");
        return;
    }


    if(!option_m)
    {
        strcpy(permission,"755");
    }


    int k = 0;
    while(temp!=NULL)
    {

                if(option_p)
                {
                    makedirectory_p(temp,permission);
                }
                else
                {
                    makedirectory(temp,permission);
                }


        k++;
        temp = temp->link;
    }





    return;
}

treeptr remove_r_path(treeptr tempcur)
{
    treeptr tempcur2 = tempcur->lchild;

    if(tempcur2->rsibling !=NULL)
    {
        while(tempcur2->rsibling!=NULL)
        {
            tempcur2 = tempcur2->rsibling;
        }

        if(tempcur2->lchild == NULL)
        {
            return tempcur2;
        }
        else
        {
            remove_r_path(tempcur2);
        }
    }
    else
    {
        if(tempcur2->lchild == NULL)
        {
            return tempcur2;
        }
        else
        {
            remove_r_path(tempcur2);
        }
    }

}

void remov_r(nodeptr temp,int option_r,int option_rf)
{
        treeptr tempcur;
        if(temp == NULL)
        {
            printf("rm : wrong operation\n");
            return;
        }

            char svdata[100];
            strcpy(svdata,temp->data);
            char *token;
            char *temppath;

            if(temp->data[0] == '/') //절대 경로
            {
                token = strtok(temp->data,"/");
                tempcur = root;
                treeptr tempuser = user;
                char *temppath;

                if(token == NULL) // path : /
                {
                    printf("rm : can't remove root file\n");
                }

                treeptr tempcur2 = tempcur->lchild;
                if(tempcur2 == NULL)
                {
                    printf("rm : can't remove %s. no file or directory\n",svdata);
                    return;
                }
                else
                {
                    while(tempcur2!=NULL)
                    {
                        if(!strcmp(token,tempcur2->name))
                        {
                            break;
                        }
                        tempcur2 = tempcur2->rsibling;
                    }

                    if(tempcur2==NULL)
                    {
                        printf("rm : can't remove %s. no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        tempcur = tempcur2;
                    }

                }
                temppath = token;
                token = strtok(NULL,"/");

                tempuser = tempcur2;


                while(token!=NULL)
                {
                    temppath = token;
                    tempcur2 = tempcur->lchild;
                    if(tempcur2 == NULL)
                    {
                        printf("rm : can't remove %s. no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(token,tempcur2->name))
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2==NULL)
                        {
                            printf("rm : can't remove %s. no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            tempcur = tempcur2;
                        }

                    }
                    token = strtok(NULL,"/");
                }



            }
            else //상대 경로
            {
                token = strtok(temp->data,"/");
                temppath = token;
                tempcur = current;
                treeptr tempuser = user;
                while(token!=NULL)
                {
                    temppath = token;
                    if(!strcmp(token,"."))
                    {

                    }
                    else if(!strcmp(token,".."))
                    {
                        if(tempcur == root)
                        {
                            tempcur = tempcur;
                        }
                        else
                        {
                            if(tempcur->previous == root)
                            {
                                tempuser = root;
                            }
                            tempcur = tempcur->previous;
                        }

                    }
                    else
                    {
                        treeptr tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("rm : can't remove %s. no file or directory\n",svdata);
                            return;
                        }

                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(tempcur2->name,token))
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2 == NULL)
                        {
                            printf("rm : can't remove %s. no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            if(tempcur == root)
                            {
                                tempuser = tempcur2;
                                tempcur = tempuser;
                            }
                            else
                            {
                                tempcur = tempcur2;
                                tempuser = tempuser;
                            }

                        }



                    }

                    token = strtok(NULL,"/");
                }


            }

            if(tempcur->previous == root)
            {
                printf("rm : can't remove user folder\n");
                return;
            }

            treeptr temptemp = tempcur->rsibling;

            while(temptemp!=NULL)
            {
                if(!strcmp(temptemp->name,tempcur->name))
                {
                    break;
                }
                temptemp = temptemp->rsibling;
            }

            if(temptemp!=NULL)
            {
                int suc = 0;

                printf("folder and file with the same name '%s'.\n",tempcur->name);
                printf("delete folder? file?\n");
                while(!suc)
                {
                    char yn[10];
                    printf("choose : ");
                    gets(yn);

                    if(!strcmp(yn,"folder") || !strcmp(yn,"FOLDER"))
                    {
                        suc = 1;
                        if(tempcur->type != 'd')
                        {
                            tempcur = temptemp;
                        }
                    }
                    else if(!strcmp(yn,"file") || !strcmp(yn,"FILE"))
                    {
                        suc = 1;
                        if(tempcur->type != '-')
                        {
                            tempcur = temptemp;
                        }
                    }
                    else
                    {
                        printf("please input 'folder' or 'file'\n");
                    }

                }

            }

            if(tempcur->type == '-')
            {
                char txt[45];
                strcpy(txt,tempcur->name);
                strcat(txt,tempcur->previous->name);
                strcat(txt,".txt");
                remove(txt);

                if(tempcur->lsibling == NULL)
                {
                    if(tempcur->rsibling==NULL) // x x
                    {
                        tempcur->previous->lchild = NULL;

                        if(option_rf)
                        {
                            free(tempcur);
                        }
                        else
                        {
                            if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                        }

                    }
                    else // x o
                    {

                        tempcur->previous->lchild = tempcur->rsibling;
                        tempcur->rsibling->lsibling = NULL;


                        if(option_rf)
                        {
                            free(tempcur);
                        }
                        else
                        {
                            if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                        }


                    }
                }
                else
                {
                    if(tempcur->rsibling==NULL) // o x
                    {
                        tempcur->lsibling->rsibling = NULL;

                        if(option_rf)
                        {
                            free(tempcur);
                        }
                        else
                        {
                            if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                        }
                    }
                    else // o o
                    {
                        tempcur->lsibling->rsibling = tempcur->rsibling;
                        tempcur->rsibling->lsibling = tempcur->lsibling;

                        if(option_rf)
                        {
                            free(tempcur);
                        }
                        else
                        {
                            if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                        }
                    }
                }



                return;
            }



                while(tempcur->lchild!=NULL)
                {
                    treeptr tempcur2;
                    tempcur2 = remove_r_path(tempcur);

                    treeptr temppre;
                    if(tempcur2->previous == NULL)
                    {
                        temppre = NULL;
                    }
                    else
                    {
                        temppre = tempcur2->previous;
                    }

                    treeptr templcd;
                    if(tempcur2->lsibling == NULL)
                    {
                        templcd = NULL;
                    }
                    else
                    {
                        templcd = tempcur2->lsibling;
                    }

                    if(templcd == NULL)
                    {
                        if(tempcur2->type == 'd')
                        {
                            if(option_rf)
                            {
                                free(temppre->lchild);
                                temppre ->lchild = NULL;
                            }
                            else
                            {
                                //휴지통에 넣어놓기
                                if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur2;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur2;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }

                                temppre ->lchild = NULL;
                            }

                        }
                        else if(tempcur2->type == '-')
                        {
                            if(option_rf)
                            {
                                char txt[45];
                                strcpy(txt,tempcur2->name);
                                strcat(txt,temppre->name);
                                strcat(txt,".txt");
                                remove(txt);
                                free(temppre->lchild);
                                temppre ->lchild = NULL;
                            }
                            else
                            {
                                char txt[45];
                                strcpy(txt,tempcur2->name);
                                strcat(txt,temppre->name);
                                strcat(txt,".txt");
                                remove(txt);
                                if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur2;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur2;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                                temppre ->lchild = NULL;
                            }
                        }

                    }
                    else
                    {

                        if(tempcur2->type == 'd')
                        {
                            if(option_rf)
                            {
                                free(templcd->rsibling);
                                templcd->rsibling = NULL;
                            }
                            else
                            {
                                //휴지통에 넣어놓기
                                if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur2;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur2;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                                templcd->rsibling = NULL;
                            }

                        }
                        else if(tempcur2->type == '-')
                        {
                            if(option_rf)
                            {
                                char txt[45];
                                strcpy(txt,tempcur2->name);
                                strcat(txt,temppre->name);
                                strcat(txt,".txt");
                                remove(txt);
                                free(templcd->rsibling);
                                templcd->rsibling = NULL;
                            }
                            else
                            {
                                char txt[45];
                                strcpy(txt,tempcur2->name);
                                strcat(txt,temppre->name);
                                strcat(txt,".txt");
                                remove(txt);
                                //휴지통에 넣어놓기
                                if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur2;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur2;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                                templcd->rsibling = NULL;
                            }
                        }
                    }

                }

                if(tempcur->lsibling == NULL)
                {
                    if(tempcur->rsibling==NULL) // x x
                    {
                        tempcur->previous->lchild = NULL;

                        if(option_rf)
                        {
                            free(tempcur);
                        }
                        else
                        {
                            if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                        }

                    }
                    else // x o
                    {

                        tempcur->previous->lchild = tempcur->rsibling;
                        tempcur->rsibling->lsibling = NULL;


                        if(option_rf)
                        {
                            free(tempcur);
                        }
                        else
                        {
                            if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                        }


                    }
                }
                else
                {
                    if(tempcur->rsibling==NULL) // o x
                    {
                        tempcur->lsibling->rsibling = NULL;

                        if(option_rf)
                        {
                            free(tempcur);
                        }
                        else
                        {
                            if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                        }
                    }
                    else // o o
                    {
                        tempcur->lsibling->rsibling = tempcur->rsibling;
                        tempcur->rsibling->lsibling = tempcur->lsibling;

                        if(option_rf)
                        {
                            free(tempcur);
                        }
                        else
                        {
                            if(binhead == NULL)
                                {
                                    binhead = (binptr)malloc(sizeof(bin));
                                    binhead->trash = tempcur;
                                    binhead->link = NULL;
                                }
                                else
                                {
                                    binptr newtrash = (binptr)malloc(sizeof(bin));
                                    newtrash->trash = tempcur;
                                    newtrash->link = NULL;

                                    binptr tempbin = binhead;
                                    while(tempbin->link != NULL)
                                    {
                                        tempbin = tempbin->link;
                                    }

                                    tempbin->link = newtrash;

                                }
                        }
                    }
                }



}


void remov_n(nodeptr temp,int option_f)
{
    treeptr tempcur;
        if(temp == NULL)
        {
            printf("rm : wrong operation\n");
            return;
        }

            char svdata[100];
            strcpy(svdata,temp->data);
            char *token;
            char *temppath;

            if(temp->data[0] == '/') //절대 경로
            {
                token = strtok(temp->data,"/");
                tempcur = root;
                treeptr tempuser = user;
                char *temppath;

                if(token == NULL) // path : /
                {
                    printf("rm : can't remove root file\n");
                }

                treeptr tempcur2 = tempcur->lchild;
                if(tempcur2 == NULL)
                {
                    printf("rm : can't remove %s. no file or directory\n",svdata);
                    return;
                }
                else
                {
                    while(tempcur2!=NULL)
                    {
                        if(!strcmp(token,tempcur2->name))
                        {
                            break;
                        }
                        tempcur2 = tempcur2->rsibling;
                    }

                    if(tempcur2==NULL)
                    {
                        printf("rm : can't remove %s. no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        tempcur = tempcur2;
                    }

                }
                temppath = token;
                token = strtok(NULL,"/");

                tempuser = tempcur2;


                while(token!=NULL)
                {
                    temppath = token;
                    tempcur2 = tempcur->lchild;
                    if(tempcur2 == NULL)
                    {
                        printf("rm : can't remove %s. no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(token,tempcur2->name))
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2==NULL)
                        {
                            printf("rm : can't remove %s. no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            tempcur = tempcur2;
                        }

                    }
                    token = strtok(NULL,"/");
                }



            }
            else //상대 경로
            {
                token = strtok(temp->data,"/");
                temppath = token;
                tempcur = current;
                treeptr tempuser = user;
                while(token!=NULL)
                {
                    temppath = token;
                    if(!strcmp(token,"."))
                    {

                    }
                    else if(!strcmp(token,".."))
                    {
                        if(tempcur == root)
                        {
                            tempcur = tempcur;
                        }
                        else
                        {
                            if(tempcur->previous == root)
                            {
                                tempuser = root;
                            }
                            tempcur = tempcur->previous;
                        }

                    }
                    else
                    {
                        treeptr tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("rm : can't remove %s. no file or directory\n",svdata);
                            return;
                        }

                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(tempcur2->name,token))
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2 == NULL)
                        {
                            printf("rm : can't remove %s. no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            if(tempcur == root)
                            {
                                tempuser = tempcur2;
                                tempcur = tempuser;
                            }
                            else
                            {
                                tempcur = tempcur2;
                                tempuser = tempuser;
                            }

                        }



                    }

                    token = strtok(NULL,"/");
                }


            }

            treeptr temppre;
            if(tempcur->previous == NULL)
            {
                temppre = tempcur;
            }
            else
            {
                temppre = tempcur->previous;
            }


            if(tempcur->type == 'd')
            {
                tempcur = temppre->lchild;
                while(tempcur!=NULL)
                {
                    if(!strcmp(tempcur->name,temppath) && tempcur->type == '-')
                    {
                        break;
                    }
                    tempcur = tempcur->rsibling;
                }

                if(tempcur == NULL)
                {
                    printf("rm : Can't remove %s : directory\n",svdata);
                    return;
                }


            }



            treeptr templcd;
            if(tempcur->lsibling == NULL)
            {
                templcd = NULL;
            }
            else
            {
                templcd = tempcur->lsibling;
            }




                if(templcd == NULL) // file 맨 앞일때
                {
                    if(option_f)
                    {
                        if(tempcur->rsibling == NULL) //x x
                        {
                            temppre->lchild = NULL;
                        }
                        else //x o
                        {
                            temppre->lchild = tempcur->rsibling;
                            tempcur->rsibling->lsibling = NULL;
                        }
                        char txt[45];
                        strcpy(txt,tempcur->name);
                        strcat(txt,temppre->name);
                        strcat(txt,".txt");
                        remove(txt);
                        free(temppre->lchild);

                    }
                    else
                    {
                        if(tempcur->rsibling == NULL) // x x
                        {
                            temppre->lchild = NULL;
                        }
                        else // x o
                        {
                            temppre->lchild = tempcur->rsibling;
                            tempcur->rsibling->lsibling = NULL;
                        }
                        char txt[45];
                        strcpy(txt,tempcur->name);
                        strcat(txt,temppre->name);
                        strcat(txt,".txt");
                        remove(txt);

                        if(binhead == NULL)
                        {
                            binhead = (binptr)malloc(sizeof(bin));
                            binhead->trash = tempcur;
                            binhead->link = NULL;
                        }
                        else
                        {
                            binptr newtrash = (binptr)malloc(sizeof(bin));
                            newtrash->trash = tempcur;
                            newtrash->link = NULL;

                            binptr tempbin = binhead;
                            while(tempbin->link != NULL)
                            {
                                tempbin = tempbin->link;
                            }

                            tempbin->link = newtrash;

                        }
                    }
                }
                else //여러개가 들어있을 때
                {
                    if(option_f)
                    {
                        char txt[45];
                        strcpy(txt,tempcur->name);
                        strcat(txt,temppre->name);
                        strcat(txt,".txt");
                        remove(txt);
                        if(tempcur->rsibling == NULL) // o x
                        {
                            templcd->rsibling = NULL;
                        }
                        else // o o
                        {
                            templcd->rsibling = tempcur->rsibling;
                            tempcur->rsibling->lsibling = templcd;
                        }

                        free(templcd->rsibling);

                    }
                    else
                    {
                        char txt[45];
                        strcpy(txt,tempcur->name);
                        strcat(txt,temppre->name);
                        strcat(txt,".txt");
                        remove(txt);


                        if(tempcur->rsibling == NULL)
                        {
                            templcd->rsibling = NULL;
                        }
                        else
                        {
                            templcd->rsibling = tempcur->rsibling;
                            tempcur->rsibling->lsibling = templcd;
                        }


                        if(binhead == NULL)
                        {
                            binhead = (binptr)malloc(sizeof(bin));
                            binhead->trash = tempcur;
                            binhead->link = NULL;
                        }
                        else
                        {
                            binptr newtrash = (binptr)malloc(sizeof(bin));
                            newtrash->trash = tempcur;
                            newtrash->link = NULL;

                            binptr tempbin = binhead;
                            while(tempbin->link != NULL)
                            {
                                tempbin = tempbin->link;
                            }

                            tempbin->link = newtrash;

                        }


                    }
                }




}

void remov()
{
    nodeptr pre = head;
    nodeptr cur = pre -> link;
    int option_r = 0;
    int option_rf = 0;
    int option_f = 0;
    pid_t pid[255];

    if(cur==NULL) //옵션 없고 경로 없을 때
    {
        printf("rm : wrong operator\n");
        return;
    }
    else
    {
        while(cur!=NULL)
        {
            if(!strcmp(cur->data,"-r"))
            {
                option_r = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }
            if(!strcmp(cur->data,"-f"))
            {
                option_f = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }
            if(!strcmp(cur->data,"-rf") || !strcmp(cur->data,"-fr"))
            {
                option_rf = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }
            pre = pre->link;
            cur = pre->link;
        }
    }


    nodeptr temp = head->link;
    int k = 0;
    if(temp == NULL)
    {
        printf("rm : wrong operator\n");
        return;
    }
    while(temp!=NULL)
    {

                if(option_r || option_rf)
                {
                    remov_r(temp,option_r,option_rf);
                }
                else
                {
                    remov_n(temp,option_f);
                }


        k++;
        temp = temp->link;
    }

}

void dividePermission(int x)
{
    char a,b,c;
    int temnum;
    temnum = x%2;
    x = x/2;
    if(temnum == 1)
    {
        c = 'x';
    }
    else
    {
        c = '-';
    }
    temnum = x%2;
    x = x/2;
    if(temnum == 1)
    {
        b = 'w';
    }
    else
    {
        b = '-';
    }
    temnum = x%2;
    x = x/2;
    if(temnum == 1)
    {
        a = 'r';
    }
    else
    {
        a = '-';
    }

    printf("%c%c%c",a,b,c);
}

void ls_path(nodeptr tempnode,int option_a, int option_l, int option_al)
{
    treeptr tempcur;
    treeptr tempuser;
    treeptr temptree;
    int linknum;

    char svdata[100];
    strcpy(svdata,tempnode->data);

            if(tempnode->data[0] == '/') //절대 경로
            {
                char *token = strtok(tempnode->data,"/");
                tempcur = root;
                tempuser = user;
                char *temppath;

                if(token == NULL) // path : /
                {
                    current = root;
                    user = root;
                    return;
                }

                treeptr tempcur2 = tempcur->lchild;
                if(tempcur2 == NULL)
                {
                    printf("ls: can't access '%s': no file or directory\n",svdata);
                    return;
                }
                else
                {
                    while(tempcur2!=NULL)
                    {
                        if(!strcmp(token,tempcur2->name) && tempcur2->type == 'd')
                        {
                            break;
                        }
                        tempcur2 = tempcur2->rsibling;
                    }

                    if(tempcur2==NULL)
                    {
                        printf("ls: can't access '%s': no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        tempcur = tempcur2;
                    }

                }
                token = strtok(NULL,"/");

                tempuser = tempcur2;


                while(token!=NULL)
                {
                    tempcur2 = tempcur->lchild;
                    if(tempcur2 == NULL)
                    {
                        printf("ls: can't access '%s': no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(token,tempcur2->name) && tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2==NULL)
                        {
                            printf("ls: can't access '%s': no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            tempcur = tempcur2;
                        }

                    }
                    token = strtok(NULL,"/");
                }

            }
            else // 상대 경로
            {
                char *token = strtok(tempnode->data,"/");
                tempcur = current;
                tempuser = user;
                while(token!=NULL)
                {
                    if(!strcmp(token,"."))
                    {

                    }
                    else if(!strcmp(token,".."))
                    {
                        if(tempcur == root)
                        {
                            tempcur = tempcur;
                        }
                        else
                        {
                            if(tempcur->previous == root)
                            {
                                tempuser = root;
                            }
                            tempcur = tempcur->previous;
                        }

                    }
                    else
                    {
                        treeptr tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("ls: can't access '%s': no file or directory\n",svdata);
                            return;
                        }

                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(tempcur2->name,token) && tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2 == NULL)
                        {
                            printf("ls: can't access '%s': no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            if(tempcur == root)
                            {
                                tempuser = tempcur2;
                                tempcur = tempuser;
                            }
                            else
                            {
                                tempcur = tempcur2;
                                tempuser = tempuser;
                            }

                        }



                    }

                    token = strtok(NULL,"/");
                }

            }


            //ls 표현
            printf("%s:\n",tempcur->name);


            if(option_a || option_al) //option -a or -al
            {
                if(option_a) //option -a
                {
                    printf(".\t..\t");
                }
                else //option -la
                {
                    temptree = tempcur;
                    printf("%c",temptree->type);
                    for(int i=0;i<3;i++)
                    {
                        dividePermission(temptree->permission[i]);
                    }

                    treeptr temp = temptree->lchild;
                    linknum = 2;
                    if(temp != NULL)
                    {
                        while(temp!=NULL)
                        {
                            linknum++;
                            temp = temp->rsibling;
                        }
                    }
                    if(temptree->type == 'd')
                    {
                        printf(" %d",linknum);
                    }
                    else
                    {
                        printf(" 1");
                    }
                    printf(" %s\t%s\t",temptree->user,temptree->group);
                    if(temptree->type == 'd')
                    {
                        printf("4096\t");
                    }
                    printf("%d/%d  %d:%d\t",temptree->time[0],temptree->time[1],temptree->time[2],temptree->time[3]);
                    printf(".\n"); //현재 폴더 내용 출력

                    if(tempcur != root)
                    {
                        temptree = tempcur->previous;
                    }
                    else
                    {
                        temptree = tempcur;
                    }

                    printf("%c",temptree->type);
                    for(int i=0;i<3;i++)
                    {
                        dividePermission(temptree->permission[i]);
                    }

                    temp = temptree->lchild;
                    linknum = 2;
                    if(temp != NULL)
                    {
                        while(temp!=NULL)
                        {
                            linknum++;
                            temp = temp->rsibling;
                        }
                    }
                    if(temptree->type == 'd')
                    {
                        printf(" %d",linknum);
                    }
                    else
                    {
                        printf(" 1");
                    }
                    printf(" %s\t%s\t",temptree->user,temptree->group);
                    if(temptree->type == 'd')
                    {
                        printf("4096\t");
                    }
                    printf("%d/%d  %d:%d\t",temptree->time[0],temptree->time[1],temptree->time[2],temptree->time[3]);
                    printf("..\n"); //이전 폴더 내용 출력
                }

            } // .과 .. 출력



            temptree = tempcur->lchild;
            if(temptree!=NULL)
            {
                while(temptree!=NULL)
                {
                    if(!option_a && !option_al)
                    {
                        if(temptree->name[0] == '.')
                        {
                            temptree = temptree->rsibling;
                            continue;
                        }
                    }

                    if(!option_l && !option_al) //no option or option -a
                    {
                        printf("%s\t",temptree->name);
                    }
                    else
                    {
                        printf("%c",temptree->type);
                        for(int i=0;i<3;i++)
                        {
                            dividePermission(temptree->permission[i]);
                        }

                        treeptr temp = temptree->lchild;
                        linknum = 2;
                        if(temp != NULL)
                        {
                            while(temp!=NULL)
                            {
                                linknum++;
                                temp = temp->rsibling;
                            }
                        }
                        if(temptree->type == 'd')
                        {
                            printf(" %d",linknum);
                        }
                        else
                        {
                            printf(" 1");
                        }
                        printf(" %s\t%s\t",temptree->user,temptree->group);
                        if(temptree->type == 'd')
                        {
                            printf("4096\t");
                        }
                        else
                        {
                            //text file 크기 측정 후 출력
                            char txt[45];
                            strcpy(txt,temptree->name);
                            strcat(txt,temptree->previous->name);
                            strcat(txt,".txt");
                            FILE *fp = fopen(txt,"rt");
                            fseek(fp,0,SEEK_END);
                            printf("%ld\t",ftell(fp));
                            fclose(fp);
                        }
                        printf("%d/%d  %d:%d\t",temptree->time[0],temptree->time[1],temptree->time[2],temptree->time[3]);
                        printf("%s\n",temptree->name);
                    }



                    temptree = temptree->rsibling;
                }
            }

            if(!option_l && !option_al)
            {
                printf("\n");
            }




}

void ls()
{
    nodeptr pre = head;
    nodeptr cur = pre -> link;
    int option_a = 0;
    int option_al = 0;
    int option_l = 0;
    int linknum;

    treeptr temptree; //ls로 볼 애가 있는 노드

    pid_t pid[255];
//ls -a

    if(cur==NULL) //옵션 없고 경로 없을 때
    {
        temptree = current;
    }
    else
    {
        while(cur!=NULL)
        {
            if(!strcmp(cur->data,"-a"))
            {
                option_a = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }
            if(!strcmp(cur->data,"-l"))
            {
                option_l = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }
            if(!strcmp(cur->data,"-la") || !strcmp(cur->data,"-al"))
            {
                option_al = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }
            pre = pre->link;
            cur = pre->link;
        }
    }

    if(head->link == NULL) //경로x
    {
        if(option_a || option_al) //option -a or -al
        {
            if(option_a) //option -a
            {
                printf(".\t..\t");
            }
            else //option -la
            {
                temptree = current;
                printf("%c",temptree->type);
                for(int i=0;i<3;i++)
                {
                    dividePermission(temptree->permission[i]);
                }

                treeptr temp = temptree->lchild;
                linknum = 2;
                if(temp != NULL)
                {
                    while(temp!=NULL)
                    {
                        linknum++;
                        temp = temp->rsibling;
                    }
                }
                if(temptree->type == 'd')
                {
                    printf(" %d",linknum);
                }
                else
                {
                    printf(" 1");
                }

                printf(" %s\t%s\t",temptree->user,temptree->group);
                if(temptree->type == 'd')
                {
                    printf("4096\t");
                }
                printf("%d/%d  %d:%d\t",temptree->time[0],temptree->time[1],temptree->time[2],temptree->time[3]);
                printf(".\n"); //현재 폴더 내용 출력

                if(current != root)
                {
                    temptree = current->previous;
                }
                else
                {
                    temptree = current;
                }

                printf("%c",temptree->type);
                for(int i=0;i<3;i++)
                {
                    dividePermission(temptree->permission[i]);
                }

                temp = temptree->lchild;
                linknum = 2;
                if(temp != NULL)
                {
                    while(temp!=NULL)
                    {
                        linknum++;
                        temp = temp->rsibling;
                    }
                }
                if(temptree->type == 'd')
                {
                    printf(" %d",linknum);
                }
                else
                {
                    printf(" 1");
                }
                printf(" %s\t%s\t",temptree->user,temptree->group);
                if(temptree->type == 'd')
                {
                    printf("4096\t");
                }
                printf("%d/%d  %d:%d\t",temptree->time[0],temptree->time[1],temptree->time[2],temptree->time[3]);
                printf("..\n"); //이전 폴더 내용 출력
            }

        } // .과 .. 출력



        temptree = current->lchild;
        if(temptree!=NULL)
        {
            while(temptree!=NULL)
            {
                if(!option_a && !option_al)
                {
                    if(temptree->name[0] == '.')
                    {
                        temptree = temptree->rsibling;
                        continue;
                    }
                }

                if(!option_l && !option_al) //no option or option -a
                {
                    printf("%s\t",temptree->name);
                }
                else
                {
                    printf("%c",temptree->type);
                    for(int i=0;i<3;i++)
                    {
                        dividePermission(temptree->permission[i]);
                    }

                    treeptr temp = temptree->lchild;
                    linknum = 2;
                    if(temp != NULL)
                    {
                        while(temp!=NULL)
                        {
                            linknum++;
                            temp = temp->rsibling;
                        }
                    }
                    if(temptree->type == 'd')
                        {
                            printf(" %d",linknum);
                        }
                        else
                        {
                            printf(" 1");
                        }
                    printf(" %s\t%s\t",temptree->user,temptree->group);
                    if(temptree->type == 'd')
                    {
                        printf("4096\t");
                    }
                    else
                    {
                        //text file 크기 측정 후 출력
                        char txt[45];
                        strcpy(txt,temptree->name);
                        strcat(txt,temptree->previous->name);
                        strcat(txt,".txt");
                        FILE *fp = fopen(txt,"rt");
                        fseek(fp,0,SEEK_END);
                        printf("%ld\t",ftell(fp));
                        fclose(fp);
                    }
                    printf("%d/%d  %d:%d\t",temptree->time[0],temptree->time[1],temptree->time[2],temptree->time[3]);
                    printf("%s\n",temptree->name);
                }



                temptree = temptree->rsibling;
            }
        }

        if(!option_l && !option_al)
        {
            printf("\n");
        }

    }
    else //경로 o
    {

        nodeptr tempnode = head->link;
        int k = 0;
        while(tempnode!=NULL)
        {
                    ls_path(tempnode,option_a,option_l,option_al);


            k++;
            tempnode = tempnode->link;
        }


    }


}

void cat_path(nodeptr tempnode,treeptr temptree,int option_c,int option_n)
{
    char svdata[100];
            strcpy(svdata,tempnode->data);
            int pathtype; // 0 = 절대경로 / 1 = 상대경로
            char *token;
            if(tempnode->data[0] == '>') //option >
            {
                option_c = 1;
                char *tempdata = strtok(tempnode->data,">");
                if(tempnode->data[1]=='/') //절대 경로
                {
                    pathtype = 0;
                }
                else
                {
                    pathtype = 1;
                }
                token = strtok(tempdata,"/");

            }
            else
            {
                if(tempnode->data[0]=='/') //절대 경로
                {
                    pathtype = 0;
                }
                else
                {
                    pathtype = 1;
                }
                token = strtok(tempnode->data,"/");
            }


            char *temppath;
            if(pathtype == 0) //절대 경로
            {
                temptree = root->lchild;

                if(temptree == NULL)
                {
                    printf("cat : no user error\n");
                    return;
                }

                while(temptree!=NULL)
                {
                    if(!strcmp(temptree->name,token))
                    {
                        break;
                    }
                    temptree = temptree->rsibling;
                }

                if(temptree == NULL)
                {
                    printf("cat: %s: can't make file. deny access\n",svdata);
                    return;
                }


                token = strtok(NULL,"/"); //token = 만들 애 , temptree = user
                while(token!=NULL)
                {
                    temppath = token;
                    token = strtok(NULL,"/");

                    treeptr temptree2 = temptree->lchild;
                    if(temptree2==NULL)
                    {
                        if(token == NULL) //printf("make file 1 %s/%s\n",temptree->name,temppath);
                        {


                            char txt[45];
                            strcpy(txt,temppath);
                            strcat(txt,temptree->name);
                            strcat(txt,".txt");

                            FILE *fp;


                            if(option_c) //create
                            {
                                int i = 1;

                                char catdata[1000];
                                gets(catdata);
                                if(!strcmp(catdata,"end"))
                                {
                                    return;
                                }

                                fp = fopen(txt,"wt");

                                if(option_n)
                                {
                                    fprintf(fp,"\t%d %s\n",i,catdata);
                                    i++;
                                }
                                else
                                {
                                    fprintf(fp,"%s\n",catdata);
                                }


                                fclose(fp);


                                while(1)
                                {
                                    gets(catdata);
                                    if(!strcmp(catdata,"end"))
                                    {
                                        break;
                                    }

                                    fp = fopen(txt,"a+t");
                                    if(option_n)
                                    {
                                        fprintf(fp,"\t%d %s\n",i,catdata);
                                        i++;
                                    }
                                    else
                                    {
                                        fprintf(fp,"%s\n",catdata);
                                    }

                                    fclose(fp);
                                }
                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                            newtree->type = '-';
                            strcpy(newtree->name,temppath);
                            strcpy(newtree->user,temptree->user);
                            strcpy(newtree->group,temptree->group);
                            giveTime(newtree->time);
                            newtree->permission[0] = 6;
                            newtree->permission[1] = 6;
                            newtree->permission[2] = 4;
                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                            newtree->previous = temptree;
                            temptree->lchild = newtree;
                            }



                        }
                        else
                        {
                            printf("cat: %s: wrong directory.\n",svdata);
                            return;
                        }
                    }
                    else
                    {
                        while(temptree2!=NULL)
                        {
                            if(!strcmp(temptree2->name,temppath) && temptree2->type == 'd')
                            {
                                break;
                            }
                            temptree2 = temptree2->rsibling;
                        }

                        if(temptree2 == NULL)
                        {
                            if(token == NULL) //printf("make file2  %s/%s\n",temptree->name,temppath); //이미 lchild에 다른 폴더 또는 파일 있음
                            {


                                char txt[45];
                                strcpy(txt,temppath);
                                strcat(txt,temptree->name);
                                strcat(txt,".txt");


                                if(option_c) //create
                                {

                                    int i = 1;

                                    char catdata[1000];
                                    gets(catdata);
                                    if(!strcmp(catdata,"end"))
                                    {
                                        return;
                                    }

                                    FILE *fp = fopen(txt,"wt");

                                    if(option_n)
                                    {
                                        fprintf(fp,"\t%d %s\n",i,catdata);
                                        i++;
                                    }
                                    else
                                    {
                                        fprintf(fp,"%s\n",catdata);
                                    }


                                    fclose(fp);


                                    while(1)
                                    {
                                        gets(catdata);
                                        if(!strcmp(catdata,"end"))
                                        {
                                            break;
                                        }

                                        fp = fopen(txt,"a+t");
                                        if(option_n)
                                        {
                                            fprintf(fp,"\t%d %s\n",i,catdata);
                                            i++;
                                        }
                                        else
                                        {
                                            fprintf(fp,"%s\n",catdata);
                                        }

                                        fclose(fp);
                                    }

                                    temptree2 = temptree->lchild;
                                    while(temptree2!=NULL)
                                    {
                                        if(!strcmp(temptree2->name,temppath) && temptree2->type == '-')
                                        {
                                            break;
                                        }

                                        temptree2 = temptree2->rsibling;
                                    }

                                    if(temptree2 == NULL)
                                    {
                                        treeptr newtree = (treeptr)malloc(sizeof(tree));
                                        newtree->type = '-';
                                        strcpy(newtree->name,temppath);
                                        strcpy(newtree->user,temptree->user);
                                        strcpy(newtree->group,temptree->group);
                                        giveTime(newtree->time);
                                        newtree->permission[0] = 6;
                                        newtree->permission[1] = 6;
                                        newtree->permission[2] = 4;
                                        newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                                        temptree2 = temptree->lchild;
                                        while(temptree2->rsibling!=NULL)
                                        {
                                            temptree2 = temptree2->rsibling;
                                        }

                                        newtree->previous = temptree2->previous;
                                        newtree->lsibling = temptree2;
                                        temptree2->rsibling = newtree;
                                    }




                                }
                                else //read
                                {
                                    int i = 1;
                                    FILE *fp = fopen(txt,"rt");
                                    if(fp == NULL)
                                    {
                                        printf("cat: %s: no file or directory\n",svdata);
                                        return;
                                    }
                                    char inputcat[1000];
                                    while(fgets(inputcat,1000,fp)!=NULL)
                                    {
                                        if(option_n)
                                        {
                                            printf("\t%d ",i);
                                            printf("%s",inputcat);
                                            i++;
                                        }
                                        else
                                        {
                                            printf("%s",inputcat);
                                        }
                                    }
                                    fclose(fp);
                                }




                                return;
                            }
                            else
                            {
                                printf("cat: %s: wrong direction. no directory\n",svdata);
                                return;
                            }
                        }

                        if(token == NULL) //printf("make file3 %s/%s\n",temptree->name,temppath); //같은 이름의 파일 있을 떄
                        {



                            char txt[45];
                                strcpy(txt,temppath);
                                strcat(txt,temptree->name);
                                strcat(txt,".txt");


                                if(option_c) //create
                                {
                                    int i = 1;

                                    char catdata[1000];
                                    gets(catdata);
                                    if(!strcmp(catdata,"end"))
                                    {
                                        return;
                                    }

                                    FILE *fp = fopen(txt,"wt");

                                    if(option_n)
                                    {
                                        fprintf(fp,"\t%d %s\n",i,catdata);
                                        i++;
                                    }
                                    else
                                    {
                                        fprintf(fp,"%s\n",catdata);
                                    }


                                    fclose(fp);


                                    while(1)
                                    {
                                        gets(catdata);
                                        if(!strcmp(catdata,"end"))
                                        {
                                            break;
                                        }

                                        fp = fopen(txt,"a+t");
                                        if(option_n)
                                        {
                                            fprintf(fp,"\t%d %s\n",i,catdata);
                                            i++;
                                        }
                                        else
                                        {
                                            fprintf(fp,"%s\n",catdata);
                                        }

                                        fclose(fp);
                                    }

                                    temptree2 = temptree->lchild;
                                    while(temptree2!=NULL)
                                    {
                                        if(!strcmp(temptree2->name,temppath) && temptree2->type == '-')
                                        {
                                            break;
                                        }

                                        temptree2 = temptree2->rsibling;
                                    }

                                    if(temptree2 == NULL)
                                    {
                                        treeptr newtree = (treeptr)malloc(sizeof(tree));
                                        newtree->type = '-';
                                        strcpy(newtree->name,temppath);
                                        strcpy(newtree->user,temptree->user);
                                        strcpy(newtree->group,temptree->group);
                                        giveTime(newtree->time);
                                        newtree->permission[0] = 6;
                                        newtree->permission[1] = 6;
                                        newtree->permission[2] = 4;
                                        newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                                        temptree2 = temptree->lchild;
                                        while(temptree2->rsibling!=NULL)
                                        {
                                            temptree2 = temptree2->rsibling;
                                        }

                                        newtree->previous = temptree2->previous;
                                        newtree->lsibling = temptree2;
                                        temptree2->rsibling = newtree;
                                    }


                                }
                                else //read
                                {
                                    int i = 1;
                                    FILE *fp = fopen(txt,"rt");
                                    if(fp == NULL)
                                        {
                                            printf("cat: %s: no file or directory\n",svdata);
                                            return;
                                        }
                                    char inputcat[1000];
                                    while(fgets(inputcat,1000,fp)!=NULL)
                                    {
                                        if(option_n)
                                        {
                                            printf("\t%d ",i);
                                            printf("%s",inputcat);
                                            i++;
                                        }
                                        else
                                        {
                                            printf("%s",inputcat);
                                        }
                                    }
                                    fclose(fp);
                                }
                        }
                        else
                        {
                            temptree = temptree2;
                        }


                    }

                }


            }
            else //상대 경로
            {
                temptree = current;
                treeptr tempuser = user;


                while(token!=NULL)
                {
                    temppath = token;
                    token = strtok(NULL,"/");

                    if(!strcmp(temppath,"."))
                    {

                    }
                    else if(!strcmp(temppath,".."))
                    {
                        if(temptree != root)
                        {
                            temptree = temptree->previous;
                        }

                    }
                    else
                    {
                        treeptr temptree2 = temptree->lchild;
                        if(temptree2 == NULL)
                        {
                            if(token == NULL) //printf("make file1 %s/%s\n",temptree->name,temppath);
                            {

                                char txt[45];
                                strcpy(txt,temppath);
                                strcat(txt,temptree->name);
                                strcat(txt,".txt");

                                FILE *fp;


                                if(option_c) //create
                                {
                                    int i = 1;

                                    char catdata[1000];
                                    gets(catdata);
                                    if(!strcmp(catdata,"end"))
                                    {
                                        return;
                                    }

                                    fp = fopen(txt,"wt");

                                    if(option_n)
                                    {
                                        fprintf(fp,"\t%d %s\n",i,catdata);
                                        i++;
                                    }
                                    else
                                    {
                                        fprintf(fp,"%s\n",catdata);
                                    }


                                    fclose(fp);


                                    while(1)
                                    {
                                        gets(catdata);
                                        if(!strcmp(catdata,"end"))
                                        {
                                            break;
                                        }

                                        fp = fopen(txt,"a+t");
                                        if(option_n)
                                        {
                                            fprintf(fp,"\t%d %s\n",i,catdata);
                                            i++;
                                        }
                                        else
                                        {
                                            fprintf(fp,"%s\n",catdata);
                                        }

                                        fclose(fp);
                                    }
                                treeptr newtree = (treeptr)malloc(sizeof(tree));
                                newtree->type = '-';
                                strcpy(newtree->name,temppath);
                                strcpy(newtree->user,temptree->user);
                                strcpy(newtree->group,temptree->group);
                                giveTime(newtree->time);
                                newtree->permission[0] = 6;
                                newtree->permission[1] = 6;
                                newtree->permission[2] = 4;
                                newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                                newtree->previous = temptree;
                                temptree->lchild = newtree;
                                }
                            }
                            else
                            {
                                printf("cat: %s: wrong direction. no directory\n",svdata);
                                return;
                            }
                        }
                        else
                        {
                            int type = 0;
                            while(temptree2!=NULL)
                            {
                                if(!strcmp(temptree2->name,temppath)&&temptree2->type == 'd')
                                {
                                    break;
                                }
                                temptree2 = temptree2->rsibling;
                            }

                            if(temptree2 == NULL)
                            {


                                if(token == NULL) //printf("make file2 %s/%s\n",temptree->name,temppath);
                                {
                                    if(temptree == root)
                                    {
                                        printf("cat: %s: can't make file. deny access\n",svdata);
                                        return;
                                    }


                                    char txt[45];
                                    strcpy(txt,temppath);
                                    strcat(txt,temptree->name);
                                    strcat(txt,".txt");


                                    if(option_c) //create
                                    {

                                        int i = 1;

                                        char catdata[1000];
                                        gets(catdata);
                                        if(!strcmp(catdata,"end"))
                                        {
                                            return;
                                        }

                                        FILE *fp = fopen(txt,"wt");

                                        if(option_n)
                                        {
                                            fprintf(fp,"\t%d %s\n",i,catdata);
                                            i++;
                                        }
                                        else
                                        {
                                            fprintf(fp,"%s\n",catdata);
                                        }


                                        fclose(fp);


                                        while(1)
                                        {
                                            gets(catdata);
                                            if(!strcmp(catdata,"end"))
                                            {
                                                break;
                                            }

                                            fp = fopen(txt,"a+t");
                                            if(option_n)
                                            {
                                                fprintf(fp,"\t%d %s\n",i,catdata);
                                                i++;
                                            }
                                            else
                                            {
                                                fprintf(fp,"%s\n",catdata);
                                            }

                                            fclose(fp);
                                        }

                                        temptree2 = temptree->lchild;
                                        while(temptree2!=NULL)
                                        {
                                            if(!strcmp(temptree2->name,temppath) && temptree2->type == '-')
                                            {
                                                break;
                                            }

                                            temptree2 = temptree2->rsibling;
                                        }

                                        if(temptree2 == NULL)
                                        {
                                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                                            newtree->type = '-';
                                            strcpy(newtree->name,temppath);
                                            strcpy(newtree->user,temptree->user);
                                            strcpy(newtree->group,temptree->group);
                                            giveTime(newtree->time);
                                            newtree->permission[0] = 6;
                                            newtree->permission[1] = 6;
                                            newtree->permission[2] = 4;
                                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                                            temptree2 = temptree->lchild;
                                            while(temptree2->rsibling!=NULL)
                                            {
                                                temptree2 = temptree2->rsibling;
                                            }

                                            newtree->previous = temptree2->previous;
                                            newtree->lsibling = temptree2;
                                            temptree2->rsibling = newtree;
                                        }




                                    }
                                    else //read
                                    {
                                        int i = 1;
                                        FILE *fp = fopen(txt,"rt");
                                        if(fp == NULL)
                                        {
                                            printf("cat: %s: no file or directory\n",svdata);
                                            return;
                                        }
                                        char inputcat[1000];
                                        while(fgets(inputcat,1000,fp)!=NULL)
                                        {
                                            if(option_n)
                                            {
                                                printf("\t%d ",i);
                                                printf("%s",inputcat);
                                                i++;
                                            }
                                            else
                                            {
                                                printf("%s",inputcat);
                                            }
                                        }
                                        fclose(fp);
                                    }


                                }
                                else
                                {
                                    printf("cat: %s: wrong direction\n",svdata);
                                    return;
                                }
                            }
                            else
                            {
                                if(token == NULL) //printf("make file3 %s/%s\n",temptree->name,temppath);
                                {
                                    if(temptree == root)
                                    {
                                        printf("cat: %s: can't make file. deny access\n",svdata);
                                        return;
                                    }



                                    char txt[45];
                                    strcpy(txt,temppath);
                                    strcat(txt,temptree->name);
                                    strcat(txt,".txt");


                                    if(option_c) //create
                                    {
                                        int i = 1;

                                        char catdata[1000];
                                        gets(catdata);
                                        if(!strcmp(catdata,"end"))
                                        {
                                            return;
                                        }

                                        FILE *fp = fopen(txt,"wt");

                                        if(option_n)
                                        {
                                            fprintf(fp,"\t%d %s\n",i,catdata);
                                            i++;
                                        }
                                        else
                                        {
                                            fprintf(fp,"%s\n",catdata);
                                        }


                                        fclose(fp);


                                        while(1)
                                        {
                                            gets(catdata);
                                            if(!strcmp(catdata,"end"))
                                            {
                                                break;
                                            }

                                            fp = fopen(txt,"a+t");
                                            if(option_n)
                                            {
                                                fprintf(fp,"\t%d %s\n",i,catdata);
                                                i++;
                                            }
                                            else
                                            {
                                                fprintf(fp,"%s\n",catdata);
                                            }

                                            fclose(fp);
                                        }

                                        temptree2 = temptree->lchild;
                                        while(temptree2!=NULL)
                                        {
                                            if(!strcmp(temptree2->name,temppath) && temptree2->type == '-')
                                            {
                                                break;
                                            }

                                            temptree2 = temptree2->rsibling;
                                        }

                                        if(temptree2 == NULL)
                                        {
                                            treeptr newtree = (treeptr)malloc(sizeof(tree));
                                            newtree->type = '-';
                                            strcpy(newtree->name,temppath);
                                            strcpy(newtree->user,temptree->user);
                                            strcpy(newtree->group,temptree->group);
                                            giveTime(newtree->time);
                                            newtree->permission[0] = 6;
                                            newtree->permission[1] = 6;
                                            newtree->permission[2] = 4;
                                            newtree->lchild = newtree->rsibling = newtree->lsibling = newtree->previous = NULL;

                                            temptree2 = temptree->lchild;
                                            while(temptree2->rsibling!=NULL)
                                            {
                                                temptree2 = temptree2->rsibling;
                                            }

                                            newtree->previous = temptree2->previous;
                                            newtree->lsibling = temptree2;
                                            temptree2->rsibling = newtree;
                                        }


                                    }
                                    else //read
                                    {
                                        int i = 1;
                                        FILE *fp = fopen(txt,"rt");
                                        if(fp == NULL)
                                        {
                                            printf("cat: %s: no file or directory\n",svdata);
                                            return;
                                        }
                                        char inputcat[1000];
                                        while(fgets(inputcat,1000,fp)!=NULL)
                                        {
                                            if(option_n)
                                            {
                                                printf("\t%d ",i);
                                                printf("%s",inputcat);
                                                i++;
                                            }
                                            else
                                            {
                                                printf("%s",inputcat);
                                            }
                                        }
                                        fclose(fp);
                                    }

                                }
                                else
                                {
                                    temptree = temptree2;
                                }
                            }



                        }
                    }

                }




            }
}

void cat()
{
    nodeptr pre = head;
    nodeptr cur = pre -> link;
    int option_n = 0;
    int option_c = 0;
    treeptr temptree;
    pid_t pid[255];

    if(cur==NULL) //옵션 없고 경로 없을 때
    {
        temptree = current;
    }
    else
    {
        while(cur!=NULL)
        {
            if(!strcmp(cur->data,"-n"))
            {
                option_n = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }
            pre = pre->link;
            cur = pre->link;
        }
    }

    nodeptr tempnode = head->link;

    int i = 1;

    if(tempnode == NULL) // 경로 x ,출력만 하는 것
    {

        while(1)
        {
            char catdata[100];
            gets(catdata);

            if(!strcmp(catdata,"end"))
            {
                return;
            }

            if(option_n)
            {
                printf("     %d ",i);
                printf("%s\n",catdata);
                i++;
            }
            else
            {
                printf("%s\n",catdata);
            }
        }

    }
    else //경로 o
    {
        int k = 0;
        while(tempnode!=NULL)
        {
                    cat_path(tempnode,temptree,option_c,option_n);

            k++;
            tempnode = tempnode->link;
        }

    }


}

void removedir_n(nodeptr temp)
{
    treeptr tempcur;
    char *token;
        char *temppath;
        char svdata[100];
        strcpy(svdata,temp->data);
        if(temp->data[0] == '/') //절대 경로
            {
                token = strtok(temp->data,"/");
                tempcur = root;
                treeptr tempuser = user;


                if(token == NULL) // path : /
                {
                    printf("rmdir : can't remove root file\n");
                }

                treeptr tempcur2 = tempcur->lchild;
                if(tempcur2 == NULL)
                {
                    printf("rmdir : can't remove %s. no file or directory\n",svdata);
                    return;
                }
                else
                {
                    while(tempcur2!=NULL)
                    {
                        if(!strcmp(token,tempcur2->name))
                        {
                            break;
                        }
                        tempcur2 = tempcur2->rsibling;
                    }

                    if(tempcur2==NULL)
                    {
                        printf("rmdir : can't remove %s. no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        tempcur = tempcur2;
                    }

                }
                temppath = token;
                token = strtok(NULL,"/");

                tempuser = tempcur2;


                while(token!=NULL)
                {
                    temppath = token;
                    tempcur2 = tempcur->lchild;
                    if(tempcur2 == NULL)
                    {
                        printf("rmdir : can't remove %s. no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(token,tempcur2->name) && tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2==NULL)
                        {
                            printf("rmdir : can't remove %s. no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            tempcur = tempcur2;
                        }

                    }
                    token = strtok(NULL,"/");
                }



            }
            else //상대 경로
            {
                token = strtok(temp->data,"/");
                temppath = token;
                tempcur = current;
                treeptr tempuser = user;
                while(token!=NULL)
                {
                    temppath = token;
                    if(!strcmp(token,"."))
                    {

                    }
                    else if(!strcmp(token,".."))
                    {
                        if(tempcur == root)
                        {
                            tempcur = tempcur;
                        }
                        else
                        {
                            if(tempcur->previous == root)
                            {
                                tempuser = root;
                            }
                            tempcur = tempcur->previous;
                        }

                    }
                    else
                    {
                        treeptr tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("rmdir : can't remove %s. no file or directory\n",svdata);
                            return;
                        }

                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(tempcur2->name,token) && tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2 == NULL)
                        {
                            printf("rmdir : can't remove %s. no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            if(tempcur == root)
                            {
                                tempuser = tempcur2;
                                tempcur = tempuser;
                            }
                            else
                            {
                                tempcur = tempcur2;
                                tempuser = tempuser;
                            }

                        }



                    }

                    token = strtok(NULL,"/");
                }


            }

            if(tempcur->previous == root)
            {
                printf("rmdir : fail to remove user directory\n");
                return;
            }

            if(tempcur->lchild == NULL)
            {
                if(tempcur->lsibling == NULL) //x x
                {
                    if(tempcur->rsibling == NULL) //x o
                    {
                        tempcur->previous->lchild = NULL;
                        free(tempcur);
                    }
                    else
                    {
                        tempcur->previous->lchild = tempcur->rsibling;
                        tempcur->rsibling->lsibling = NULL;
                        free(tempcur);
                    }
                }
                else
                {
                    if(tempcur->rsibling == NULL) // o x
                    {
                        tempcur->lsibling->rsibling = NULL;
                        free(tempcur);
                    }
                    else // o o
                    {
                        tempcur->lsibling->rsibling = tempcur->rsibling;
                        tempcur->rsibling->lsibling = tempcur->lsibling;
                        free(tempcur);
                    }
                }
            }
            else
            {
                printf("rmdir : fail to remove directory '%s' : directory is not empty\n",svdata);
                return;
            }


}

void removedir_p(nodeptr temp)
{
    treeptr tempcur;
    char *token;
        char *temppath;
        char svdata[100];
        strcpy(svdata,temp->data);
        treeptr deep;

        if(temp->data[0] == '/') //절대 경로
            {
                token = strtok(temp->data,"/");
                tempcur = root;
                treeptr tempuser = user;


                if(token == NULL) // path : /
                {
                    printf("rmdir : can't remove root file\n");
                }

                treeptr tempcur2 = tempcur->lchild;
                if(tempcur2 == NULL)
                {
                    printf("rmdir : can't remove %s. no file or directory\n",svdata);
                    return;
                }
                else
                {
                    while(tempcur2!=NULL)
                    {
                        if(!strcmp(token,tempcur2->name) && tempcur2->type == 'd')
                        {
                            break;
                        }
                        tempcur2 = tempcur2->rsibling;
                    }

                    if(tempcur2==NULL)
                    {
                        printf("rmdir : can't remove %s. no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        tempcur = tempcur2;
                    }

                }
                temppath = token;
                token = strtok(NULL,"/");

                tempuser = tempcur2;

                deep = tempcur2; //절대경로의 deep은 user

                while(token!=NULL)
                {
                    temppath = token;
                    tempcur2 = tempcur->lchild;
                    if(tempcur2 == NULL)
                    {
                        printf("rmdir : can't remove %s. no file or directory\n",svdata);
                        return;
                    }
                    else
                    {
                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(token,tempcur2->name)&& tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2==NULL)
                        {
                            printf("rmdir : can't remove %s. no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                            if(tempcur2->lchild != NULL && tempcur2->lchild->rsibling != NULL)
                            {
                                printf("rmdir : fail to remove directory '%s' : directory is not empty\n",temppath);
                                return;
                            }
                            tempcur = tempcur2;
                        }

                    }
                    token = strtok(NULL,"/");
                }



            }
            else //상대 경로
            {
                token = strtok(temp->data,"/");
                tempcur = current;
                deep = tempcur;
                while(token!=NULL)
                {
                    if(!strcmp(token,"."))
                    {
                        if(tempcur->lchild!=NULL && tempcur->lchild->rsibling!=NULL)
                        {
                            printf("rmdir : fail to remove directory '%s' : directory is not empty\n",token);
                            return;
                        }
                        else
                        {
                            tempcur = tempcur;
                        }

                    }
                    else if(!strcmp(token,".."))
                    {
                        if(tempcur == root)
                        {
                            tempcur = tempcur;
                        }
                        else
                        {
                            tempcur = tempcur->previous;
                        }
                        if(tempcur->lchild!=NULL && tempcur->lchild->rsibling!=NULL)
                        {
                            printf("rmdir : fail to remove directory '%s' : directory is not empty\n",token);
                            return;
                        }
                        deep = tempcur;

                    }
                    else
                    {
                        treeptr tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("rmdir : can't remove %s. no file or directory\n",svdata);
                            return;
                        }

                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(tempcur2->name,token)&& tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2 == NULL)
                        {
                            printf("rmdir : can't remove %s. no file or directory\n",svdata);
                            return;
                        }
                        else
                        {
                                if(tempcur2->lchild == NULL)
                                {
                                    tempcur = tempcur2;
                                    break;
                                }
                                if(tempcur2->lchild != NULL & tempcur2->lchild->rsibling!=NULL)
                                {
                                    printf("rmdir : fail to remove directory '%s' : directory is not empty\n",token);
                                    return;
                                }
                                tempcur = tempcur2;

                        }



                    }

                    token = strtok(NULL,"/");
                }

            }

            if(deep == root)
            {
                printf("rmdir :can't remove user folder\n");
                return;
            }

            while(tempcur!=deep)
            {

                if(tempcur->rsibling == NULL)
                {
                    if(tempcur->lsibling == NULL) //x x
                    {
                        tempcur->previous->lchild = NULL;
                    }
                    else // o x
                    {
                        tempcur->lsibling->rsibling = NULL;
                    }
                }
                else
                {
                    if(tempcur->lsibling == NULL) // x o
                    {
                        tempcur->previous->lchild = tempcur->rsibling;
                        tempcur->rsibling->lsibling = NULL;
                    }
                    else // o o
                    {
                        tempcur->lsibling->rsibling = tempcur->rsibling;
                        tempcur->rsibling->lsibling = tempcur->lsibling;
                    }
                }
                treeptr tempcur2 = tempcur;
                tempcur = tempcur->previous;
                free(tempcur2);
            }


            current = deep;
}

void removedir()
{
    nodeptr pre = head;
    nodeptr cur = pre -> link;
    int option_p = 0;
    treeptr tempcur;
    pid_t pid[255];


    if(cur==NULL) //옵션 없고 경로 없을 때
    {
        printf("rmdir : wrong operator\n");
        return;
    }
    else
    {
        while(cur!=NULL)
        {
            if(!strcmp(cur->data,"-p"))
            {
                option_p = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    break;
                }
            }

            pre = pre->link;
            cur = pre->link;
        }
    }

    nodeptr temp = head->link;
    int k = 0;
    if(temp == NULL)
    {
        printf("rm : wrong operator\n");
        return;
    }
    while(temp!=NULL)
    {
                if(option_p)
                {
                    removedir_p(temp);
                }
                else
                {
                    removedir_n(temp);
                }


        k++;
        temp = temp->link;
    }




}

void preorder(treeptr ptr,treeptr cur,int option_a,int option_f,int option_p)
{
    if(ptr!= NULL)
    {

        if(option_a || ptr->name[0] != '.')
        {
            treeptr temptree = cur;
            treeptr tempcur = ptr;
            while(temptree != ptr)
            {
                tempcur = ptr;
                while(tempcur->previous!=temptree)
                {
                    tempcur = tempcur->previous;
                }
                if(temptree != cur)
                {
                    if(temptree->rsibling == NULL)
                    {
                        printf("    ");
                    }
                    else
                    {
                        if(!option_a)
                        {
                            if(temptree->rsibling->name[0] != '.')
                            {
                                printf("|   ");
                            }
                            else
                            {
                                temptree = temptree->rsibling;
                                while(temptree!=NULL)
                                {
                                    if(temptree->name[0] != '.')
                                    {
                                        break;
                                    }
                                    temptree = temptree->rsibling;
                                }
                                if(temptree == NULL)
                                {
                                    printf("    ");
                                }
                                else
                                {
                                    printf("|   ");
                                }
                            }

                        }
                        else
                        {
                             printf("|   ");
                        }

                    }
                }

                temptree = tempcur;
            }



            if(ptr->rsibling == NULL)
            {
                if(option_f)
                {
                    printf("|_  ");
                    if(option_p)
                    {
                        printf("[");
                        printf("%c",ptr->type);
                        dividePermission(ptr->permission[0]);
                        dividePermission(ptr->permission[1]);
                        dividePermission(ptr->permission[2]);
                        printf("] ");
                    }
                    pwd(ptr,cur);
                }
                else
                {
                    printf("|_  ");
                    if(option_p)
                    {
                        printf("[");
                        printf("%c",ptr->type);
                        dividePermission(ptr->permission[0]);
                        dividePermission(ptr->permission[1]);
                        dividePermission(ptr->permission[2]);
                        printf("] ");
                    }
                    printf("%s\n",ptr->name);
                }

            }
            else
            {
                if(ptr->rsibling->name[0]!='.')
                {
                    if(option_f)
                    {
                        printf("|_  ");
                        if(option_p)
                        {
                            printf("[");
                            printf("%c",ptr->type);
                            dividePermission(ptr->permission[0]);
                            dividePermission(ptr->permission[1]);
                            dividePermission(ptr->permission[2]);
                            printf("] ");
                        }
                        pwd(ptr,cur);
                    }
                    else
                    {
                        printf("|_  ");
                        if(option_p)
                        {
                            printf("[");
                            printf("%c",ptr->type);
                            dividePermission(ptr->permission[0]);
                            dividePermission(ptr->permission[1]);
                            dividePermission(ptr->permission[2]);
                            printf("] ");
                        }
                        printf("%s\n",ptr->name);


                    }

                }
                else
                {

                    if(!option_a)
                    {
                        temptree = ptr->rsibling;
                            while(temptree!=NULL)
                            {
                                if(temptree->name[0] != '.')
                                {
                                    break;
                                }
                                temptree = temptree->rsibling;
                            }
                            if(temptree == NULL)
                            {
                                if(option_f)
                                {
                                    printf("|_  ");
                                    if(option_p)
                                    {
                                        printf("[");
                                        printf("%c",ptr->type);
                                        dividePermission(ptr->permission[0]);
                                        dividePermission(ptr->permission[1]);
                                        dividePermission(ptr->permission[2]);
                                        printf("] ");
                                    }
                                    pwd(ptr,cur);
                                }
                                else
                                {
                                    printf("|_  ");
                                    if(option_p)
                                    {
                                        printf("[");
                                        printf("%c",ptr->type);
                                        dividePermission(ptr->permission[0]);
                                        dividePermission(ptr->permission[1]);
                                        dividePermission(ptr->permission[2]);
                                        printf("] ");
                                    }
                                    printf("%s\n",ptr->name);
                                }

                            }
                            else
                            {
                                if(option_f)
                                {
                                    printf("|_  ");
                                    if(option_p)
                                    {
                                        printf("[");
                                        printf("%c",ptr->type);
                                        dividePermission(ptr->permission[0]);
                                        dividePermission(ptr->permission[1]);
                                        dividePermission(ptr->permission[2]);
                                        printf("] ");
                                    }
                                    pwd(ptr,cur);
                                }
                                else
                                {
                                    printf("|_  ");
                                    if(option_p)
                                    {
                                        printf("[");
                                        printf("%c",ptr->type);
                                        dividePermission(ptr->permission[0]);
                                        dividePermission(ptr->permission[1]);
                                        dividePermission(ptr->permission[2]);
                                        printf("] ");
                                    }
                                    printf("%s\n",ptr->name);
                                }

                            }
                    }
                    else
                    {
                        if(option_f)
                        {
                            printf("|_  ");
                            if(option_p)
                            {
                                printf("[");
                                printf("%c",ptr->type);
                                dividePermission(ptr->permission[0]);
                                dividePermission(ptr->permission[1]);
                                dividePermission(ptr->permission[2]);
                                printf("] ");
                            }
                            pwd(ptr,cur);
                        }
                        else
                        {
                            printf("|_  ");
                            if(option_p)
                            {
                                printf("[");
                                printf("%c",ptr->type);
                                dividePermission(ptr->permission[0]);
                                dividePermission(ptr->permission[1]);
                                dividePermission(ptr->permission[2]);
                                printf("] ");
                            }
                            printf("%s\n",ptr->name);
                        }

                    }

                }

            }

        }

        if(option_a)
        {
            preorder(ptr->lchild,cur,option_a,option_f,option_p);
            preorder(ptr->rsibling,cur,option_a,option_f,option_p);
        }
        else
        {
            if(ptr->name[0]=='.')
            {
                preorder(ptr->rsibling,cur,option_a,option_f,option_p);
            }
            else
            {
                preorder(ptr->lchild,cur,option_a,option_f,option_p);
                preorder(ptr->rsibling,cur,option_a,option_f,option_p);
            }
        }

    }
}

void treefunction(nodeptr temp,int option_a,int option_f,int option_p)
{
    treeptr tempcur;
    if(temp == NULL)
    {
        tempcur = current;
    }
    else
    {
        char *token;
        char *temppath;
        char svdata[100];
        strcpy(svdata,temp->data);
        if(temp->data[0] == '/') //절대 경로
            {
                token = strtok(temp->data,"/");
                tempcur = root;
                treeptr tempuser = user;


                if(token == NULL) // path : /
                {
                    tempcur == root;
                }
                else
                {
                                        treeptr tempcur2 = tempcur->lchild;
                    if(tempcur2 == NULL)
                    {
                        printf("tree : no file or directory '%s'\n",svdata);
                            return;
                    }
                    else
                    {
                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(token,tempcur2->name))
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2==NULL)
                        {
                            printf("tree : no file or directory '%s'\n",svdata);
                            return;
                        }
                        else
                        {
                            tempcur = tempcur2;
                        }

                    }
                    temppath = token;
                    token = strtok(NULL,"/");

                    tempuser = tempcur2;


                    while(token!=NULL)
                    {
                        temppath = token;
                        tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("tree : no file or directory '%s'\n",svdata);
                            return;
                        }
                        else
                        {
                            while(tempcur2!=NULL)
                            {
                                if(!strcmp(token,tempcur2->name) && tempcur2->type == 'd')
                                {
                                    break;
                                }
                                tempcur2 = tempcur2->rsibling;
                            }

                            if(tempcur2==NULL)
                            {
                                printf("tree : no file or directory '%s'\n",svdata);
                            return;
                            }
                            else
                            {
                                tempcur = tempcur2;
                            }

                        }
                        token = strtok(NULL,"/");
                    }

                }




            }
            else //상대 경로
            {
                token = strtok(temp->data,"/");
                temppath = token;
                tempcur = current;
                treeptr tempuser = user;
                while(token!=NULL)
                {
                    temppath = token;
                    if(!strcmp(token,"."))
                    {

                    }
                    else if(!strcmp(token,".."))
                    {
                        if(tempcur == root)
                        {
                            tempcur = tempcur;
                        }
                        else
                        {
                            if(tempcur->previous == root)
                            {
                                tempuser = root;
                            }
                            tempcur = tempcur->previous;
                        }

                    }
                    else
                    {
                        treeptr tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("tree : no file or directory '%s'\n",svdata);
                            return;
                        }

                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(tempcur2->name,token) && tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2 == NULL)
                        {
                            printf("tree : no file or directory '%s'\n",svdata);
                            return;
                        }
                        else
                        {
                            if(tempcur == root)
                            {
                                tempuser = tempcur2;
                                tempcur = tempuser;
                            }
                            else
                            {
                                tempcur = tempcur2;
                                tempuser = tempuser;
                            }

                        }



                    }

                    token = strtok(NULL,"/");
                }


            }
    }

    printf("%s\n",tempcur->name);
    preorder(tempcur->lchild,tempcur,option_a,option_f,option_p);
    printf("\n");
}

void tre()
{
    nodeptr pre = head;
    nodeptr cur = pre -> link;
    int option_f = 0;
    int option_a = 0;
    int option_p = 0;
    pid_t pid[255];
    treeptr tempcur;

    if(cur==NULL) //옵션 없고 경로 없을 때
    {
        tempcur = current;
    }
    else
    {
        for(int i=0;i<3;i++)
        {
            pre = head;
            cur = pre -> link;
            while(cur!=NULL)
            {
                if(!strcmp(cur->data,"-f"))
                {
                    option_f = 1;
                    if(cur->link!=NULL)
                    {
                        pre->link = cur->link;
                        free(cur);
                        cur = pre->link;
                    }
                    else
                    {
                        pre->link = NULL;
                        free(cur);
                        break;
                    }
                }
                if(!strcmp(cur->data,"-a"))
                {
                    option_a = 1;
                    if(cur->link!=NULL)
                    {
                        pre->link = cur->link;
                        free(cur);
                        cur = pre->link;
                    }
                    else
                    {
                        pre->link = NULL;
                        free(cur);
                        break;
                    }
                }
                if(!strcmp(cur->data,"-p"))
                {
                    option_p = 1;
                    if(cur->link!=NULL)
                    {
                        pre->link = cur->link;
                        free(cur);
                        cur = pre->link;
                    }
                    else
                    {
                        pre->link = NULL;
                        free(cur);
                        break;
                    }
                }

                pre = pre->link;
                cur = pre->link;
            }

        }

    }

    nodeptr temp = head->link;



    int k = 0;
    if(temp == NULL)
    {
        nodeptr newnode = (nodeptr)malloc(sizeof(node));
        strcpy(newnode->data,".");
        newnode->link = NULL;

        temp = newnode;
    }

    while(temp!=NULL)
    {
                treefunction(temp,option_a,option_f,option_p);


        k++;

        temp = temp->link;
    }



}

void preorderfind_s(treeptr tempcur2,char *string,int option_t,char type)
{
    if(tempcur2!=NULL)
    {
        if(option_t)
        {
            if(type == 'd')
            {
                if(strstr(tempcur2->name,string) && tempcur2->type == 'd')
                {
                    pwd(tempcur2,root);
                }
            }
            else
            {
                if(strstr(tempcur2->name,string) && tempcur2->type == '-')
                {
                    pwd(tempcur2,root);
                }
            }
        }
        else
        {
            if(strstr(tempcur2->name,string))
            {
                pwd(tempcur2,root);
            }

        }
        preorderfind_s(tempcur2->lchild,string,option_t,type);
        preorderfind_s(tempcur2->rsibling,string,option_t,type);
    }
}

void preorderfind_n(treeptr tempcur2,char *string,int option_t,char type)
{
    if(tempcur2!=NULL)
    {
        if(option_t)
        {
            if(type == 'd')
            {
                if(!strcmp(tempcur2->name,string) && tempcur2->type == 'd')
                {
                    pwd(tempcur2,root);
                }
            }
            else
            {
                if(!strcmp(tempcur2->name,string) && tempcur2->type == '-')
                {
                    pwd(tempcur2,root);
                }
            }
        }
        else
        {
            if(!strcmp(tempcur2->name,string))
            {
                pwd(tempcur2,root);
            }
        }

        preorderfind_n(tempcur2->lchild,string,option_t,type);
        preorderfind_n(tempcur2->rsibling,string,option_t,type);
    }
}

void preorderfind_nn(treeptr tempcur,int option_t,char type)
{
    if(tempcur!=NULL)
    {
        if(option_t)
        {
            if(type == 'd')
            {
                if(tempcur->type == 'd')
                {
                    pwd(tempcur,root);
                }
            }
            else
            {
                if(tempcur->type == '-')
                {
                    pwd(tempcur,root);
                }
            }
        }
        else
        {
            pwd(tempcur,root);
        }

        preorderfind_nn(tempcur->lchild,option_t,type);
        preorderfind_nn(tempcur->rsibling,option_t,type);
    }
}

void findfunction(nodeptr temp,int option_n,int option_t,char *name,char type)
{
    treeptr tempcur;
    if(temp == NULL)
    {
        tempcur = current;
    }
    else
    {
        char *token;
        char *temppath;
        char svdata[100];
        strcpy(svdata,temp->data);
        if(temp->data[0] == '/') //절대 경로
            {
                token = strtok(temp->data,"/");
                tempcur = root;
                treeptr tempuser = user;


                if(token == NULL) // path : /
                {
                    tempcur == root;
                }
                else
                {
                    treeptr tempcur2 = tempcur->lchild;
                    if(tempcur2 == NULL)
                    {
                        printf("find : no file or directory '%s'\n",svdata);
                            return;
                    }
                    else
                    {
                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(token,tempcur2->name))
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2==NULL)
                        {
                            printf("find : no file or directory '%s'\n",svdata);
                            return;
                        }
                        else
                        {
                            tempcur = tempcur2;
                        }

                    }
                    temppath = token;
                    token = strtok(NULL,"/");

                    tempuser = tempcur2;


                    while(token!=NULL)
                    {
                        temppath = token;
                        tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("find : no file or directory '%s'\n",svdata);
                            return;
                        }
                        else
                        {
                            while(tempcur2!=NULL)
                            {
                                if(!strcmp(token,tempcur2->name) && tempcur2->type == 'd')
                                {
                                    break;
                                }
                                tempcur2 = tempcur2->rsibling;
                            }

                            if(tempcur2==NULL)
                            {
                                printf("find : no file or directory '%s'\n",svdata);
                            return;
                            }
                            else
                            {
                                tempcur = tempcur2;
                            }

                        }
                        token = strtok(NULL,"/");
                    }

                }




            }
            else //상대 경로
            {
                token = strtok(temp->data,"/");
                temppath = token;
                tempcur = current;
                treeptr tempuser = user;
                while(token!=NULL)
                {
                    temppath = token;
                    if(!strcmp(token,"."))
                    {

                    }
                    else if(!strcmp(token,".."))
                    {
                        if(tempcur == root)
                        {
                            tempcur = tempcur;
                        }
                        else
                        {
                            if(tempcur->previous == root)
                            {
                                tempuser = root;
                            }
                            tempcur = tempcur->previous;
                        }

                    }
                    else
                    {
                        treeptr tempcur2 = tempcur->lchild;
                        if(tempcur2 == NULL)
                        {
                            printf("find : no file or directory '%s'\n",svdata);
                            return;
                        }

                        while(tempcur2!=NULL)
                        {
                            if(!strcmp(tempcur2->name,token) && tempcur2->type == 'd')
                            {
                                break;
                            }
                            tempcur2 = tempcur2->rsibling;
                        }

                        if(tempcur2 == NULL)
                        {
                            printf("find : no file or directory '%s'\n",svdata);
                            return;
                        }
                        else
                        {
                            if(tempcur == root)
                            {
                                tempuser = tempcur2;
                                tempcur = tempuser;
                            }
                            else
                            {
                                tempcur = tempcur2;
                                tempuser = tempuser;
                            }

                        }



                    }

                    token = strtok(NULL,"/");
                }


            }
    }


    if(option_n)
    {

        char optionname[25];
        memset(optionname,0,sizeof(optionname));
        if(name[0] == '"' && name[1] == '*')
        {
            int i = 2;
            while(name[i]!='*' || name[i+1]!='"')
            {
                optionname[i-2] = name[i];
                i++;
                if(i>25)
                {
                    printf("find : input exact name\n");
                    return;
                }
            }
            optionname[i] = '\0';

            treeptr tempcur2 = tempcur->lchild;
            if(tempcur2 == NULL)
            {

            }
            else
            {
                preorderfind_s(tempcur2,optionname,option_t,type);
            }

        }
        else if(name[0] == '"' && name[1] != '*')
        {
            int i = 1;
            while(name[i]!='"')
            {
                optionname[i-1] = name[i];
                i++;
                if(i>25)
                {
                    printf("find : input exact name\n");
                    return;
                }
            }
            optionname[i] = '\0';

            treeptr tempcur2 = tempcur->lchild;
            if(tempcur2 == NULL)
            {

            }
            else
            {
                preorderfind_n(tempcur2,optionname,option_t,type);
            }

        }
        else
        {
            treeptr tempcur2 = tempcur->lchild;
            if(tempcur2 == NULL)
            {

            }
            else
            {
                preorderfind_n(tempcur2,name,option_t,type);
            }
        }
    }
    else
    {
        printf("%s :\n",tempcur->name);
        preorderfind_nn(tempcur->lchild,option_t,type);
        printf("\n");
    }
}


void find()
{
    nodeptr pre = head;
    nodeptr cur = pre -> link;
    int option_n = 0;
    int option_t = 0;
    char name[20];
    char type;
    treeptr tempcur;

    if(cur==NULL) //옵션 없고 경로 없을 때
    {
        tempcur = current;
    }
    else
    {
        while(cur!=NULL)
        {
            if(!strcmp(cur->data,"-name"))
            {
                option_n = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                    strcpy(name,cur->data);

                    if(cur->link!=NULL)
                    {
                        pre->link = cur->link;
                        free(cur);
                        cur = pre->link;
                    }
                    else
                    {
                        pre->link = NULL;
                        free(cur);
                        break;
                    }
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    printf("find : need more factor\n");
                    return;
                }
            }
            if(!strcmp(cur->data,"-type"))
            {
                option_t = 1;
                if(cur->link!=NULL)
                {
                    pre->link = cur->link;
                    free(cur);
                    cur = pre->link;
                    if(!strcmp(cur->data,"d"))
                    {
                        type = cur->data[0];
                    }
                    else if(!strcmp(cur->data,"f"))
                    {
                        type = cur->data[0];
                    }
                    else
                    {
                        printf("find : wrong type\n");
                        return;
                    }


                    if(cur->link!=NULL)
                    {
                        pre->link = cur->link;
                        free(cur);
                        cur = pre->link;
                    }
                    else
                    {
                        pre->link = NULL;
                        free(cur);
                        break;
                    }
                }
                else
                {
                    pre->link = NULL;
                    free(cur);
                    printf("find : need more factor\n");
                    return;
                }
            }

            pre = pre->link;
            cur = pre->link;
        }
    }

    pid_t pid[255];

    nodeptr temp = head->link;
    int k = 0;
    if(temp == NULL)
    {
        nodeptr newnode = (nodeptr)malloc(sizeof(node));
        strcpy(newnode->data,".");
        newnode->link = NULL;

        temp = newnode;
    }
    while(temp!=NULL)
    {

                findfunction(temp,option_n,option_t,name,type);



        k++;
        temp = temp->link;
    }

}

