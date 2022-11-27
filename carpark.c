/*In my laptop and lab PC,looks like it works well in test all 'invalid prks' 
and in 4x4, 6x6 prk*/
/*But seems run out of memory in larger prks*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXSIZE 20
#define MAX 1000
#define MINLEN 2
#define ALPHABETA 26
#define WALL '#'
#define SPACE '.'
#define DIRECTION 4

typedef struct Position{
   int px;
   int py;
}Position;

typedef struct Car{
   char name;
   Position pos[MAXSIZE];
   int len;
   bool orien;
   bool exit;
}Car;

typedef struct Park{
   char map[MAXSIZE][MAXSIZE];
   Car tcar[ALPHABETA];
   int parent;
}Park;

void test(void);
bool check_valid(char op[][MAXSIZE], int HEIGHT, int WIDTH);  //check if valid
bool valid_door(char op[][MAXSIZE], int HEIGHT, int WIDTH);  //check if door > 0
bool valid_char(char op[][MAXSIZE], int HEIGHT, int WIDTH);  //check extra char & weird char
bool valid_car(char op[][MAXSIZE], int HEIGHT, int WIDTH);  //check sec & shape
int init_park(Park p[MAX], char op[][MAXSIZE] ,int HEIGHT, int WIDTH);  //initial 1st prk
bool check_solution(int orien, int x, int y, char op[][MAXSIZE], int HEIGHT, int WIDTH);  //check if solut for everycar
bool check_name(Park p[MAX], char name, int cnt);  //check if name has been stored
bool check_bond(int m, int n, int HEIGHT, int WIDTH);  //check bond
void solve(bool show, int num, Park p[MAX], int HEIGHT, int WIDTH);  //solve
int move_car(int f, int num, Park p[MAX], int HEIGHT, int WIDTH);  //move every car in every prk
bool unique_add(int f, int cnt, int n, Park p[MAX], Park tmp);  //if unique prk,add it
bool check_exit(int px, int py, int HEIGHT, int WIDTH);  //check if car reach bond
bool cexit(Park p[MAX], int f, int num);  //check if car already exit
bool check_final(int cf, int num, Park p[MAX]);  //check if all cars exit
int check_print(int f, Park p[MAX],int HEIGHT, int WIDTH);  //print if -show

int main(int argc, char* argv[]){
   test();
   bool show;
   argc = 0;
   FILE* fp;
   if(strcmp(argv[1], "-show") == 0){
      show = true;
      fp = fopen(argv[2], "r");
   }else{
      show = false;
      fp = fopen(argv[1], "r");
   }
   if(fp == NULL){
      fprintf(stderr, "Cannot open file?\n");
      exit(EXIT_FAILURE);
   }
   int WIDTH, HEIGHT;
   fscanf(fp, "%dx%d", &HEIGHT, &WIDTH);
   Park p[MAX];
   char str[MAXSIZE];
   int l = 0, len;
   while(fgets(str, MAXSIZE, fp) != NULL){
      if(l == 0){
         l ++;
      }else{
         len = strlen(str);
         str[len - 1] ='\0';
         strcpy(p[0].map[l - 1], str);
         l ++;
      }
   }
   if(check_valid(p[0].map, HEIGHT, WIDTH) == false){
      printf("Invalid Carpark?");
      exit(EXIT_FAILURE);
   }
   int num = init_park(p, p[0].map, HEIGHT, WIDTH);
   solve(show, num, p, HEIGHT, WIDTH);
   fclose(fp);
   return 0;
}

void test(void){
   char arr1[][MAXSIZE] = {{'#','.','#','#','#','#'},{'.','B','B','B','.','#'},{'#','A','.','.','.','#'}\
,{'#','A','.','.','.','#'},{'#','A','.','.','.','#'},{'#','#','#','#','#','#'}};
   assert(valid_door(arr1, 6, 6) == true);
   char arr2[][MAXSIZE] = {{'#','#','#','#','#','#'},{'#','B','B','B','.','#'},{'#','A','.','.','.','#'}\
,{'#','A','.','.','.','#'},{'#','A','.','.','.','#'},{'#','#','#','#','#','#'}};
   assert(valid_door(arr2, 6, 6) == false);
   char arr3[][MAXSIZE] = {{'#','.','#','#','#','#', ' '},{'.','B','B','B','.','#'}\
,{'#','A','.','.','.','#'},{'#','A','.','.','.','#'},{'#','A','.','.','.','#'}\
,{'#','#','#','#','#','#'}};
   assert(valid_char(arr3, 6, 6) == false);
   char arr4[][MAXSIZE] = {{'@','.','#','#','#','#'},{'.','B','B','B','.','#'},{'#','A','.','.','.','#'}\
,{'#','A','.','.','.','#'},{'#','A','.','.','.','#'},{'#','#','#','#','#','#'}};
   assert(valid_char(arr4, 6, 6) == false);
   char arr5[][MAXSIZE] = {{'#','.','#','#','#','#'},{'.','C','C','C','.','#'},{'#','A','.','.','.','#'}\
,{'#','A','.','.','.','#'},{'#','A','.','.','.','#'},{'#','#','#','#','#','#'}};
   assert(valid_car(arr5, 6, 6) == false);
   char arr6[][MAXSIZE] = {{'#','.','#','#','#','#'},{'.','B','B','B','.','#'},{'#','A','A','.','.','#'}\
,{'#','A','.','.','.','#'},{'#','A','.','.','.','#'},{'#','#','#','#','#','#'}};
   assert(valid_car(arr6, 6, 6) == false);
   char arr7[][MAXSIZE] = {{'#','.','#','#','#','#'},{'#','.','.','.','.','#'},{'#','.','.','.','.','#'}\
,{'#','.','.','.','.','#'},{'#','.','.','.','.','#'},{'#','#','#','#','#','#'}};
   assert(valid_car(arr7, 6, 6) == false);
   char arr8[][MAXSIZE] = {{'#','.','#','#','#','#'},{'#','.','.','.','.','#'},{'#','.','.','.','.','#'}\
,{'#','A','.','.','.','#'},{'#','A','.','.','.','#'},{'#','#','#','#','#','#'}};
   assert(check_solution(0, 1, 1, arr8, 6, 6) == false);
   assert(check_bond(6, 0, 6, 6) == false);
   assert(check_exit(5, 0, 6, 6) == true);
}

bool check_valid(char op[][MAXSIZE], int HEIGHT, int WIDTH){
   if(valid_door(op, HEIGHT, WIDTH) == false){
      return false;
   }
   if(valid_char(op, HEIGHT, WIDTH) == false){
      return false;
   }
   if(valid_car(op, HEIGHT, WIDTH) == false){
      return false;
   }
   return true;
}

bool valid_door(char op[][MAXSIZE], int HEIGHT, int WIDTH){
   int cnt = 0;
   for(int i = 0;i < HEIGHT ;i ++){
      for(int j = 0;j < WIDTH ;j ++){
         if(i == 0 || j == 0 || i == HEIGHT - 1 || j == WIDTH - 1){
            if(op[i][j] == SPACE)
               cnt ++;
         }
      }
   }
   if(cnt == 0)
      return false;
   return true;
}

bool valid_char(char op[][MAXSIZE], int HEIGHT, int WIDTH){
   int cnt = 0;
   for(int i = 0;i < MAXSIZE;i ++){
      for(int j = 0;j < MAXSIZE;j ++){
         if(op[i][j] == WALL || op[i][j] == SPACE || (op[i][j] >= 65 && op[i][j] <= 90)){
            cnt ++;
         }else if(op[i][j] == ' ')
            return false;
      }
      if(i <= HEIGHT - 1 && cnt != WIDTH){
         return false;
      }
      cnt = 0;
   }
   return true;
}

bool valid_car(char op[][MAXSIZE], int HEIGHT, int WIDTH){
   int max = 0, cnt = 0;
   char check[ALPHABETA];
   for(int i= 0;i < HEIGHT - 1;i ++){
      for(int j = 0;j < WIDTH - 1;j ++){
         if(op[i][j] != WALL && op[i][j] != SPACE && check[op[i][j] - 65] == 0){
            cnt ++;
            check[op[i][j] - 65] = 1;
            if(op[i][j] > max)
               max = op[i][j];
         }else if(op[i][j] != WALL && op[i][j] != SPACE){
            cnt ++;
         }
         if(op[i][j] != WALL && op[i][j] != SPACE){
            if(check_bond(i - 1, j - 1, HEIGHT, WIDTH)){
                if(op[i - 1][j] == op[i][j - 1] && op[i - 1][j] != WALL && op[i - 1][j] != SPACE)
                   return false;
            }
            if(check_bond(i - 1, j + 1, HEIGHT, WIDTH)){
                if(op[i - 1][j] == op[i][j + 1] && op[i - 1][j] != WALL && op[i - 1][j] != SPACE)
                   return false;
            }
            if(check_bond(i + 1, j + 1, HEIGHT, WIDTH)){
                if(op[i + 1][j] == op[i][j + 1] && op[i + 1][j] != WALL && op[i + 1][j] != SPACE)
                   return false;
            }
            if(check_bond(i + 1, j - 1, HEIGHT, WIDTH)){
                if(op[i + 1][j] == op[i][j - 1] && op[i + 1][j] != WALL && op[i + 1][j] != SPACE)
                   return false;
            }
         }
      }
   }
   for(int m = 0;m < max - 65;m ++){
      if(check[m] == 0)
         return false;
   }
   if(cnt == 0)
      return false;
   return true;
}

int init_park(Park p[MAX], char op[][MAXSIZE], int HEIGHT, int WIDTH){
   int cnt = 0, cnt1 = 0, k;
   for(int i = 0;i < HEIGHT;i ++){
      for(int j = 0;j < WIDTH;j ++){
         if(op[i][j] != WALL && op[i][j] != SPACE && check_name(p, op[i][j], cnt)){
            p[0].tcar[cnt].name = op[i][j];
            p[0].tcar[cnt].exit = 0;
            if(check_bond(i, j + 1, HEIGHT, WIDTH)){
               if(op[i][j] == op[i][j + 1]){
                  p[0].tcar[cnt].orien = 0;
                  for(k = j;k < WIDTH - 1;k ++){
                     if(op[i][k] == op[i][j]){
                        p[0].tcar[cnt].pos[k - j].px = k;
                        p[0].tcar[cnt].pos[k - j].py = i;
                        cnt1 ++;
                     }
                  }
                  p[0].tcar[cnt].len = cnt1;
               }
            }
            if(check_bond(i + 1, j, HEIGHT, WIDTH)){
                if(op[i][j] == op[i + 1][j]){
                  p[0].tcar[cnt].orien = 1;
                  for(k = i;k < HEIGHT - 1;k ++){
                     if(op[k][j] == op[i][j]){
                        p[0].tcar[cnt].pos[k - i].px = j;
                        p[0].tcar[cnt].pos[k - i].py = k;
                        cnt1 ++;
                     }
                  }
                  p[0].tcar[cnt].len = cnt1;
               }
            }
            if(check_solution(p[0].tcar[cnt].orien, j, i, op, HEIGHT, WIDTH) == false){
               printf("No Solution?");
               exit(EXIT_FAILURE);
            }else{
               cnt1 = 0;
               cnt ++;
            }
         }
      }
   }
   p[0].parent = 0;
   return cnt;
}

bool check_solution(int orien, int x, int y, char op[][MAXSIZE], int HEIGHT, int WIDTH){
  if(orien == 0){
      if(op[y][0] == WALL && op[y][WIDTH - 1] == WALL)
         return false;
   }else if(orien == 1){
      if(op[0][x] == WALL && op[HEIGHT - 1][x] == WALL)
         return false;
   }
   return true;
}

bool check_name(Park p[MAX], char name, int cnt){
   for(int i = 0;i < cnt;i ++){
      if(p[0].tcar[i].name == name)
         return false;
   }
   return true;
}

bool check_bond(int m, int n, int HEIGHT, int WIDTH){
   if(m < 0 || n < 0){
      return false;
   }else if(m >= HEIGHT || n >= WIDTH){
      return false;
   }
   return true;
}

void solve(bool show, int num, Park p[MAX], int HEIGHT, int WIDTH){
   int i = 0, f = 0, cnt = 0, n = 0, m = 0, pn = 0;
   do{
      pn = n;
      cnt = move_car(f, num, p, HEIGHT, WIDTH);
      n = n + cnt;
      for(i = pn;i < n + 1;i ++){	
         if(check_final(i, num, p) == true){  
            if(show)
               m = check_print(i, p, HEIGHT, WIDTH);
            printf("%i moves", m);
            return;
         }
      }
      f ++;
   }while(check_final(i, num, p) == false);
   printf("solve failed.");
}

int move_car(int f, int num, Park p[MAX], int HEIGHT, int WIDTH){
   Park tmp;
   int cnt = 0, j;
   for(int i = 0;i < num;i ++){
      if(p[f].tcar[i].orien == 0){
         if(p[f].map[p[f].tcar[i].pos[0].py][p[f].tcar[i].pos[0].px - 1] == SPACE \
&& cexit(p, f + cnt, i)){
            tmp = p[f];
            for(j = 0;j < p[f].tcar[i].len;j ++){
               tmp.tcar[i].pos[j].px = tmp.tcar[i].pos[j].px - 1;
            }
            if(unique_add(f, cnt + 1, num, p, tmp)){
               cnt ++;
               if(check_exit(tmp.tcar[i].pos[0].px,tmp.tcar[i].pos[0].py, HEIGHT, WIDTH)){
                  p[f + cnt].tcar[i].exit = 1;
                  for(j = 0;j < p[f].tcar[i].len;j ++){
                     p[f + cnt].map[tmp.tcar[i].pos[j].py][tmp.tcar[i].pos[j].px + 1] = SPACE;
                  }
               }else{
                  p[f + cnt].map[p[f + cnt].tcar[i].pos[0].py][p[f + cnt].tcar[i].pos[0].px]\
 = p[f].tcar[i].name;
                  p[f + cnt].map[p[f + cnt].tcar[i].pos[0].py][p[f + cnt].tcar[i].pos[0].px + \
p[f + cnt].tcar[i].len] = SPACE;
               }
            }
         }
         if(p[f].map[p[f].tcar[i].pos[0].py][p[f].tcar[i].pos[0].px + p[f].tcar[i].len] == SPACE\
 && cexit(p, f + cnt, i)){
            tmp = p[f];
            for(j = 0;j < p[f].tcar[i].len;j ++){
               tmp.tcar[i].pos[j].px = tmp.tcar[i].pos[j].px + 1;
            }
            if(unique_add(f, cnt + 1, num, p, tmp)){
               cnt ++;
               if(check_exit(tmp.tcar[i].pos[0].px + tmp.tcar[i].len - 1,tmp.tcar[i].pos[0].py,\
 HEIGHT, WIDTH)){
                  p[f + cnt].tcar[i].exit = 1;
                  for(j = 0;j < p[f].tcar[i].len;j ++){
                     p[f + cnt].map[tmp.tcar[i].pos[j].py][tmp.tcar[i].pos[j].px - 1] = SPACE;
                  }
               }else{
               p[f + cnt].map[p[f + cnt].tcar[i].pos[0].py][p[f + cnt].tcar[i].pos[0].px + \
p[f + cnt].tcar[i].len - 1] = p[f].tcar[i].name;
               p[f + cnt].map[p[f + cnt].tcar[i].pos[0].py][p[f + cnt].tcar[i].pos[0].px - 1] = SPACE;
               }
            }
         }
      }
      if(p[f].tcar[i].orien == 1){
         if(p[f].map[p[f].tcar[i].pos[0].py + p[f].tcar[i].len][p[f].tcar[i].pos[0].px] == SPACE \
&& cexit(p, f + cnt, i)){
            tmp = p[f];
            for(j = 0;j < p[f].tcar[i].len;j ++){
               tmp.tcar[i].pos[j].py = tmp.tcar[i].pos[j].py + 1;
            }
            if(unique_add(f, cnt + 1, num, p, tmp)){
               cnt ++;
               if(check_exit(tmp.tcar[i].pos[0].px,tmp.tcar[i].pos[0].py + tmp.tcar[i].len - 1, \
HEIGHT, WIDTH)){
                  p[f + cnt].tcar[i].exit = 1;
                  for(j = 0;j < p[f].tcar[i].len;j ++){
                     p[f + cnt].map[tmp.tcar[i].pos[j].py + 1][tmp.tcar[i].pos[j].px] = SPACE;
                  }
               }else{
               p[f + cnt].map[p[f + cnt].tcar[i].pos[0].py - 1][p[f + cnt].tcar[i].pos[0].px] = SPACE;
               p[f + cnt].map[p[f + cnt].tcar[i].pos[0].py + p[f + cnt].tcar[i].len - 1]\
[p[f + cnt].tcar[i].pos[0].px] = p[f].tcar[i].name;
               }
            }
         }
         if(p[f].map[p[f].tcar[i].pos[0].py - 1][p[f].tcar[i].pos[0].px] == SPACE && \
cexit(p, f + cnt, i)){
            tmp = p[f];
            for(j = 0;j < p[f].tcar[i].len;j ++){
               tmp.tcar[i].pos[j].py = tmp.tcar[i].pos[j].py - 1;
            }
            if(unique_add(f, cnt + 1, num, p, tmp)){
               cnt ++;
               if(check_exit(tmp.tcar[i].pos[0].px,tmp.tcar[i].pos[0].py, HEIGHT, WIDTH)){
                  p[f + cnt].tcar[i].exit = 1;
                  for(j = 0;j < p[f].tcar[i].len;j ++){
                     p[f + cnt].map[tmp.tcar[i].pos[j].py + 1][tmp.tcar[i].pos[j].px] = SPACE;
                  }
               }else{
               p[f + cnt].map[p[f + cnt].tcar[i].pos[0].py][p[f + cnt].tcar[i].pos[0].px] \
= p[f].tcar[i].name;
               p[f + cnt].map[p[f + cnt].tcar[i].pos[0].py + p[f + cnt].tcar[i].len]\
[p[f + cnt].tcar[i].pos[0].px] = SPACE;
               }
            }
         }
      }
   }
   return cnt;
}

bool cexit(Park p[MAX], int f, int num){
   for(int i = 0;i < f;i ++){
      if(p[i].tcar[num].exit == 1)
         return false;
   }
   return true;
}

bool unique_add(int f, int cnt, int n, Park p[MAX], Park tmp){
   int m = 0;
   for(int i = 0;i < f;i ++){
      for(int j = 0;j < n;j ++){
         if(tmp.tcar[j].pos[0].px == p[i].tcar[j].pos[0].px && tmp.tcar[j].pos[0].py\
 == p[i].tcar[j].pos[0].py)
            m ++;
      }
      if(m == n)
         return false;
      m = 0;
   }
   p[f + cnt] = tmp;
   p[f + cnt].parent = f;
   return true;
}

bool check_exit(int px, int py, int HEIGHT, int WIDTH){
   if(px == 0 || px == WIDTH - 1){
      return true;
   }else if(py == 0 || py == HEIGHT - 1){
      return true;
   }
   return false;
}

bool check_final(int cf, int num, Park p[MAX]){
   for(int i = 0;i < num;i ++){
      if(p[cf].tcar[i].exit == 0){
         return false;
      }
   }
   return true;
}

int check_print(int f, Park p[MAX], int HEIGHT, int WIDTH){
   int a[MAX], cnt = 1;
   a[0] = f;
   do{
      a[cnt] = p[f].parent;
      cnt ++;
      f = p[f].parent;
   }while(f > 0);
   for(int k = cnt - 1;k >= 0;k --){
      for(int i = 0;i < HEIGHT;i ++){
         for(int j = 0;j < WIDTH;j ++){
            printf("%c", p[a[k]].map[i][j]);
         }
      printf("\n");
      }
   printf("\n");
   }
   return cnt - 1;
}






