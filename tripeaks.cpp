
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

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
    struct _history{
        int pile_before;
        int pile_after;
        int remove_layer;
        int remove_x;    //remove_layer=remove_x=-1 means stock2pile
    }history[52];
  
  
    int tableau[LAYERS][WIDTH];  //例：[1][1]の上(画面上は下)に[1][2], [2][2]がかぶってる
    int stock[STOCK_LEN+1];     //stock[0]が最初のカード。nullターミネート
    int stock_nowpos;
    int pile_card;
    int tesuu;
    
    void init();
    int  c2i(char c);
    char i2c(int i){return " A234567890JQK*"[i];}
    void setTableau(int layer, int x, char val){tableau[layer][x]=c2i(val);}
    void setTableau_layer3all(const char *);
    void setTableau_layer(int layer, char *);
    void setStock_all(const char *);
    void print();
    
    bool isremovable(int layer, int x)const;
    void remove(int layer, int x);
    void inquire_card(int layer, int x);
    bool isstockend(){ return stock_nowpos>=STOCK_LEN-1; }
    void stock2pile();
    void undo();
    bool isComplete(){return (tableau[0][0] == card_empty && tableau[0][3] == card_empty && tableau[0][6] == card_empty)? true:false;}
    
    void search_candidate(int layer[10], int x[10], int *num);
    bool simple_check_deadend();
    bool exist(int a, int b);
}board;

/****************************************************************************/
void Board::init()
{
    memset(tableau,0,sizeof(tableau));
    memset(stock,0,sizeof(stock));
    memset(history,0,sizeof(history));
    
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
    tesuu = 0;
}

