#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define Base_Url "www.chitkara.edu.in"
struct node
{
  char Node_Url[200];
  int Node_depth;
  int Node_flag;
  struct node * next;
  struct node * prev;
  int Node_key;
};
struct hash
{
  struct node *first;
  struct node *last;
};
struct hash h[50];
struct node * head=NULL;
/* Check directory function */
void check_dir(char *dir)
{
  struct stat statbuf;
  if ( stat(dir, &statbuf) == -1 ) 
  {
    printf("-----------------\n");
    printf("Invalid directory\n");
    printf("-----------------\n");
    exit(1);
  }
  //Both check if there's a directory and if it's writable
  if ( !S_ISDIR(statbuf.st_mode) ) 
  {
    printf("-----------------------------------------------------\n");
    printf("Invalid directory entry. Your input isn't a directory\n");
    printf("-----------------------------------------------------\n");
    exit(1);
  }
  if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR ) 
  {
    printf("------------------------------------------\n");
    printf("Invalid directory entry. It isn't writable\n");
    printf("------------------------------------------\n");
    exit(1);
  }
}
/* Check Url is valid or not function */
int check_url( char url[])
{
  char url_temp[strlen(url)+20];
  strcpy(url_temp,"wget --spider ");
  strcat(url_temp, url);
  if(!system(url_temp)) // system return 0 for valid url 
  {
    char seed_url[strlen(url)+1];
    int i;
    for(i=0;url[i]!='/';i++)
    {
       seed_url[i]=url[i];
    }
    seed_url[i]=='\0';
    if(strcmp(Base_Url,seed_url)!=0)
    {
       printf("Base_Url is not same as seed_url");
       return 0;
    }
    else
       return 1;
  }
  else
  {
    printf("Enter a valid url");
    return 0;
  }
}
/*  Check depth is in range or not */
int check_depth(char depth[])
{
  int flag=0;
  for(int i=0;i<strlen(depth);i++)
  {
    if((depth[i]>=48)&&(depth[i]<=57))
    {
      continue;
    }
    else
    {
          flag=1;
          break;
    }
  }
  if(flag==1)
  {
    printf("Enter Depth In Numericals Only");
    return 0;   
  }  
  else
  {
    int depth_int=atoi(depth);
    if((depth_int>=0)&&(depth_int<=5))
    {}  
    else 
    {
       printf("Enter Depth In Range Only");
       return 0;   
    }    
  }
}
void null_func()
{
  int i=0;
  for(int i=0;i<50;i++)
  {
    h[i].first=NULL;
    h[i].last=NULL;
  }
}
/* Remove white space from html file retrieved and then return the file without spaces */
void removeWhiteSpace(char* html)
{
  int i;
  char *buffer = malloc(strlen(html)+1), *p=malloc (sizeof(char)+1);
  memset(buffer,0,strlen(html)+1);
  for (i=0;html[i];i++)
  {
    if(html[i]>32)
    {
      sprintf(p,"%c",html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(html,buffer);
  free(buffer); free(p);
}
int GetNextURL(char* html, char* urlofthispage, char* result, int pos)
{
  char c;
  int len, i, j;
  char* p1;  //!< pointer pointed to the start of a new-founded URL.
  char* p2;  //!< pointer pointed to the end of a new-founded URL.


  // NEW
  // Clean up \n chars
  if(pos == 0) {
    removeWhiteSpace(html);
  }
  // /NEW

  // Find the <a> <A> HTML tag.
  while (0 != (c = html[pos]))
  {
    if ((c=='<') &&
        ((html[pos+1] == 'a') || (html[pos+1] == 'A'))) {
      break;
    }
    pos++;
  }
  //! Find the URL it the HTML tag. They usually look like <a href="www.abc.com">
  //! We try to find the quote mark in order to find the URL inside the quote mark.
  if (c)
  {
    // check for equals first... some HTML tags don't have quotes...or use single quotes instead
    p1 = strchr(&(html[pos+1]), '=');

    if ((!p1) || (*(p1-1) == 'e') || ((p1 - html - pos) > 10))
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*(p1+1) == '\"' || *(p1+1) == '\'')
      p1++;

    p1++;

    p2 = strpbrk(p1, "\'\">");
    if (!p2)
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*p1 == '#')
    { // Why bother returning anything here....recursively keep going...

      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (!strncmp(p1, "mailto:",7))
      return GetNextURL(html, urlofthispage, result, pos+1);
    if (!strncmp(p1, "http", 4) || !strncmp(p1, "HTTP", 4))
    {
      //! Nice! The URL we found is in absolute path.
      strncpy(result, p1, (p2-p1));
      return  (int)(p2 - html + 1);
    } else {
      //! We find a URL. HTML is a terrible standard. So there are many ways to present a URL.
      if (p1[0] == '.') {
        //! Some URLs are like <a href="../../../a.txt"> I cannot handle this.
	// again...probably good to recursively keep going..
	// NEW

        return GetNextURL(html,urlofthispage,result,pos+1);
	// /NEW
      }
      if (p1[0] == '/') {
        //! this means the URL is the absolute path
        for (i = 7; i < strlen(urlofthispage); i++)
          if (urlofthispage[i] == '/')
            break;
        strcpy(result, urlofthispage);
        result[i] = 0;
        strncat(result, p1, (p2 - p1));
        return (int)(p2 - html + 1);
      } else {
        //! the URL is a absolute path.
        len = strlen(urlofthispage);
        for (i = (len - 1); i >= 0; i--)
          if (urlofthispage[i] == '/')
            break;
        for (j = (len - 1); j >= 0; j--)
          if (urlofthispage[j] == '.')
              break;
        if (i == (len -1)) {
          //! urlofthis page is like http://www.abc.com/
            strcpy(result, urlofthispage);
            result[i + 1] = 0;
            strncat(result, p1, p2 - p1);
            return (int)(p2 - html + 1);
        }
        if ((i <= 6)||(i > j)) {
          //! urlofthis page is like http://www.abc.com/~xyz
          //! or http://www.abc.com
          strcpy(result, urlofthispage);
          result[len] = '/';
          strncat(result, p1, p2 - p1);
          return (int)(p2 - html + 1);
        }
        strcpy(result, urlofthispage);
        result[i + 1] = 0;
        strncat(result, p1, p2 - p1);
        return (int)(p2 - html + 1);
      }
    }
  }
  return -1;
}
void print_list()
{
  struct node *templ=head;
  while(templ != NULL)
  {
    printf("%s", templ->Node_Url);
    printf(" %d",templ->Node_depth);
    //printf(" Flag  %c \n",templ->Node_flag);
    printf(" %d \n",templ->Node_key);
    templ = templ->next;
  }
}
void new_linked_list(char *line)
{
  char old_url[150]={0};
  int old_depth;
  int old_flag;
  int index;
  char temp[150]={0};
  int count=1,k=0;
  int i=0;
  while(line[i]!='\0')
  {
    if(line[i]==' ')
    {
        temp[k]='\0';
        if(count==1)
        {
          index=atoi(temp);
          //printf("%d\n",index);
          k=0;
          count=2;
        }
        else if (count==2)
        {
          old_depth=atoi(temp);
          k=0;
          count=3;
        }
        else if(count==3)
        {
          strcpy(old_url,temp);
          count=4;
          k=0;
        } 
        else if(count==4)
        {
          old_flag=atoi(temp);
          k=0;
          count=1;
        }
        k=0;
    }
    else
    {
        temp[k++]=line[i];
    }
    i++;
  }
  //printf("%d  %d  %d\n",old_depth,old_flag,index);
   
  
  struct node * ptr = NULL;
  ptr = (struct node *)calloc(1,sizeof(struct node));
  strcpy(ptr->Node_Url,old_url);
  ptr->Node_depth = old_depth;
  ptr->Node_flag=old_flag;
  ptr->next = NULL;
  ptr->prev = NULL;
  ptr->Node_key=index;
  if(head==NULL)
  {
    head=ptr;
    h[index].first=ptr;
    h[index].last=ptr;
  }
  else 
  {
    if(h[index].first==NULL)
    {
      h[index].first=ptr;
      h[index].last=ptr;
      struct node *temp=head;
      while (temp->next != NULL)  // Find the last node
		    temp = temp->next;
		  temp->next=ptr;
		  ptr->prev=temp;
    }
    else 
    {
      struct node *templast = NULL;
      templast=h[index].last;
      struct node *tempnext = NULL;
      tempnext=templast->next;
      if(tempnext!=NULL)
      {
        h[index].last=ptr;
        templast->next=ptr;
        ptr->prev=templast;
        ptr->next=tempnext;
        tempnext->prev=ptr;
      }
      else
      {
        h[index].last=ptr;
        templast->next=ptr;
        ptr->prev=templast;
        ptr->next=NULL;
      }
    }
  }
  /*struct node *templ=head;
  while(templ != NULL)
  {
    printf("%s", templ->Node_Url);
    printf(" %d",templ->Node_depth);
    //printf(" Flag  %c \n",templ->Node_flag);
    printf(" %d \n",templ->Node_key);
    templ = templ->next;
  }*/
  /*for(int i=0;i<50;i++)
  {
   if(h[i].first!=NULL)
   {
    struct node *templ=h[i].first;
    while(templ != h[i].last)
    {
      printf("%s", templ->Node_Url);
      //printf(" %d",templ->Node_depth);
      //printf(" Flag  %c \n",templ->Node_flag);
      printf(" %d \n",templ->Node_key);
      templ = templ->next;
    }
    }
  }*/
  
}
/* copy the previous data to new linked list */
void read_storage()
{
  head=NULL;
  FILE * store = fopen("/home/ubuntu/search_engine/storage.txt","r");
  char ch;
  char * line = (char *) calloc (200,sizeof(char));
  int i=0;
  while((ch=fgetc(store))!=EOF)
  {
    if(ch!='\n')
    {
      line[i]=ch;
      i++;
      //printf(",,");
    }
    else
    {
      line[i]='\0';
      //printf("%s\n",line);
      new_linked_list(line);
      i=0;
    }
  }
  print_list();
}
/* Print the array containing list */
void print(char **result)
{
	int i=0;
	for(i=0;i<100;i++)
	{
		printf("%s\n",result[i]);
	}
}
/* method to calculate the index of the hash array known as hash fucntion */
int hash_func (char s[])
{
  int sum=0;
  for(int i=0;s[i]!='\0';i++)
  {
    sum=sum+s[i];
  }
  sum=sum%50;
  return sum;
}
/* Enter the link list in file */
void store_file(struct node *head)
{
  FILE * storage = fopen("/home/ubuntu/search_engine/storage.txt","w");
  struct node * temp = head;
  while(temp!=NULL)
  { 
    fprintf(storage,"%d ",temp->Node_key);
    fprintf(storage,"%d ",temp->Node_depth);
    fprintf(storage,"%s ",temp->Node_Url);
    fprintf(storage,"%d \n",temp->Node_flag);
    temp=temp->next;
  }
}
/* Insert the links of array in linked list with hash table and return the node head */
struct node* insert_link(char **result,int depth)
{
  int index=0;
  struct node *ptr = NULL;
  for(int i=0;i<100;i++)
  {
    ptr = (struct node *)calloc(1,sizeof(struct node));
    strcpy(ptr->Node_Url,result[i]);
    ptr->Node_depth = depth;
    ptr->Node_flag=0;
    ptr->next = NULL;
    ptr->prev = NULL;
    index=hash_func(result[i]);
    ptr->Node_key=index;
    if(head==NULL)
    {
      head=ptr;
      h[index].first=ptr;
      h[index].last=ptr;
    }
    else 
    {
      if(h[index].first==NULL)
      {
        h[index].first=ptr;
        h[index].last=ptr;
        struct node *temp=head;
        while (temp->next != NULL)  // Find the last node
		       temp = temp->next;
		    temp->next=ptr;
		    ptr->prev=temp;
      }
      else 
      {
        struct node *templast = NULL;
        templast=h[index].last;
        struct node *tempnext = NULL;
        tempnext=templast->next;
        if(tempnext!=NULL)
        {
          h[index].last=ptr;
          templast->next=ptr;
          ptr->prev=templast;
          ptr->next=tempnext;
          tempnext->prev=ptr;
        }
        else
        {
          h[index].last=ptr;
          templast->next=ptr;
          ptr->prev=templast;
          ptr->next=NULL;
        }
      }
    }
  }
  /*struct node *temp=head;
  while(temp != NULL)
  {
    printf(" Url : %s", temp->Node_Url);
    printf(" Depth : %d",temp->Node_depth);
    //printf(" Flag : %c \n",temp->Node_flag);
    printf(" Key : %d \n",temp->Node_key);
    temp = temp->next;
  }*/
  store_file(head);
  return head;
}
/* Insert the links of array result in linked list and not in hash table */
/*void insert_link (char **result,int depth)   
{
  struct node *head=NULL;
  char *results;
  results=(char*)calloc(200,sizeof(char));
  for (int i=0;i<100;i++)
  {
	  struct node* ptr = (struct node*) calloc(1,sizeof(struct node));
		struct node *last = head;
    results=result[i];
		strcpy(ptr->Node_Url,results);     //  Insert data in new node
		ptr->Node_depth=depth;
		ptr->Node_flag='F';
		ptr->next = NULL;   // link new node to NULL as it is last node
		if (head == NULL)  // if list is empty add in beginning.
		{
		  head = ptr;
		  ptr->prev=NULL;
		  continue;
		}
		while (last->next != NULL)  // Find the last node
		  last = last->next;
		last->next = ptr;  // Add the node after the last node of list
		ptr->prev=last;
	}
  struct node *temp=head;
  while(temp != NULL)
  {
    printf(" Url : %s", temp->Node_Url);
    printf(" Depth : %d",temp->Node_depth);
    printf(" Flag : %c \n",temp->Node_flag);
    temp = temp->next;
  }
}*/
/* Checking duplicacy and collect 100 urls */
/*void collect_next_url(char *read_file)
{
	int i,p=0;
	char **result=NULL;
	result=(char**)calloc(100,sizeof(char*));
	char *temp;
	int flag=0;
	for(i=0;i<100;i++)
	{
	  temp=(char*)calloc(100,sizeof(char));
		result[i] = (char*)calloc(100,sizeof(char));
		p=GetNextURL(read_file,Base_Url,result[i],p);
		int flag;
		flag=0;
		for(int k=i-1;k>=0;k--)
		{
			if(strcmp(temp,result[k])==0)
			{
				flag=1;
				break;
			}
		}
		if(flag==0)
		{
			strcpy(result[i],temp);
			i++;
		}
		else
		{
		  free(temp);
		}
		/*p=GetNextURL(read_file,Base_Url,temp,p);
		flag=0;
		for(int k=0;k<i;k++)
		{
		   if(strcmp(temp,result[k])==0)
		   {
		     flag=1;
		     break;
		   }
		}
		if(flag==0)
		{
		   result[i] = (char*)malloc(sizeof(char)*200);
		   strcpy(result[i],temp);
		   i++;
		}*/
	/*}
	print(result);
  insert_link(result,1);
}*/
/* Checking duplicacy and collect 100 urls and return a array of url*/
void collect_next_url(char *read_file,int file_size)//duplicate
{
	int i,p=0,k;
	char **result=NULL;
	result=(char**)calloc(100,sizeof(char*));
	//	char temp[150];
	char *temp;
	for(i=0;i<100&&p<file_size;)
	{
		temp=(char*)calloc(150,sizeof(char));
		p=GetNextURL(read_file,Base_Url,temp,p);
		int flag;
		flag=0;
		for(k=i-1;k>=0;k--)
		{
			if(strcmp(temp,result[k])==0)
			{
				flag=1;
				break;
			}
		}
		if(flag==0)
		{
			//result[i] = (char*)malloc(sizeof(char)*120);
			result[i]=temp;
			i++;
		}
		else
		free(temp);

	}
  //print(result);
  head=insert_link(result,1);
  //print_list(head);
}

void getpage(char *url,char *dir)
{
  char urlbuffer[400]={0};
  strcat(urlbuffer,"wget -O ");
  strcat(urlbuffer,dir);
  strcat(urlbuffer,"/temp.txt ");
  strcat(urlbuffer,url);
  system(urlbuffer);
}
void extract_file(char *filename)
{
  struct stat st;
  stat(filename,&st);
  int file_size=st.st_size;
  //printf("%d",file_size);
  char *read_file=(char*) calloc (1,file_size);
  FILE *new2=fopen(filename,"r");
  char ch;
  int i=0;
  while((ch=fgetc(new2))!=EOF)
  { 
    read_file[i++]=ch;
  }
  read_file[i]='\0';
  //printf("%s",read_file);
  collect_next_url(read_file,file_size);
}
void create_permanent_file(char *depth)
{
  static int index=1;
  char base_url[20];
  strcpy(base_url,Base_Url);
  char indexarr[2];
  indexarr[0]=index+48;
  indexarr[1]='\0';
  index++;
  FILE *temp=fopen("/home/ubuntu/search_engine/temp.txt","r");
  char filename[100]="/home/ubuntu/search_engine/";
  strcat(filename,indexarr);
  strcat(filename,".txt");
  FILE *new =fopen(filename,"w");
  fprintf(new,"%s\n",base_url);
  fprintf(new,"%s\n",depth);
  char p;
  while((p=fgetc(temp))!=EOF)
  {
	putc(p,new);
  }
  fclose(temp);
  extract_file(filename);
}

int main(int argc,char * argv[])
{
  if(argc!=4)
  {
     printf("ee");
  }
  else
  { 
     int result_check_url=check_url(argv[1]);
     if(result_check_url==1)
     { 
           int result_check_depth=check_depth(argv[2]);
           check_dir(argv[3]);
     }
     // printf("..");
     char ans; 
     printf("Want to retrieve previous data ? (Y/N):");
     scanf("%c",&ans);
     if(ans=='N')
     {
       getpage(argv[1],argv[3]);
       create_permanent_file(argv[2]);
     }
     else if(ans=='Y')
     { 
       null_func();
       head=NULL;
       read_storage(head);
     }
  }
}
