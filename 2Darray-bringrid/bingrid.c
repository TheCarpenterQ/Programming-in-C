#include "bingrid.h"

void Pairs(board* brd);  //3 rules
void OXO(board* brd);
void Counting(board* brd);
char rule(char l, char r);  //function for rule 1&2
bool access(int sz, int n, int m);  //boundary check

bool str2board(board* brd, char* str)
{
   int k = 0;
   if(strcmp(str, "") == 0){     //check if valid
       return false;
   }else if(strlen(str) % 2 != 0){     //check even
       return false;
   }else{
      brd->sz = strlen(str) / 2;        //put string into board      correct:sqrt(strlen(str))       ??????
      for(int i = 0;i < brd->sz;i ++){
         for(int j = 0;j < brd->sz;j ++){
            brd->b2d[i][j] = str[k];
            k ++;
         }
      }
      return true;
   }
}

void board2str(char* str, board* brd)
{
   int k = 0;
   for(int i = 0;i < brd->sz;i ++){
      for(int j = 0;j < brd->sz;j ++){
         str[k] = brd->b2d[i][j];
         k ++;
      }
   }
}

bool solve_board(board* brd)
{
   board* brd0;
   do{          //If the 3 rules continue to make change
      brd0 = brd;
      Pairs(brd);
      OXO(brd);
      Counting(brd);
   }while(brd0 != brd);
   for(int i = 0;i < brd->sz;i ++){
      for(int j = 0;j < brd->sz;j ++){
         if(brd->b2d[i][j] == UNK){     //After stopping, if '.' in the puzzle
            return false;
         }
      }
   }
   return true;
}

void printboard(board* brd)
{
   for(int i = 0;i < brd->sz;i ++){
      for(int j = 0;j < brd->sz;j ++){
         printf("%d", brd->b2d[i][j]);
      }
      printf("\n");
   }
}

void test(void)
{
   assert(access(4, -1, 2) == false);
   assert(access(4, 5, 2) == false);
   assert(rule('1', '1') == ZERO);
   assert(rule('0', '0') == ONE);
}

void Pairs(board* brd)
{
   int i, j;
   char l, r;
   for(i = 0;i < brd->sz;i ++){
      for(j = 0;j < brd->sz;j ++){
         if(access(brd->sz, i, j + 1)){
            l = brd->b2d[i][j];
            r = brd->b2d[i][j + 1];
            if(l == r && l != UNK){
               if(j - 1 >= 0){
                   brd->b2d[i][j - 1] = rule(l, r);
               }
               if(j + 2 <= brd -> sz){
                   brd->b2d[i][j + 2] = rule(l, r);
               }
            }
         }
         if(access(brd->sz, i + 1, j)){
            r = brd->b2d[i + 1][j];
            if(l == r && l != UNK){
               if(i - 1 >= 0){
                  brd->b2d[i - 1][j] = rule(l, r);
               }
               if(i + 2 <= brd -> sz){
                  brd->b2d[i + 2][j] = rule(l, r);
               }
            }
         }
      }
   }
}

void OXO(board* brd)
{
   int i, j;
   char l, r;
   for(i = 0;i < brd->sz;i ++){
      for(j = 0;j < brd->sz - 2;j ++){
         if(access(brd->sz, i, j)){
            if(access(brd->sz, i, j + 2)){
               l = brd->b2d[i][j];
               r = brd->b2d[i][j + 2];
               if(l == r && l != UNK){
                  brd->b2d[i][j + 1] = rule(l, r);
               }
            }
         }
         if(access(brd->sz, i + 2, j)){
            r = brd->b2d[i + 2][j];
            if(l == r && l != UNK){
               brd->b2d[i + 1][j] = rule(l, r);
            }
         }
      }
   }
}

void Counting(board* brd)
{
   int cnt0 = 0, cnt1 = 0, i, j, m, n;
   for(i = 0;i < brd->sz;i ++){
      for(j = 0;j < brd->sz;j ++){
         if(brd->b2d[i][j] == ZERO){
            cnt0 ++;
         }else if(brd->b2d[i][j] == ONE){
            cnt1 ++;
         }
      }
      if(cnt0 == brd->sz / 2){
         for(m = 0;m < brd->sz;m ++){
            if(brd->b2d[i][m] == UNK){
               brd->b2d[i][m] = ONE;
            }
         }
      }else if(cnt1 == brd->sz / 2){
         for(n = 0;n < brd->sz;n ++){
            if(brd->b2d[i][n] == UNK){
               brd->b2d[i][n] = ZERO;
            }
         }
      }
      cnt0 = 0, cnt1 = 0;
   }
   for(i = 0;i < brd->sz;i ++){
      for(j = 0;j < brd->sz;j ++){
         if(brd->b2d[j][i] == ZERO){
            cnt0 ++;
         }else if(brd->b2d[j][i] == ONE){
            cnt1 ++;
         }
      }
      if(cnt0 == brd->sz / 2){
         for(m = 0;m < brd->sz;m ++){
            if(brd->b2d[m][i] == UNK){
               brd->b2d[m][i] = ONE;
            }
         }
      }else if(cnt1 == brd->sz / 2){
         for(n = 0;n < brd->sz;n ++){
            if(brd->b2d[n][i] == UNK){
               brd->b2d[n][i] = ZERO;
            }
         }
      }
      cnt0 = 0, cnt1 = 0;
   }
}

char rule(char l, char r)
{
   if(l == ONE && r == ONE){
      return ZERO;
   }else{
      return ONE;
   }
}

bool access(int sz, int n, int m)
{
   if(n < 0 || m < 0){
      return false;
   }else if(n >= sz || m >= sz){
      return false;
   }
   return true;
}