/****************************************************************************/
int Board::c2i(char c)
{
    const char table[] = "A234567890JQK";
    const char *pos = strchr( table, c);
    if( pos ){return pos-table+1;}
    if( c=='1' ){return 1;}
    if( c==' ' ){return card_empty;}
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
void Board::setTableau_layer(int layer, char *data)
{
    size_t len;
    switch( layer ){
    case 0: len=7; assert(strlen(data)>=len); break;
    case 1: len=8; assert(strlen(data)>=len); break;
    case 2: len=9; assert(strlen(data)>=len); break;
    case 3: len=10; assert(strlen(data)>=len); break;
    default: assert(false); break;
    }
    
    for(size_t i=0; i<len; i++){
        tableau[layer][i] = c2i(toupper(data[i]));
    }
}

/****************************************************************************/
void Board::setStock_all(const char *data)
{
    for(int i=0; i<STOCK_LEN; i++){
        stock[i] = c2i(toupper(data[i]));
    }
    pile_card = stock[0];
}

/****************************************************************************/
void Board::print()
{
    //layer 0
    printf("\n   ");
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
    printf("Pile: %c(%d), nowpos=%d\n", i2c(pile_card), pile_card, stock_nowpos );

    //history
    printf("history: ");
    for(int i=0; i<tesuu; i++){
        if( history[i].remove_layer == -1 && history[i].remove_x == -1 ){
            printf("st:");
        }else{
            printf("%d%d:", history[i].remove_layer, history[i].remove_x);
        }
    }
    printf("\n");
}

/****************************************************************************/
bool Board::isremovable(int layer, int x) const
{
    int a,b;
    if( pile_card == 1 ){
        a=2; b=13;
    } else if( pile_card == 13 ){
        a=12; b=1;
    } else {
        a=pile_card-1;
        b=pile_card+1;
    }
    
    if( tableau[layer][x]!=a && tableau[layer][x]!=b ){
        return false;
    }

    if( layer<=2 ){
        if( tableau[layer+1][x]   != card_empty ){ return false;}
        if( tableau[layer+1][x+1] != card_empty ){ return false;}
    }
    
    return true;
}
/****************************************************************************/
void Board::remove(int layer, int x)
{
    //printf("removing (%d,%d)\n", layer,x);
    assert(isremovable(layer,x));

    history[tesuu].remove_layer = layer;
    history[tesuu].remove_x     = x;
    history[tesuu].pile_before = pile_card;
    history[tesuu].pile_after  = tableau[layer][x];
    tesuu ++;

    pile_card = tableau[layer][x];
    tableau[layer][x] = card_empty;
    
    //その下のカードがremove可能になったか判定
    if( layer>=1 ){
        if( x>0 && tableau[layer][x-1]==card_empty
          && tableau[layer-1][x-1]==card_unknown ){ //左上
             inquire_card(layer-1, x-1);
        }
        if( x<WIDTH-1 && tableau[layer][x+1]==card_empty
          && tableau[layer-1][x]==card_unknown ){ //右上
             inquire_card(layer-1, x);
        }
    }
}
/****************************************************************************/
void Board::inquire_card(int layer, int x)
{
    print();
    printf("inquire(%d,%d) >", layer, x);
    char buf[80];
    for(;;){
        fgets(buf, sizeof(buf), stdin);
        int val = c2i(toupper(buf[0]));
        if( val != card_invalid ){
            tableau[layer][x] = val;
            break;
        }
    }
}

/****************************************************************************/
void Board::stock2pile()
{
    assert(isstockend()==false);
    
    //printf("stock2pile\n");
    history[tesuu].remove_layer = history[tesuu].remove_x = -1;
    history[tesuu].pile_before = pile_card;
    history[tesuu].pile_after  = stock[stock_nowpos];
    tesuu ++;
    
    stock_nowpos ++;
    pile_card = stock[stock_nowpos];
}

/****************************************************************************/
void Board::undo()
{
    assert(tesuu>=1);
    
    //printf("undo!\n");
    if( history[tesuu-1].remove_layer == -1 && history[tesuu-1].remove_x ){
        assert(stock_nowpos>=1);
        pile_card = history[tesuu-1].pile_before;
        stock_nowpos--;
    }else{
        tableau[history[tesuu-1].remove_layer][history[tesuu-1].remove_x] =
             history[tesuu-1].pile_after;
        pile_card = history[tesuu-1].pile_before;
    }
    tesuu --;
}

/****************************************************************************/
void Board::search_candidate(int ret_layer[10], int ret_x[10], int *num)
{
    *num = 0;
    for( int x=0; x<WIDTH; x++){
        for(int layer=LAYERS-1; layer>=0; layer--){
            if( tableau[layer][x] == card_empty ){continue;}
            if( isremovable(layer,x) ){
                assert(*num<=10-1);
                ret_layer[*num] = layer;
                ret_x[*num] = x;
                (*num)++;
            }
            //empty以外なので、どっちにしろもう上を調べる必要なし
            break;
        }
    }
}
/****************************************************************************/
bool Board::simple_check_deadend()
{
    int check_card;
    
    //頂上の３枚の隣接カードが残っているかチェック
    check_card = tableau[0][0];
    if( 1<=check_card && check_card<=13 ){
        int a = (check_card%13)+1;
        int b = ((check_card+11)%13)+1;
        if( ! exist(a,b) ){ return true; } //dead end
    }
    
    check_card = tableau[0][3];
    if( 1<=check_card && check_card<=13 ){
        int a = (check_card%13)+1;
        int b = ((check_card+11)%13)+1;
        if( ! exist(a,b) ){ return true; } //dead end
    }
    
    check_card = tableau[0][6];
    if( 1<=check_card && check_card<=13 ){
        int a = (check_card%13)+1;
        int b = ((check_card+11)%13)+1;
        if( ! exist(a,b) ){ return true; } //dead end
    }

    return false;
}
/****************************************************************************/
bool Board::exist(int a, int b)
{
    if( pile_card==a || pile_card==b ){
        return true; //exist!
    }

    for( int i=stock_nowpos+1; i<STOCK_LEN; i++ ){
        if( stock[i]==a || stock[i]==b ){
            return true; //exist!
        }
    }

    for( int layer=0; layer<LAYERS; layer++){
        for( int x=0; x<WIDTH; x++ ){
            if( tableau[layer][x]==a || tableau[layer][x]==b ){
                return true; //exist!
            }
        }
    }
    
    return false; //not exist
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
int test();
void action(Board &board);

int main(int argc, char* argv[])
{
#ifdef TEST
    if(argc>=2 && strcmp(argv[1],"--test")==0 ){
        return test();
    }
#endif
    
    board.init();
    
    if( argc==6 ){
        board.setTableau_layer(0, argv[1]);
        board.setTableau_layer(1, argv[2]);
        board.setTableau_layer(2, argv[3]);
        board.setTableau_layer(3, argv[4]);
        board.setStock_all(argv[5]);
    }else{
        read_layer3();
        read_stock();
    }
    board.print();
    action(board);
}

/****************************************************************************/
//候補手のストア
struct _candidate_data{
    int layer[10];
    int x[10];
    int num;
    //int selecting;  //-1 means stock2pile
} candidate_data[52];

void action(Board &board)
{
    if( board.isComplete() ){
        board.print();
        printf("Congraturation!!\n");
        exit(0);
    }
    
    if( board.tesuu==11 ){
        printf("=======now testing==============\n");
        board.print();
        printf("================================\n");
    }
    
    //board.print();
    board.search_candidate(candidate_data[board.tesuu].layer,
                           candidate_data[board.tesuu].x,
                          &candidate_data[board.tesuu].num);
    for( int i=0; i<candidate_data[board.tesuu].num; i++ ){
        board.remove(candidate_data[board.tesuu].layer[i],
                     candidate_data[board.tesuu].x    [i]);
        action(board);  //もし関数から返ってきたら、NGだったということ
        board.undo();
    }
    //forを抜けてしまった＝removeする手が全NG or removeできない
    if( !board.isstockend() ){
        board.stock2pile();
        
        if( board.simple_check_deadend() ){
            #if 0
            if( board.stock_nowpos <=22 ){ //早期発見時 デバッグ表示
                board.print();
                printf("###This is dead end###\n");
                printf("Hit enter\n");
                char dummy[80];
                fgets(dummy, 80, stdin);
            }
            #endif
            //deadendなのでaction()しない
        }else{
            action(board);  //もし関数から返ってきたら、NGだったということ
        }
        board.undo();
    }
    
    //ここまで来たら、すべての手がNG,どんずまり。
}

/****************************************************************************/
#ifdef TEST
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

class FunctionTest : public CPPUNIT_NS::TestFixture{
    //テストクラス
    CPPUNIT_TEST_SUITE(FunctionTest);//登録のスタート
    CPPUNIT_TEST(test_test);
    CPPUNIT_TEST(test_undo);
    CPPUNIT_TEST(test_search_candidate);
    CPPUNIT_TEST_SUITE_END();//登録の終了

protected:
    Board *pBoard;
    void test_test();
    void test_undo();
    void test_search_candidate();

public:
    void setUp();
    void testDown();
};

/****************************************************************************/
CPPUNIT_TEST_SUITE_REGISTRATION(FunctionTest);

//テスト起動時に実行
void FunctionTest::setUp(){
    pBoard = new Board();
}

//テスト終了時に実行
void FunctionTest::testDown(){
    delete pBoard;
}

/****************************************************************************/
void FunctionTest::test_test()
{
    pBoard->init();
    pBoard->setTableau_layer3all("1234567890");
    pBoard->setStock_all("1234567890JQK1234567890J");
    pBoard->print();
    
    //Pile top=A
    CPPUNIT_ASSERT_EQUAL(false, pBoard->isComplete());
    CPPUNIT_ASSERT_EQUAL(pBoard->isremovable(0,0), false );
    CPPUNIT_ASSERT_EQUAL(pBoard->isremovable(0,1), false );
    CPPUNIT_ASSERT_EQUAL(pBoard->isremovable(3,0), false );
    CPPUNIT_ASSERT_EQUAL(pBoard->isremovable(3,1), true );
    
    //remove test
    pBoard->remove(3,1); //card 2
    CPPUNIT_ASSERT_EQUAL(pBoard->tableau[3][1], 0);
    CPPUNIT_ASSERT_EQUAL(pBoard->pile_card, 2);

    //stock2pile test
    pBoard->stock2pile();
    CPPUNIT_ASSERT_EQUAL(pBoard->pile_card, 2);
    pBoard->stock2pile();
    CPPUNIT_ASSERT_EQUAL(pBoard->pile_card, 3);
    
    //
    pBoard->remove(3,3); //card 4
    pBoard->remove(3,4); //card 5
    pBoard->print();
}

/****************************************************************************/
void FunctionTest::test_undo()
{
    pBoard->init();
    pBoard->setTableau_layer3all("1234567890");
    pBoard->setStock_all("1234567890JQK1234567890J");
    //pBoard->print();
    
    //remove test
    pBoard->remove(3,1); //card 2
    pBoard->undo();
    CPPUNIT_ASSERT_EQUAL(pBoard->tesuu, 0);
    CPPUNIT_ASSERT_EQUAL(pBoard->pile_card, 1);
    
    pBoard->stock2pile();
    pBoard->undo();
    CPPUNIT_ASSERT_EQUAL(pBoard->pile_card, 1);
    CPPUNIT_ASSERT_EQUAL(pBoard->stock_nowpos, 0);
}
/****************************************************************************/
void FunctionTest::test_search_candidate()
{
    pBoard->init();
    pBoard->setTableau_layer3all("1234567890");
    pBoard->setStock_all("1234567890JQK1234567890J");

    int layer[10], x[10], num;
    pBoard->search_candidate(layer, x, &num);
    CPPUNIT_ASSERT_EQUAL(1, num);
    CPPUNIT_ASSERT_EQUAL(3, layer[0]);
    CPPUNIT_ASSERT_EQUAL(1, x[0]);
}

/****************************************************************************/
int test()
{
    CPPUNIT_NS::TestResult controller;

    //結果収集
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);

    //途中結果の収集
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener(&progress);

    //テストを走らせる。テストを入れて走る
    CPPUNIT_NS::TestRunner runner;
    runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run(controller);

    //結果を標準出力にする。
    CPPUNIT_NS::CompilerOutputter outputter(&result,CPPUNIT_NS::stdCOut());
    outputter.write();

    return result.wasSuccessful() ? 0 : 1;
}
#endif //TEST
