#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef uint 
   typedef unsigned int uint;
#endif

typedef struct information{
   char* var;
   char* wert;
   struct information *next;
}info;

info *anfang;

//Funktionen Definitionen
void print_head(void);
void print_header(char*);
void print_end();
int getRMethod(void);
char* getPString(void);
char* getStrings(int);
char convHexToDez(char*);
void filterString(char*);
info* getStructure(char*);
int setStruct(char*, char*);
int getStruct(info*);
void free_info(info*);

// gibt Header von Dokumente
void print_head() {
   printf("Content-Type: text/html\n\n");
}

// gibt Header von HTML und beginn HTML
void print_header(char *title) {
   printf("<!DOCTYPE html>\n");
   printf("<html>\n\t<head>\n\t\t<title>%s</title>\n\t</head>",title);
   printf("\n\t<body>\n\n");
}

//gibt Ending-tag von body und HTML
void print_end() {
   printf("\n\t</body>\n</html>\n");
}

//gibt Request-Method aus : 0 = GET 1 = POST -1 = Fehler
int getRMethod() {
   char *request;
   if((request = getenv("REQUEST_METHOD")) != NULL) {
      if(strcmp(request, "GET") == 0) return 0;
      else if(strcmp(request,"POST") == 0) return 1;
      else return -1;
   } else {
      return -1;
   }
}

//gibt Post Variablen-Wert Paare als String aus.
char* getPString() {
   long len;
   char *str;
   if((str = getenv("CONTENT_LENGTH")) != NULL)
   {
      len = atoi(str);
   } else {
      return NULL;
   }
   if(len < 1) {
      return NULL;
   }
   str = (char *) malloc(len+1);
   if(str != NULL) {      
      fgets(str,len+1,stdin);
      str[len] = '\0';
   }
   return str;
}

//gibt POST und GET Variables-Werten Paare als String aus.
char *getStrings(int mode) {
   char *str;
   char *getStr;
   switch(mode) {
      case 1 :
         str = getPString();
         break;
      case 0 :
         if((getStr = getenv("QUERY_STRING")) != NULL) {
            str = (char*) malloc(strlen(getStr+1));
            if(str != NULL) {
               strcpy(str, getStr);
               str[strlen(str)] = '\0';
            }
         }
         break;
      default:
         str = NULL;
   }
   return str;
}

//konvertiert Hexadezimal String nach Dezimal Char
char convHexToDez(char *hex) {
   char ascii;
   ascii =  (hex[0] >= 'A' ? ((hex[0] & 0xdf) - 'A')+10 : (hex[0] - '0'));

   ascii <<= 4; /* Bitverschiebung für ascii*=16 */
   ascii += (hex[1] >= 'A' ? ((hex[1] & 0xdf) - 'A')+10 : (hex[1] - '0'));
   return ascii;
}

// verschönt der String, ersetz + und Hexacode mit Leerzeichen und Sonderzeichen
void filterString(char* str) {
   uint i = 0, j=0;
   while(str[i] != '\0') {
      if(str[i] == '%'){
         str[j] = convHexToDez(&str[i+1]);
         i += 2;
      } else if(str[i] == '+'){
         str[j] = 32;
      }
      else {
         str[j] = str[i];
      }
      i++;
      j++;
   }
   str[j] = '\0';
}

// verweise den Werten in zu Struct members.
int setStruct(char* bez, char* wert) {
   info *help;
   help = (info*) malloc(sizeof(info));
   if(help == NULL) {
      -1;
   }
   if((help->var = (char*) malloc(strlen(bez)+1)) == NULL || (help->wert = (char*) malloc(strlen(wert)+1)) == NULL) {
      return -1;
   }
   strncpy(help->var, bez, strlen(bez)+1);
   strncpy(help->wert, wert, strlen(wert)+1);
   help->next = NULL;

   if(anfang == NULL) {
      anfang = help;
   } else {
      info *zeiger;
      zeiger = anfang;
      while(zeiger->next != NULL) {
         zeiger = zeiger->next;
      }
      zeiger->next = help;
   }
   return 0;
}

// gibt eine Structure info mit Variable und Wert Paar
info *getStructure(char* str) {
   anfang = NULL;
   char *token, *token1;
   token = strtok(str,"&");
   char* strdup = &str[0];
   while( token != NULL) {
      char *rem;
      rem = strchr(strdup, '=');
      uint len = rem + 1 - strdup;
      uint len1 = strlen(strdup+len)+1;
      char str1[len];
      char str2[len1];
      strncpy(str1,strdup,len-1);
      strncpy(str2,strdup+len,len1-1);
      str1[len-1] = '\0';
      str2[len1-1] = '\0';
      if(setStruct(str1,str2) == -1) {
         return NULL;
      };
      strdup += strlen(token)+1;
      token = strtok(NULL,"&");
   }
   return anfang;
}

// gibt die Structure aus.
int getStruct(info *head) {
   info *zeiger = anfang;
   while(zeiger != NULL) {
      printf("%s -> %s <br>\n", zeiger->var, zeiger->wert);
      zeiger = zeiger->next;
   }
   return 0;
}

// befreie info Struct
void free_info(info* head) {
   info *zeiger, *zeiger1;
   if (head == NULL)
   {
      zeiger = head;
      while(zeiger->next != NULL) {
         zeiger1 = zeiger;
         zeiger = zeiger->next;
         free(zeiger1->var);
         free(zeiger1->wert);
         free(zeiger1);
      }
   }
}

int main(int argc, char* argv[]) {
   char *str;
   print_head();
   print_header("Auswert");
   if((str = getStrings(getRMethod())) == NULL) {
      return EXIT_FAILURE;
   }
   filterString(str);
   anfang = getStructure(str);
   if(anfang != NULL) {
      getStruct(anfang);
   }
   free(str);
   free_info(anfang);
   print_end();
   return EXIT_SUCCESS;
}