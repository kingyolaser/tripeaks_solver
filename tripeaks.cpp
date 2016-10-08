
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/****************************************************************************/
#define WIDTH     10
#define LAYERS    4
#define STOCK_LEN 24

class Board{
  public:
    enum{
        card_invalid=-1,
        card_empty=0,
        card_unknown=14,
    };
    //const static int WIDTH=10;
    //const static int STOCK_LEN=24;
  
  
    int tableau[LAYERS][WIDTH];  //例：[1][1]の上(画面上は下)に[1][2], [2][2]がかぶってる
    int stock[STOCK_LEN+1];     //stock[0]が最初のカード。nullターミネート
    int stock_nowpos;
    int pile_card;
    
    void init();
    int  c2i(char c);
    char i2c(int i){return " A234567890JQK*"[i];}
    void setTableau(int layer, int x, char val){tableau[layer][x]=c2i(val);}
    void setTableau_layer3all(const char *);
    void setStock_all(const char *);
    void print();
    
    bool isremovable(int layer, int x);
}board;

/****************************************************************************/
void Board::init()
{
    memset(tableau,0,sizeof(tableau));
    memset(stock,0,sizeof(stock));

    tableau[0][0] = card_unknown;
    tableau[0][3] = card_unknown;
    tableau[0][6] = card_unknown;

    tableau[1][0] = card_unknown;
    tableau[1][1] = card_unknown;
    tableau[1][3] = card_unknown;
    tableau[1][4] = card_unknown;
    tableau[1][6] = card_unknown;
    tableau[1][7] = card_unknown;

    tableau[2][0] = card_unknown;
    tableau[2][1] = card_unknown;
    tableau[2][2] = card_unknown;
    tableau[2][3] = card_unknown;
    tableau[2][4] = card_unknown;
    tableau[2][5] = card_unknown;
    tableau[2][6] = card_unknown;
    tableau[2][7] = card_unknown;
    tableau[2][8] = card_unknown;

    stock_nowpos=0;
    pile_card=card_empty;
}

/****************************************************************************/
int Board::c2i(char c)
{
    const char table[] = "A234567890JQK";
    const char *pos = strchr( table, c);
    if( pos ){return pos-table+1;}
    if( c=='1' ){return 1;}
    return card_invalid;
}

/****************************************************************************/
void Board::setTableau_layer3all(const char *data)
{
    for(int i=0; i<WIDTH; i++){
        tableau[3][i] = c2i(data[i]);
    }
}

/****************************************************************************/
void Board::setStock_all(const char *data)
{
    for(int i=0; i<WIDTH; i++){
        stock[i] = c2i(data[i]);
    }
    pile_card = stock[0];
}

/****************************************************************************/
void Board::print()
{
    //layer 0
    printf("   ");
    for(int i=0; i<7; i++){
        printf("%c ", i2c(tableau[0][i]));
    }
    printf("\n");
    //layer 1
    printf("  ");
    for(int i=0; i<8; i++){
        printf("%c ", i2c(tableau[1][i]));
    }
    printf("\n");
    //layer 2
    printf(" ");
    for(int i=0; i<9; i++){
        printf("%c ", i2c(tableau[2][i]));
    }
    printf("\n");
    //layer 1
    for(int i=0; i<WIDTH; i++){
        printf("%c ", i2c(tableau[3][i]));
    }
    printf("\n");
    
    //pile
    printf("Pile: %c\n", i2c(pile_card) );
}

/****************************************************************************/
bool Board::isremovable(int layer, int x)
{
    if( tableau[layer][x]==card_empty ){ return false; }
    if( layer<=2 ){
        if( tableau[layer+1][x]   != card_empty ){ return false;}
        if( tableau[layer+1][x+1] != card_empty ){ return false;}
    }
    
    if( tableau[layer][x] == 1 ){
        if( pile_card != 2 && pile_card != 13 ){ return false; }
    } else if( tableau[layer][x] == 13 ){
        if( pile_card != 1 && pile_card != 12 ){ return false; }
    } else {
        if( tableau[layer][x]+1 != pile_card
         && tableau[layer][x]-1 != pile_card ){ return false; }
    }
    
    return true;
}
/****************************************************************************/
void usage()
{
    exit(0);
}

/****************************************************************************/
void read_layer3()
{
    char  buf[80];
    for(;;){
        printf("Input layer-3 sequence > ");
        fgets(buf, sizeof(buf), stdin);
        
        //length check
        if( strlen(buf)!=WIDTH+1 ){ continue; } //+1 means '\n'
        if( buf[WIDTH]!='\n' ){continue;}
        buf[WIDTH]='\0';
        
        //to Upper
        int i;
        for(int i=0; i<WIDTH; i++){
            buf[i] = toupper(buf[i]);
        }
        
        //check 数字 or AJQK
        for( i=0; i<WIDTH; i++){
            if( strchr("1234567890JQKA", buf[i])==NULL){
                break;
            }
        }
        if(i!=WIDTH){
        	continue;  //check NG
        }
        break; //input OK
    }
    board.setTableau_layer3all(buf);
}

/****************************************************************************/
void read_stock()
{
    char  buf[80];
    for(;;){
        printf("Input stock sequence > ");
        fgets(buf, sizeof(buf), stdin);

        //length check
        if( strlen(buf)!=STOCK_LEN+1 ){ continue; } //+1 means '\n'
        if( buf[STOCK_LEN]!='\n' ){continue;}
        buf[STOCK_LEN]='\0';
        
        //to Upper
        int i;
        for(int i=0; i<STOCK_LEN; i++){
            buf[i] = toupper(buf[i]);
        }
        
        //check 数字 or AJQK
        for( i=0; i<STOCK_LEN; i++){
            if( strchr("1234567890JQKA", buf[i])==NULL){
                break;
            }
        }
        if(i!=STOCK_LEN){
            continue;  //check NG
        }
        break; //input OK

    }
    board.setStock_all(buf);
}
/****************************************************************************/
int main(int argc, char* argv[])
{
    board.init();
    read_layer3();
    read_stock();
    board.print();
}

