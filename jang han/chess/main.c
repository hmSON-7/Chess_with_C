#include <stdio.h>
#include <stdlib.h>

char board[8][8] = {
                    { 'R' , 'H' , 'C' , 'K' , 'Q' , 'C' , 'H' , 'R' },
                    { 'P' , 'P' , 'P' , 'P' , 'P' , 'P' , 'P' , 'P' },
                    { ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' },
                    { ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' },
                    { ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' },
                    { ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' },
                    { 'p' , 'p' , 'p' , 'p' , 'p' , 'p' , 'p' , 'p' },
                    { 'r' , 'h' , 'c' , 'k' , 'q' , 'c' , 'h' , 'r' }
                    };



main()
{
int  x = 0 ;
char ch ;

    printf( "\n\tWELCOME TO CHESS GAME" ) ;
    printf("\n\n\t Have fun playing ");
    printf( "\n\n\t\t han seong" ) ;
    getch();
    system( "cls" ) ;

 do
 {
 x++ ;
 system( "cls" ) ;
 display();

 if( (x%2) == 1 )
 {
     player1();
 }
 else
 {
     player2();
 }

 printf( " \n\nPress Enter To Continue ! \n\n " ) ;

 ch = getch();
 }while( ch == 13 ) ;

}


void display()
{
    int i , j , k ;

    printf( " " ) ;
    for( i=0 ; i<8 ; i++ ) printf( "    %d" , i ) ; printf( "\n" ) ;

    for( k=0 ; k<8 ; k++ )
    {
    printf( "  " ) ;
    for( i=0 ; i<42 ; i++ ) { printf( "-" ) ; } printf( "\n" ) ; printf( "%d " , k ) ;

    for( j=0 ; j<8 ; j++ ) { printf( "|| %c " , board[k][j] ) ; }
    printf( "|| \n" ) ;
    }

    printf( "  " ) ;
    for( i=0 ; i<42 ; i++ ) { printf( "-" ) ; } printf( "\n" ) ;

}

void change( int r1 , int c1 , int r2 , int c2 )
{
    char temp ;

    temp = board[r1][c1] ;
    board[r1][c1] = board[r2][c2] ;
    board[r2][c2] = temp ;

}


void player1()
{
    int p1 , p2 , c1 , r1 , c2 , r2;

    printf( "\nPLAYER 1 - Turn(samll)Case\n" ) ;
    again1:
    printf( "\nEnter Position of Element to change ( RC ): " ) ;
    scanf( "%d" , &p1 ) ;

    c1 = p1 % 10 ;
    r1 = p1 / 10 ;

     switch( board[r1][c1] )
    {
        case 'p': pawnb( r1 , c1 ) ;
                  break ;
        case 'r': rook( r1 , c1 ) ;
                  break ;
        case 'h': horse( r1 , c1 ) ;
                  break ;
        case 'c': camel( r1 , c1 ) ;
                  break ;
        case 'k': king( r1 , c1 ) ;
                  break ;
        case 'q': queen( r1 , c1 ) ;
                  break ;
        default: printf( "Invalid Position ! " ) ; goto again1 ;
    }


    printf( "\nEnter Position of Place to Send ( RC ): " ) ;
    scanf( "%d" , &p2 ) ;

    c2 = p2 % 10 ;
    r2 = p2 / 10  ;


    change(r1,c1,r2,c2) ;
}

void player2()
{
    int p1 , p2 , c1 , r1 , c2 , r2;

    printf( "\nPLAYER 2 - Turn(big) Case \n") ;
    again2:
    printf( "\nEnter Please enter your destination ( RC ): " ) ;
    scanf( "%d" , &p1 ) ;

    c1 = p1 % 10 ;
    r1 = p1 / 10 ;

    switch( board[r1][c1] )
    {
        case 'P': pawn( r1 , c1 );
                  break ;
        case 'R': rook( r1 , c1 ) ;
                  break ;
        case 'H': horse( r1 , c1 );
                  break ;
        case 'C': camel( r1 , c1 );
                  break ;
        case 'K': king( r1 , c1 ) ;
                  break ;
        case 'Q': queen( r1 , c1 ) ;
                  break ;
        default: printf("Invalid Position ! ") ; goto again2 ;
    }


    printf( "\nPlease enter your destination ( RC ): " ) ;
    scanf( "%d" , &p2 ) ;

    c2 = p2 % 10 ;
    r2 = p2 / 10  ;


    change(r1,c1,r2,c2) ;
}

