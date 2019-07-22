#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


struct page
{
  int page_no;
  int wc;
  char url[150];
  struct page * word;
};


struct word
{
  char wordc[100];
  int word_key;
  struct page * first;
  struct word * next;
  struct word * prev;
};


struct hash
{
  struct word * first;
  struct word * last;
};


struct hash h[50];
struct word * headw=NULL;
struct page * headp=NULL;
char page_url[150]={0};
int page = 0;

/* Print list */
void print_list()
{
  struct word *temp = NULL;
  temp = headw;

  while(temp!=NULL)
  {

    printf("%s comes ",temp->wordc);

    struct page *temp1 = temp->first;
    
    while(temp1!=NULL)
    {
      printf(" %d times",temp1->wc);
      temp1=temp1->word;
    }
    printf("\n*******\n");
    temp=temp->next;
  }
}

/* Print the list */
void print()
{
  struct word *templ;
  templ=headw;
  while(templ != NULL)
  {
    printf("%s", templ->wordc);
    printf(" %d",templ->word_key);
    templ = templ->next;
  }
}

/* Generate key for hash for all words */
int generate_key(char *word)
{
  int key=0;
  for(int i=0;word[i]!='\0';i++)
  {
    key=key+word[i];
  }
  key=key%50;
  return key;
}

/* Check Duplicate in result */ 
struct word* check_duplicate(struct word *array)
{
  struct word * temp = headw;
  while(temp!=NULL)
  {
    if(strcmp(temp->wordc,array->wordc) == 0)
    {
      return temp;
    }
    temp=temp->next;
  }
  return NULL;
}

/* Add the extracted words from function add_word to linked list */
void insert_list (char *wrd)
{

      struct word *temp=NULL;
      temp = (struct word*)calloc(1,sizeof(struct word));
      strcpy(temp->wordc,wrd);
      int index = generate_key(wrd);
      temp->word_key = index;
      temp->next=NULL;
      temp->first = NULL;
      if(headw==NULL)
      {
            headw = temp;
            h[index].first = temp;
            h[index].last = temp;

            struct page *ptr=NULL;
            ptr = (struct page*)calloc(1,sizeof(struct page));
            ptr->page_no = page;
            ptr->wc = 1;
            strcpy(ptr->url,page_url);
            ptr->word=NULL;
            headw->first = ptr;
      }
      else
      {
        struct word *link = check_duplicate(temp);
          if(link==NULL)
          {
                    struct page *ptr=NULL;
                    ptr = (struct page*)calloc(1,sizeof(struct page));
                    ptr->page_no = page;
                    ptr->wc = 1;
                    strcpy(ptr->url,page_url);
                    ptr->word=NULL;

                    temp->first = ptr;

                      if(h[index].first==NULL)
                      {
                              h[index].first = temp;
                              h[index].last  = temp;
                              struct word *p = NULL;
                              p=headw;
                              while(p->next!=NULL)
                              p=p->next;
                              p->next = temp;
                      }
                      else
                      {

                          //this is to insert the node at the for the same key
                              struct word *k=NULL;
                              k=h[index].last;
                              if(k->next!=NULL)
                              {
                                  temp->next=k->next;
                                  k->next = temp;
                              }
                              else
                              {
                                temp->next=NULL;
                                k->next=temp;
                              }
                              h[index].last = temp;



                      }
          }
          else
          {
            //if word exist then increment the count of the word

                    struct page *head=NULL;
                    head = link->first;
                    struct page *temp2 = head;

                    int flag = 0;//to check that if that pages node exist or not

                    while(temp2!=NULL)
                    {
                      if(temp2->page_no == page)
                      {
                        temp2->wc +=1;
                        flag=1;
                        break;
                      }
                      temp2=temp2->word;
                    }
        // if p2 donot exist then create and link it
                    if(flag==0)
                    {

                      struct page *ptr=NULL;
                      ptr = (struct page*)calloc(1,sizeof(struct page));
                      ptr->page_no = page;
                      ptr->wc = 1;
                      strcpy(ptr->url,page_url);
                      ptr->word=NULL;
                      temp2 = head;
                      while(temp2->word!=NULL)
                      temp2=temp2->word;
                      temp2->word = ptr;

                    }
                      free(temp);
          }
      }
}    

/* Extract words from the determined start and end point from the prevoius function (extract_words) */
void extract_word(char *start,char *end)
{
    int i=0,j=0;

    char *temp = start;
    int difference = end-start;
    
    char *string = (char*)calloc(1,difference);
    int k=0;
    
    for(temp;temp!=end;temp++)
    	string[k++]=*temp;

    char *word = (char*)calloc(1,30);

    
    for(i=0;i<strlen(string);i++)
    {
          if(string[i]==']'||string[i]=='['||string[i]=='!'||string[i]==' '||string[i]==','||string[i]==':'||string[i]=='-'||string[i]=='.'||string[i]==')'||string[i]=='('||string[i]=='&'||string[i]==';'||string[i]=='-')
          {
            if(((word[0]>='A')&&(word[0]<='Z'))||((word[0]>='a'&&word[0]<='z')&&(strlen(word)>2)))
            {
                word[j]='\0';
                
	        int length = strlen(word);

                if(length>=2)
                {
                  insert_list(word);
                }
            }
            j=0;
          }
          else
          {
            word[j++]=string[i];
          }
    }
    if(j!=0)
    {
      if((word[0]>='A'&&word[0]<='Z')||(word[0]>='a'&&word[0]<='z'))
      {
          word[j]='\0';
          if(strlen(word)>2)
            {
              insert_list(word);
            }
      }
    }

}

/* Extract words from title tag */
void title_search(char *html,int size)
{
    char *start,*end;
    for(int i=0;i<size-1;i++)
    {
      if(html[i]=='<'&&html[i+1]=='t')
      {
        start = &html[i];
        start = strpbrk(start,">");
        start++;
        break;
      }
    }
    end = strpbrk(start,"<");
    extract_word(start,end);
}

/* Extract words from the heading tags */
void head_search(char *html,int size)
{
  int m =0;
  char *start,*end;
  for(int i=0;i<size;i++)
  {
    if(html[i]=='<'&&html[i+1]=='h'&&html[i+2]>='1'&&html[i+2]<='6')
    {
      start = &html[i];
      start = strpbrk(start,">");
      start++;
      end = strpbrk(start,"<");
      extract_word(start,end);
    }
  }
}

/* Search tags in html file */
void search_tag(char *html,int size)
{
  int i=0,j=0;
  for(int i=0;i<size;i++)
  {
    if(html[i]=='\n')
    {
      break;
    }
    else
    {
      page_url[j++]=html[i];
    }
  }
  page_url[j]='\0';
  title_search(html,size);
  head_search(html,size);
}

int main()
{
  int c=1;
  char name[10]={0};
  FILE *file=NULL;
  while(1)
  {
    char file_name[10]={0};
    char path_name[100]={0};
    struct stat st;
    stat(file_name,&st);
    sprintf(file_name,"%d",c);
    strcat(file_name,".txt");
    strcat(path_name,"/home/ubuntu/search_engine/");
    strcat(path_name,file_name);
    file=fopen(path_name,"r");
    if(file)
    {
      struct stat st;
      stat(path_name,&st);
      int file_size=st.st_size;
      char *html=(char*) calloc (1,file_size);
      char ch;
      int i=0;
      while((ch=fgetc(file))!=EOF)
      { 
	html[i++]=ch;
      }
      html[i]='\0';
      search_tag(html,file_size);
    }
    else
    {
      printf("Finish");
      break;
    } 
    c++;
  }
print_list();
}
