#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <ctype.h>
#include <string.h>
#include<conio.h>
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;
enum ErrorType { ERROR_EOF, ERROR_LINE_ENTER, ERROR_UNEXPECTED, ERROR_UNBOUND_SYMBOL,
                 ERROR_NON_LIST, ERROR_INVALID_ARG_NUMBER, ERROR_INVALID_ARG_TYPE,
                 ERROR_APPLY_NON_FUNCTION, ERROR_NO_RETURN_VALUE, ERROR_FORMAT, ERROR_COND 
};
enum TokenType { LEFT_PAREN, RIGHT_PAREN, DOT, QUOTE, T, NIL, STRING, INT, FLOAT, SYMBOL };
                 
struct SyntaxError {
  int mLine = 1 ;
  int mColumn = 0 ;
  char mChar ;
  ErrorType mType ;
};

struct Node {
  TokenType mTokenType;
  string mToken;
  int mark = 0 ;
  Node* lNode;
  Node* rNode;
}; 
typedef Node* Node2 ;

bool watch_if_add_one = false ;

class Token {
  public:
    SyntaxError mSaveError ;
    char mUseForRead ;
    void GetOneChar() {
      char mch = mUseForRead ;
      mUseForRead = cin.get() ;
      if ( mUseForRead == '`' ) mSaveError.mType = ERROR_EOF, throw mSaveError ;
      if ( mch == '\n' ) mSaveError.mColumn = 1, mSaveError.mLine++ ;  
      else mSaveError.mColumn++ ;
    } // GetOneChar() 
    
    void GetNonWhiteChar() { 
      do { 
        GetOneChar() ; 
      } while ( mUseForRead == ' ' || mUseForRead == '\n' ) ;
    } // GetNonWhiteChar()
    
    string GetNoTypeToken() {
      string sToken = "" ;
      char peek_char = '\0' ;
      bool out = false ; 
      GetNonWhiteChar() ;
      while ( mUseForRead == ';' ) {
        do {
          GetOneChar() ;
        } while ( mUseForRead != '\n' ) ;
        
        GetNonWhiteChar() ; 
      } // while()
      
      if ( mUseForRead == '(' || mUseForRead == ')' || mUseForRead == '\'' ) sToken += mUseForRead ;
      else if ( mUseForRead == '\"' ) { 
        do {
          sToken += mUseForRead ;
          peek_char = cin.peek() ;
          if ( peek_char != '\n' && peek_char != '\"' ) GetOneChar() ;
          else out = true ;      
        } while( out == false ) ;
        
        if ( peek_char == '\"' ) GetOneChar(), sToken += mUseForRead ;        
      } // else if
      else {  
        do {
          sToken += mUseForRead ;
          peek_char = cin.peek() ;
          if ( peek_char != '\n' && peek_char != ' ' && peek_char != '\'' && peek_char != '\"' && peek_char != '(' && peek_char != ')' )
            GetOneChar() ;
          else out = true ;           
        } while( out == false ) ;       
      } // else if()
      
      return sToken ;  
    } // GetNoTypeToken()
    
    void ReadJunk() {
      do {
        GetOneChar() ;  
      } while ( mUseForRead != '\n' ) ;
    } // ReadJunk() 
    
    Node2 GetToken() {
      Node2 first_node = new Node;
      string sToken = GetNoTypeToken() ;
      first_node->lNode = NULL, first_node->rNode = NULL, first_node->mToken = sToken ;
      if ( sToken == "(" ) first_node->mTokenType = LEFT_PAREN ;
      else if ( sToken == ")" ) first_node->mTokenType = RIGHT_PAREN ;
      else if ( sToken == "." ) first_node->mTokenType = DOT ;
      else if ( sToken == "\'" ) first_node->mToken = "quote", first_node->mTokenType = QUOTE ;
      else if ( sToken == "t" || sToken == "#t" ) first_node->mToken = "#t", first_node->mTokenType = T ;
      else if ( sToken == "nil" || sToken == "#f" ) first_node->mToken = "nil", first_node->mTokenType = NIL ;
      else if ( sToken[0] == '\"' && sToken[sToken.length()-1] != '\"' ) {
        watch_if_add_one = true ;  
        mSaveError.mType = ERROR_LINE_ENTER, throw mSaveError ; 
      } // else if
      else if ( sToken[0] == '\"' && sToken[sToken.length()-1] == '\"' ) {
        if ( sToken.length() == 1 ) {
          watch_if_add_one = true ;  
          mSaveError.mType = ERROR_LINE_ENTER, throw mSaveError ;  
        } // if()
        
        else first_node->mTokenType = STRING ;  
      } // else if()
      else if ( If_Token_Int( sToken ) ) 
        first_node->mTokenType = INT, first_node->mToken = ChangeIntFlaot( sToken, "INT" ) ;
      else if ( If_Token_Float( sToken ) ) 
        first_node->mTokenType = FLOAT, first_node->mToken = ChangeIntFlaot( sToken, "FLOAT" ) ;
      else first_node->mTokenType = SYMBOL ;
      return first_node ;
    } // GetToken()
    
    bool If_Token_Int( string sToken ) {
      if ( sToken[0] == '+' || sToken[0] == '-' || isdigit(sToken[0]) ) {
        if ( sToken.length() == 1 && sToken[0] == '+' ) return false ;
        else if ( sToken.length() == 1 && sToken[0] == '-' ) return false ; 
        for ( int i = 1; i < sToken.length() ; i++ )
          if ( isdigit(sToken[i]) == false ) return false ;    
        return true ;  
      } // if()
      else return false ;  
    } // If_Token_Int()

    bool If_Token_Float( string sToken ) {
      bool find_dot = false ;  
      if ( sToken[0] == '+' || sToken[0] == '-' ) {
        if ( sToken.length() == 1 ) return false ;  
        if ( sToken[1] == '.' && sToken.length() == 2 ) return false ; 
        for ( int i = 1; i < sToken.length() ; i++ ) {
          if ( find_dot == false && sToken[i] == '.' ) find_dot = true ;
          else if ( isdigit(sToken[i]) == false ) return false ;
        } // for()
        
        return true ; 
      } // if()
      else if ( isdigit(sToken[0]) || sToken[0] == '.' ) {
        for ( int i = 0; i < sToken.length() ; i++ ) {
          if ( find_dot == false && sToken[i] == '.' ) find_dot = true ;
          else if ( isdigit(sToken[i]) == false ) return false ;
        } // for()
        
        return true ;          
      } // else if()
      else return false ;  
    } // If_Token_Float()
    
    string ChangeIntFlaot( string sToken, string type ) {
      char a[100] ;
      if ( type == "FLOAT" ) {
        float now_float = atof( sToken.c_str() ) ; 
        sprintf( a, "%.3f", now_float ) ;
      } // if()
      else {
        int now_int = atoi( sToken.c_str() ) ;
        sprintf( a, "%d", now_int ) ;  
      } // else
      
      string newToken( a ) ;
      return newToken ;
    } // ChangeIntFlaot()
    
    
};

enum TreeType { LIST, ATOM_ONE, ATOM_TWO, DOTTED_PAIR, EXIT };
struct TreeTypeNode {
  Node2 mTree ;
  TreeType mTree_type ;  
};

struct EvalError { 
  ErrorType mType ;
  string mErrorToken ;
  Node2 mErrorTree ;
};

struct Bound_Symbol {
  int mlocation ;
  Node2 mBoundThing ;
  Node2 mBoundNode ;
};
struct Bound_Function {
  string name ;
  Node2 mBoundFunctionThing ;
  Node2 mBoundFunctionNode ;
};
class MyTree {
  public :
    Token mtoken ;
    EvalError mSaveError ;
    vector< Bound_Symbol > mUserBound ;
    vector< Bound_Symbol > mLetUserBound ;
    vector< vector< Bound_Symbol > > mAllUserBound ;
    vector< Bound_Function > mUserFunction ;
    Bound_Function mLambdaFunction ;
    bool mget_let_num = false ;
    bool mget_user_num = false ;
    void See_If_End_Tree( Node2 tree_1, TreeTypeNode treeNode_1 ) {
      bool check = false ;  
      if ( tree_1->lNode->mToken == "exit" && tree_1->rNode->mToken == ")" && tree_1->mToken == "(" )
        treeNode_1.mTree_type = EXIT, check = true ;
      else if ( tree_1->lNode->mToken == "exit" && tree_1->rNode->mToken == "nil" && tree_1->mToken == "(" )
        treeNode_1.mTree_type = EXIT, check = true ;
      if ( check == true ) throw treeNode_1 ;
    } // See_If_End_Tree()
    
    void GetMyTree() {
      bool get = false ;  
      TreeTypeNode treeNode_1 ;
      Node2 tree_1 = new Node ;
      int saveline = mtoken.mSaveError.mLine ;
      int savecolumn = mtoken.mSaveError.mColumn ;
      try {
        tree_1 = mtoken.GetToken() ;
      } // try 
      catch ( SyntaxError e ) {
        if ( e.mType == ERROR_LINE_ENTER ) get = true ;
        else throw e ;  
      } // catch()
       
      if ( mtoken.mSaveError.mLine == saveline ) mtoken.mSaveError.mLine = 1, mtoken.mSaveError.mColumn -= savecolumn ; 
      if ( mtoken.mSaveError.mLine > saveline ) mtoken.mSaveError.mLine -= saveline ;  
      
      if ( get == true ) throw mtoken.mSaveError ;
      if ( tree_1->mTokenType == INT || tree_1->mTokenType == FLOAT || tree_1->mTokenType == STRING ||
           tree_1->mTokenType == T  || tree_1->mTokenType == NIL )
        treeNode_1.mTree = tree_1, treeNode_1.mTree_type = ATOM_ONE, throw treeNode_1 ;
      else if ( tree_1->mTokenType == SYMBOL )
        treeNode_1.mTree = Watch_SYMBOL( tree_1, "" ), treeNode_1.mTree_type = ATOM_TWO, throw treeNode_1 ;   
      else if ( tree_1->mTokenType == LEFT_PAREN ) {
        BuildTree( tree_1 ) ;
        if ( tree_1->mTokenType == NIL ) treeNode_1.mTree_type = ATOM_ONE ;
        else See_If_End_Tree( tree_1, treeNode_1 ), treeNode_1.mTree_type = DOTTED_PAIR , tree_1 = Change_To_Project_Two( tree_1 ) ;
        treeNode_1.mTree = tree_1, throw treeNode_1 ;
      } // else if()
      else if ( tree_1->mTokenType == QUOTE ) {
        tree_1 = new Node, tree_1 = BuildQuoteTree() , tree_1 = Change_To_Project_Two( tree_1 ),
        treeNode_1.mTree = tree_1, treeNode_1.mTree_type = LIST, throw treeNode_1 ;  
      } // else if()
      else OutQTree( tree_1 ) ;
    } // GetNyTree()
    
    void BuildTree( Node2 tree_1 ) {
      bool see = false ;	
      Node2 node_use = new Node ;	
      Node2 tokentree_2 = mtoken.GetToken() ;
      if ( tokentree_2->mTokenType != RIGHT_PAREN ) {
        if ( tokentree_2->mTokenType == DOT ) {
          if ( tree_1->lNode != NULL && tree_1->rNode != NULL )
            mtoken.mSaveError.mType = ERROR_UNEXPECTED, mtoken.mSaveError.mChar = '.', throw mtoken.mSaveError ;
          else if ( tree_1->lNode != NULL && tree_1->rNode == NULL ) {
            tokentree_2 = mtoken.GetToken() ;
            if ( tokentree_2->mTokenType == RIGHT_PAREN )
              mtoken.mSaveError.mType = ERROR_UNEXPECTED, mtoken.mSaveError.mChar = ')', throw mtoken.mSaveError ;  
          } // else if()
		  else {
		    char q = cin.peek() ; 
            if ( q != '\n' ) mtoken.mSaveError.mType = ERROR_UNEXPECTED ;
            else mtoken.mSaveError.mType = ERROR_LINE_ENTER ;
            watch_if_add_one = true ;  
            mtoken.mSaveError.mChar = q , throw mtoken.mSaveError ;
		  } // else
        } // if()
        else see = true ;
        
		if ( tokentree_2->mTokenType == QUOTE ) {
		  if ( tree_1->lNode != NULL && tree_1->rNode != NULL )
		    mtoken.mSaveError.mType = ERROR_UNEXPECTED, mtoken.mSaveError.mChar = '\'', throw mtoken.mSaveError ;
		  else node_use = BuildQuoteTree() ;		
		} // if()
		
		if ( tree_1->lNode == NULL ) {
		  if ( tokentree_2->mTokenType == QUOTE ) tree_1->lNode = node_use ;
		  else {
		  	tree_1->lNode = tokentree_2 ;
		  	if ( tokentree_2->mTokenType == LEFT_PAREN ) BuildTree( tree_1->lNode ) ;
		  } // else
	    } // if()
	    else if ( tree_1->rNode == NULL ) {
	      if ( see == false ) {
	      	if ( tokentree_2->mTokenType == QUOTE ) tree_1->rNode = node_use ;
	      	else tree_1->rNode = tokentree_2 ;
	      	if ( tokentree_2->mTokenType == LEFT_PAREN ) BuildTree( tree_1->rNode ) ;
		  }	// if()
		  else if ( see == true ) {
		    Node2 node_use2 = new Node ;
			node_use2->mToken = "(", node_use2->mTokenType = LEFT_PAREN ;
			if ( tokentree_2->mTokenType == QUOTE ) node_use2->lNode = node_use ;
			else node_use2->lNode = tokentree_2 ;
			node_use2->rNode = NULL, tree_1->rNode = node_use2, tree_1 = tree_1->rNode ;
			if ( tokentree_2->mTokenType != QUOTE )
			  if ( tokentree_2->mTokenType == LEFT_PAREN ) BuildTree( tree_1->lNode ) ;
		  } // else if()
		} // else if()
		else {
		  mtoken.mSaveError.mType = ERROR_UNEXPECTED, mtoken.mSaveError.mChar = tokentree_2->mToken[0] ;
		  throw mtoken.mSaveError ;
	    } // else
		BuildTree( tree_1 ) ;
      } // if()
      else {
        if ( tree_1->lNode == NULL && tree_1->rNode == NULL && tree_1->mToken == "(")
		  tree_1->mToken = "nil", tree_1->mTokenType = NIL ;
		else {
		  if ( tree_1->rNode == NULL ) {
		  	Node2 use_save = new Node ;
		  	use_save->mToken = ")", use_save->mTokenType = RIGHT_PAREN, use_save->lNode = NULL, use_save->rNode = NULL ;
		  	tree_1->rNode = use_save ;
		  } // if()	
		} // else 
      } // else
    } // BuildTree()
                                   
    bool OutQTree( Node2 test ) {
	  if ( test->mTokenType != RIGHT_PAREN && test->mTokenType != DOT ) return true ;
	  else {
	  	char error = cin.peek() ;
	  	if ( test->mTokenType == RIGHT_PAREN ) {   
          mtoken.mSaveError.mType = ERROR_UNEXPECTED, mtoken.mSaveError.mChar = ')' ;
          throw mtoken.mSaveError ;	  	    
        } // if()
        watch_if_add_one = true ;
        if ( error == '\n' ) mtoken.mSaveError.mType = ERROR_LINE_ENTER, throw mtoken.mSaveError ;
        mtoken.mSaveError.mType = ERROR_UNEXPECTED, mtoken.mSaveError.mChar = error ;
        throw mtoken.mSaveError ;
	  }	// else
	} // OutQTree ()

    Node2 BuildQuoteTree() {
      Node2 node_1 = new Node, node_2 = new Node, node_3 = new Node, node_4 = new Node, get_node = new Node ;
	  node_1->mToken = "quote", node_1->mTokenType = SYMBOL, node_1->lNode = NULL, node_1->rNode = NULL ;
	  node_2->mToken = ")", node_2->mTokenType = RIGHT_PAREN, node_2->lNode = NULL, node_2->rNode = NULL ;
	  node_3->mToken = "(", node_3->mTokenType = LEFT_PAREN, node_3->lNode = NULL, node_3->rNode = node_2 ;
	  node_4->mToken = "(", node_4->mTokenType = LEFT_PAREN, node_4->lNode = node_1, node_4->rNode = node_3 ;
      Node2 tokentree = mtoken.GetToken() ;
      if ( OutQTree( tokentree ) == true ) {
        get_node->mToken = tokentree->mToken, get_node->mTokenType = tokentree->mTokenType ;
        get_node->lNode = NULL, get_node->rNode = NULL ;
        if ( tokentree->mTokenType == LEFT_PAREN ) BuildTree( get_node ) ;
        else if ( tokentree->mTokenType == QUOTE ) get_node = new Node, get_node = BuildQuoteTree() ;
        node_3->lNode = get_node ;
        return node_4 ;
      } // if()
      else return NULL ;	
	} // BuildQuoteTree()
	
	void CoutTree( Node2 tree_1, int level, bool Left_Right ) {
	  if ( tree_1 != NULL ) {
	    if ( Left_Right == false ) {
	      cout << endl ;
          if ( tree_1->mTokenType != RIGHT_PAREN ) {
            if ( tree_1->mTokenType != NIL ) {
              for ( int i = 0 ; i < level ; i++ ) cout << "  " ;
              if ( tree_1->mTokenType != LEFT_PAREN ) {
                cout << "." << endl ;
                for ( int i = 0 ; i < level ; i++ ) cout << "  " ;
                HelpCoutTree( tree_1 ) ;
                cout << endl ;
                for ( int i = 0 ; i < level - 1 ; i++ ) cout << "  " ;
                cout << ")" ;
              } // if() 
            } // if()
            else {
              for ( int i = 0 ; i < level - 1 ; i++ ) cout << "  " ;
              cout << ")" ; 
            } // else
          } // if()
          else {
            for ( int i = 0 ; i < level - 1 ; i++ ) cout << "  " ;
            cout << ")" ;             
          } // else
        } // if()
        else {
          HelpCoutTree( tree_1 ) ;
          if ( tree_1->mTokenType == LEFT_PAREN ) cout << " " ;
        } // else
        
        CoutTree( tree_1->lNode, level + 1, true ) ;
        CoutTree( tree_1->rNode, level, false ) ;  
      } // if()
    } // CoutTree()
    
    void HelpCoutTree( Node2 tree_1 ) {
      if ( SystemBound( tree_1->mToken ) == false ) {
        if ( tree_1->mark == 2 ) cout << "#<user function>" ;
        else cout << tree_1->mToken ;
      } // if()
      else {
        if ( tree_1->mark == 0 ) cout << "#<internal function>" ;
        else cout << tree_1->mToken ;                    
      } // else        
    } // HelpCoutTree()
    
    bool SystemBound( string see ) {
      if ( see == "cons" || see == "list" || see == "quote" || see == "define" ||
           see == "clean-environment" || see == "car" || see == "cdr" || 
           see == "equal?" || see == "eqv?" || see == "cond" || see == "if" ) return true ;
      else if ( see == "lambda" || see == "let" ) return true ;     
      else if ( Work_Watch_Symbol_Type( see ) || TokenAri( see ) != 0 ) return true ;   
      else return false ;  
    } // SystemBound()
    
    void Error_Time_Read( string test ) {
      if ( test == "exit" || test == "define" || test == "clean-environment" ) {
        mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = test ;
        throw mSaveError ; 
      } // if()
    } // Error_Time_Read()
    
    bool Work_Watch_Symbol_Type( string see ) {
      if ( see == "pair?" || see == "null?" || see == "integer?" || see == "real?" ||
           see ==  "number?" || see == "string?" || see == "boolean?" || see == "symbol?" )  return true ;
      else return false ;        
    } // Work_Watch_Symbol_Type()
    
    int TokenAri( string see ) {
      int num = 0 ;  
      if ( see == "+" || see == "-" || see == "*" || see == "/" || see == ">" || 
           see == "<" || see == "<=" || see == ">=" || see == "=" )  num = 1 ;
      else if ( see == "string-append" || see == "string>?" ) num = 2 ;
      else if ( see == "not" || see == "and" || see == "or" ) num = 3 ;
      else if ( see == "begin" ) num = 4 ;
      return num ;        
    } // TokenAri()
    
    Node2 Watch_Change_To_Project_Two_Type( Node2 tree_1 ) {
      if ( tree_1->lNode->mTokenType == LEFT_PAREN ) {
        Error_Time_Read( tree_1->lNode->lNode->mToken ) ;  
        Node2 use = Change_To_Project_Two( tree_1->lNode ) ;
        mSaveError.mType = ERROR_APPLY_NON_FUNCTION, mSaveError.mErrorTree = use ;
        if ( SystemBound( use->mToken ) == true ) {
          if ( use->mark == 0 ) tree_1->lNode = use ;
          else 
            throw mSaveError ; 
        } // if()
        else if ( use->mToken == "#<user function>" ) {
          tree_1->lNode = use, tree_1->mark = 50 ;  
        } // else if
        else {
          if ( FindUserFunction( use->mToken ) != -1 )
            tree_1->lNode = use ; 
          else  
            throw mSaveError ;
        } // else
      } // else
      else if ( tree_1->lNode->mTokenType == SYMBOL ) {
        Node2 use = Watch_SYMBOL( tree_1->lNode, "" ) ;
        if ( use->mTokenType != SYMBOL ) {
          mSaveError.mType = ERROR_APPLY_NON_FUNCTION, mSaveError.mErrorTree = use ;
          throw mSaveError ; 
        } // if()
        else if ( SystemBound( use->mToken ) == true ) {
          if ( use->mark != 0 ) {
            mSaveError.mType = ERROR_APPLY_NON_FUNCTION, mSaveError.mErrorTree = use ;
            throw mSaveError ;              
          } // if() 
        } // else 
      } // else if()
      
      return tree_1 ;  
    } // Watch_Change_To_Project_Two_Type()
    
    int FindUserFunction( string use ) {
      int out = -1 ;
      for ( int i = 0; i < mUserFunction.size(); i++ ) {
        if ( mUserFunction[i].name == use ) out = i ;  
      } // for()
      
      return out ; 
    } // FindUserFunction()
    
    Node2 Change_To_Project_Two( Node2 tree_1 ) {
      Node2 out = new Node ;
      tree_1 = Watch_Change_To_Project_Two_Type( tree_1 ) ;
      if ( tree_1->mark == 50 )          
        out = WorkUserFunction( tree_1, "lambda expression" ) ;  
      else if ( tree_1->lNode->mTokenType != LEFT_PAREN ) {
        if ( tree_1->lNode->mTokenType != SYMBOL ) {
          mSaveError.mType = ERROR_APPLY_NON_FUNCTION, mSaveError.mErrorTree = tree_1->lNode ;
          throw mSaveError ;
        } // if()
        else {
          string save_name = tree_1->lNode->mToken ;
          tree_1->lNode = Watch_SYMBOL( tree_1->lNode, "" ) ;  
          if ( SystemBound( tree_1->lNode->mToken ) == false ) {
            if ( FindUserFunction( tree_1->lNode->mToken ) == -1 ) {
              mSaveError.mType = ERROR_UNBOUND_SYMBOL ;
              mSaveError.mErrorTree = tree_1->lNode, mSaveError.mErrorToken = tree_1->lNode->mToken ;
              throw mSaveError ;
            } // if()
            else out = WorkUserFunction( tree_1, save_name ) ;
          } // if()
          else {
            string name_2 = tree_1->lNode->mToken ;  
            if ( name_2 == "cons" || name_2 == "eqv?" || name_2 == "equal?" ) 
              out = WorkConsEqvEqual( tree_1, name_2, save_name ) ;
            else if ( name_2 == "list" || TokenAri( name_2 ) != 0 ) 
              out = WorkListAri( tree_1, name_2, save_name ) ;
            else if ( name_2 == "let" ) out = WorkLetLambda( tree_1, "let" ) ;
            else if ( name_2 == "lambda" ) out = WorkLetLambda( tree_1, "lambda" ) ;  
            else if ( name_2 == "quote" ) out = WorkQuote( tree_1, save_name ) ;
            else if ( name_2 == "car" || name_2 == "cdr" ) out = WorkCarCdr( tree_1, save_name ) ;
            else if ( name_2 == "define" ) WorkDefine( tree_1 ) ;
            else if ( name_2 == "clean-environment" ) WorkClean( tree_1 ) ;
            else if ( Work_Watch_Symbol_Type( name_2 ) ) out = WorkWatchType( tree_1, name_2, save_name ) ;
            else if ( name_2 == "cond" ) out = WorkCond( tree_1, name_2, save_name ) ;
            else if ( name_2 == "if" ) out = WorkIf( tree_1, name_2, save_name ) ; 
          } // else
        } // else
      } // if()
      
      return out ; 
    } // Change_To_Project_Two()
    
    bool Body_Equal( Node2 tree_1, Node2 thing ) {  
      vector< Bound_Symbol > a ;
      Node2 use = new Node, stree_1 = tree_1, sthing = thing ;
      Bound_Symbol save ;
      if ( thing->mTokenType != NIL ) {  
        while ( tree_1->mToken != ")" && thing->mToken != ")" ) { 
          save.mBoundThing = thing->lNode ;
          if ( tree_1->lNode->mToken == "(" ) {
            Error_Time_Read( tree_1->lNode->lNode->mToken ) ;  
            Node2 work = Change_To_Project_Two( tree_1->lNode ) ;
            use = work ;   
          } // if()
          else if ( tree_1->lNode->mTokenType == SYMBOL )
            use = Watch_SYMBOL( tree_1->lNode, "" ) ;
          else use = tree_1->lNode ; 
          save.mBoundNode = use ;
          a.push_back( save ) ;
          tree_1 = tree_1->rNode ;
          thing = thing->rNode ;
        } // while() 
      } // if()
      
      mAllUserBound.push_back( a ) ;
      if ( tree_1->mToken == ")" && thing->mToken == ")" ) {
        tree_1 = stree_1, thing = sthing ;
        return true ;  
      } // if()
      else if ( tree_1->mToken == ")" && thing->mTokenType == NIL ) return true ;
      else {
        tree_1 = stree_1, thing = sthing ;  
        return false ;
      } // else
    } // Body_Equal()
    
    void Copy_Tree( Node2 copy, Node2 tree_1, int left_right ) {
      if ( tree_1 != NULL ) {
        Node2 add = new Node ;
        add->lNode = NULL, add->rNode = NULL, add->mark = tree_1->mark, 
        add->mToken = tree_1->mToken, add->mTokenType = tree_1->mTokenType ;
        if ( left_right == 1 ) copy->lNode = add, copy = copy->lNode ;  
        else if ( left_right == 2 ) copy->rNode = add, copy = copy->rNode ;
        Copy_Tree( copy, tree_1->lNode, 1 ) ;
        Copy_Tree( copy, tree_1->rNode, 2 ) ; 
      } // if() 
    } // Copy_Tree()
    
    Node2 WorkUserFunction( Node2 tree_1, string save_name ) {
      int num_save = mAllUserBound.size() ;  
      Node2 out = new Node, thing = new Node, sthing = new Node ;
      Node2 function = new Node, sfunction = new Node ;
      function->lNode = NULL, function->rNode = NULL, function->mToken = "(", function->mTokenType = LEFT_PAREN ;
      //sthing->lNode = NULL, sthing->rNode = NULL, sthing->mToken = "(", sthing->mTokenType = LEFT_PAREN ;
      string s_name = tree_1->lNode->mToken ;
      bool watch_equal = false ;
      tree_1 = tree_1->rNode ;
      if ( s_name != "#<user function>" ) {
        thing = mUserFunction[ FindUserFunction( s_name ) ].mBoundFunctionThing, sthing = thing ;
        sfunction = mUserFunction[ FindUserFunction( s_name ) ].mBoundFunctionNode, Copy_Tree( function, sfunction, 0 ) ;  
      } // if()
      else 
        thing = mLambdaFunction.mBoundFunctionThing, function = mLambdaFunction.mBoundFunctionNode ;
      try { 
        watch_equal = Body_Equal( tree_1, thing ) ;
        if ( watch_equal == true ) { 
          if ( function->mToken == "(" ) {
            Error_Time_Read( function->lNode->mToken ) ;
            Node2 work = Change_To_Project_Two( function ) ;
            out = work ;  
          } // else
          else if ( function->mTokenType == SYMBOL ) out = Watch_SYMBOL( function, "" ) ;
          else out = function ; 
        } // if()
        else { 
          mSaveError.mType = ERROR_INVALID_ARG_NUMBER, mSaveError.mErrorToken = save_name ;
          throw mSaveError ;  
        } // else
      } // try
      catch ( EvalError t ) {  
        if ( mAllUserBound.size() == num_save + 1 ) mAllUserBound.pop_back() ;
        throw t ;  
      } // catch
      catch ( SyntaxError e ) {
        if ( mAllUserBound.size() == num_save + 1 ) mAllUserBound.pop_back() ;
        throw e ;  
      } // catch
      
      mAllUserBound.pop_back() ;
      if ( s_name != "#<user function>" ) {
        mUserFunction[ FindUserFunction( s_name ) ].mBoundFunctionThing = sthing ;
        // mUserFunction[ FindUserFunction( s_name ) ].mBoundFunctionNode = sfunction ; 
      } // if()
      else 
        mLambdaFunction.mBoundFunctionNode = new Node, mLambdaFunction.mBoundFunctionThing = new Node ;         
      return out ;   
    } // WorkUserFunction()
    
    int Find_Number_Error( Node2 tree_1, string which, string name ) {  
      int num = 0 ;
      Node2 save_node = tree_1 ;
      mSaveError.mType = ERROR_INVALID_ARG_NUMBER, tree_1 = tree_1->rNode ;
      for ( bool see_error = false ; tree_1 != NULL ; tree_1 = tree_1->rNode ) { 
        num++ ;
        if ( tree_1->mTokenType == RIGHT_PAREN ) {
          if ( which == "cons" || which == "eqv?" || which == "equal?" ) {
            if ( num != 3 ) see_error = true, mSaveError.mErrorToken = name ;
          } // if()
          else if ( which == "let" || which == "lambda" ) {
            if ( which == "let" )  which = "LET" ;
            if ( num < 3 )
              see_error = true, mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = which ;
          } // else if
          else if ( which == "cond_in_two" ) {
            if ( num == 1 ) { 
              see_error = true, mSaveError.mType = ERROR_COND, mSaveError.mErrorTree = save_node;  
            } // if()              
          } // else if
          else if ( which == "list" || which == "begin" || which == "cond" || which == "cond_in" ) {
            if ( num == 1 ) {  
              if ( which != "cond_in" ) see_error = true, mSaveError.mErrorToken = name ;
              else see_error = true, mSaveError.mType = ERROR_COND, mSaveError.mErrorTree = save_node;  
            } // if() 
          } // else if
          else if ( which == "define" ) {
            if ( num != 3 ) 
              mSaveError.mType = ERROR_FORMAT, see_error = true, mSaveError.mErrorToken = "define" ;
          } // else if
          else if ( which == "car" || which == "cdr" || which == "quote" || 
                    Work_Watch_Symbol_Type( which ) || which == "not" ) {
            if ( num != 2 ) see_error = true, mSaveError.mErrorToken = name ;
          } // else if
          else if ( TokenAri( which ) != 0 ) {
            if ( num == 1 || num == 2 ) see_error = true, mSaveError.mErrorToken = name ;
          } // else if
          else if ( which == "clean-environment" ) { 
            if ( num != 1 ) see_error = true, mSaveError.mErrorToken = "clean-environment" ;
          } // else if
          else if ( which == "if" ) { 
            if ( num != 4 ) see_error = true, mSaveError.mErrorToken = name ;  
          } // else if
          
          if ( see_error == true ) throw mSaveError ; 
        } // if()
      } // for() 
      
      return num ;
    } // Find_Number_Error()
    
    int Find_User_Symbol( string use ) {
      int out = -1 ;
      vector < Bound_Symbol > save ;
      if ( mAllUserBound.size() != 0 ) {
        save = mAllUserBound.back() ;
        for ( int i = 0; i < save.size() ; i++ ) {
          if ( save[i].mBoundThing->mToken == use ) out = i ; 
        } // for
      } // if()
      return out ;
    } // Find_User_Symbol()
    
    int Find_Bound_Symbol( string use ) {
      int out = -1 ;  
      mget_user_num = false ;
      mget_let_num = false ;
      
      int save_L = Find_Let_Symbol( use ) ;
      if ( save_L != -1 ) out = save_L, mget_let_num = true ;
      int save_U = Find_User_Symbol( use ) ;
      if ( save_U != -1 ) out = save_U, mget_user_num = true ;
      if ( mget_user_num == false && mget_let_num == false ) {
        for ( int i = 0 ; i < mUserBound.size() ; i++ )
          if ( mUserBound[i].mBoundThing->mToken == use ) out = i ;
      } // if()
      /*
      if ( mget_user_num == true ) cout << "1" ;
      else if ( mget_let_num == true ) cout << "2" ;
      else cout << "3" ;
      */
      return out ;
    } // Find_Bound_Symbol()
    
    Node2 Find_Bound_Node( int number ) {
      if ( mget_let_num == true ) return mLetUserBound[number].mBoundNode ;
      else if ( mget_user_num == true ) return mAllUserBound.back()[number].mBoundNode ;
      else return mUserBound[number].mBoundNode ;    
    } // Find_Bound_Node()
    
    int Find_Bound_Memory( int number ) {
      return mUserBound[number].mlocation ;    
    } // Find_Bound_Memory()
    
    int Find_Let_Symbol( string use ) {
      int out = -1 ;  
      for ( int j = 0 ; j < mLetUserBound.size() ; j++ ) {
        if ( mLetUserBound[j].mBoundThing->mToken == use ) out = j ;  
      } // for()
      
      return out ; 
    } // Find_Let_Symbol()
    
    Node2 Help_let( Node2 tree_1 ) {
      Node2 save = new Node ;
      if ( tree_1->mTokenType == SYMBOL ) save = Watch_SYMBOL( tree_1, "" ) ;  
      else if ( tree_1->mTokenType == LEFT_PAREN ) {
        Error_Time_Read( tree_1->lNode->mToken ) ;
        save = Change_To_Project_Two( tree_1 ) ;  
      } // else if
      else save = tree_1 ; 
      return save ;  
    } // Help_let()
    
    bool Check( Node2 tree_1, string which ) {
      for ( ; tree_1->mToken != ")" ; tree_1 = tree_1->rNode ) {
        if ( which == "let" ) {
          bool see = false ;  
          if ( tree_1->lNode->mToken == "(" ) {
            Bound_Symbol add ;
            if ( tree_1->lNode->lNode->mTokenType == SYMBOL ) {
              if ( tree_1->lNode->rNode->rNode->mToken != ")" )
                mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = "LET", see = true ;
            } // if()
            else mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = "LET", see = true ; 
          } // if() 
          else mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = "LET", see = true ;
          if ( see == true ) throw mSaveError ; 
        } // if()
        else {
          if ( tree_1->lNode->mTokenType != SYMBOL ) {
            mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = "lambda" ;
            throw mSaveError ;              
          } // if()
        } // else
      } // for()
      
      return false ;
    } // Check()
    
    Node2 WorkLetLambda( Node2 tree_1, string work_type ) {
      Find_Number_Error( tree_1, tree_1->lNode->mToken , "" ) ;
      Node2 use = NULL, save = new Node ;
      int num = 1 ;
      try {
        for( tree_1 = tree_1->rNode ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
          num++ ;  
          if ( tree_1->mToken == "(" ) {
            if ( tree_1->lNode->mTokenType == SYMBOL ) {   
              if ( num == 2 ) { 
                mSaveError.mType = ERROR_FORMAT ;
                if ( work_type == "let" )  mSaveError.mErrorToken = "LET" ;
                else if ( work_type == "lambda" ) mSaveError.mErrorToken = "lambda" ;
                throw mSaveError ;              
              } // if()
              else if ( work_type == "let" )
                save = Watch_SYMBOL( tree_1->lNode, "" ) ;
              else save = tree_1->lNode ;             
            } // if()
            else if ( tree_1->lNode->mTokenType == LEFT_PAREN ) {
              Error_Time_Read( tree_1->lNode->lNode->mToken ) ;  
              if ( num == 2 ) {                
                if ( work_type == "let" ) {
                  bool see = Check( tree_1->lNode, work_type ) ;
                  if ( see == false ) {
                    Node2 save_tree = tree_1 ;  
                    for ( tree_1 = tree_1->lNode ; tree_1->mToken != ")" ; tree_1 = tree_1->rNode ) {
                      Bound_Symbol add ;
                      int let_num = Find_Let_Symbol( tree_1->lNode->lNode->mToken ) ;
                      if ( let_num == -1 ) {
                        add.mBoundNode = Help_let( tree_1->lNode->rNode->lNode ) ; 
                        add.mBoundThing = tree_1->lNode->lNode ;
                        mLetUserBound.push_back( add ) ; 
                      } // if()
                      else  mLetUserBound[let_num].mBoundNode = Help_let( tree_1->lNode->rNode->lNode ) ;
                    } // while()
                  
                    tree_1 = save_tree ; 
                  } // if() 
                } // if()
                else {
                  bool see = Check( tree_1->lNode, work_type ) ;
                  if ( see == false ) mLambdaFunction.mBoundFunctionThing = tree_1->lNode ;   
                } // else
              } // if() 
              else if ( work_type == "let" ) save = Change_To_Project_Two( tree_1->lNode ) ;
              else save = tree_1->lNode ;      
            } // else if()
            else {
              if ( num == 2 ) {
                if ( tree_1->lNode->mToken != "nil" ) {
                  mSaveError.mType = ERROR_FORMAT ;
                  if ( work_type == "let" )  mSaveError.mErrorToken = "LET" ;
                  else if ( work_type == "lambda" ) mSaveError.mErrorToken = "lambda" ;
                  throw mSaveError ;
                } // if()
                else {
                  if ( work_type != "let" ) 
                    mLambdaFunction.mBoundFunctionThing = tree_1->lNode ; 
                } // else
              } // if()
              else save = tree_1->lNode ;   
            } // else          
          } // if()
        } // for()
      } // try
      catch ( EvalError t ) {
        mLetUserBound.clear() ;
        throw t ;  
      } // catch
      catch ( SyntaxError e ) {
        mLetUserBound.clear() ;
        throw e ;  
      } // catch
       
      if ( work_type == "let" ) mLetUserBound.clear() ;
      else {
        mLambdaFunction.mBoundFunctionNode = save ;
        Node2 put = new Node ;
        put->lNode = NULL, put->rNode = NULL ;
        put->mToken = "#<user function>", put->mTokenType = SYMBOL ;
        return put ; 
      } // else
      
      return save ;      
    } // WorkLetLambda()
    
    Node2 Watch_SYMBOL( Node2 put, string which ) { 
      int save_number = Find_Bound_Symbol( put->mToken ) ; 
      if ( save_number == -1 ) {
        if ( SystemBound( put->mToken ) == false ) {  
          mSaveError.mType = ERROR_UNBOUND_SYMBOL, mSaveError.mErrorToken = put->mToken ;   
          throw mSaveError ; 
        } // if()
        else return put ; 
      } // if()
      else return Find_Bound_Node( save_number ) ;
    } // Watch_SYMBOL()
    
    Node2 WorkCondTwo( Node2 tree_1, string name ) {       
      if ( name == "cond" ) Find_Number_Error( tree_1, "cond_in_two", "cond_in_two" ) ;  
      int num = 1 ;
      bool see = false ;
      Node2 save_one = new Node ;
      if ( true ) {
        for( ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
          num++ ;
          if ( tree_1->mToken == "(" ) {
            if ( tree_1->lNode->mTokenType == SYMBOL ) { 
              tree_1->lNode = Watch_SYMBOL( tree_1->lNode, "" ) ; 
              
              if ( num == 2 ) { 
                if ( tree_1->lNode->mTokenType != NIL || SystemBound( tree_1->lNode->mToken ) ) see = true ;
              } // if()
              else {
                save_one = tree_1->lNode ;
                if ( name == "if" ) return save_one ;                  
              } // else
            } // if()
            else if ( tree_1->lNode->mTokenType == LEFT_PAREN ) {
              Error_Time_Read( tree_1->lNode->lNode->mToken ),
              tree_1->lNode = Change_To_Project_Two( tree_1->lNode ) ;  
              if ( num == 2 ) {
                if ( tree_1->lNode->mTokenType != NIL ) see = true ;
              } // if()
              else {
                save_one = tree_1->lNode ;
                if ( name == "if" ) return save_one ;                
              } // else 
            } // else if
            else {
              if ( num == 2 ) {
                if ( tree_1->lNode->mTokenType != NIL ) see = true ;   
              } // if()
              else {
                save_one = tree_1->lNode ;
                if ( name == "if" ) return save_one ;
              } // else 
            } // else
          } // if()
        
          if ( name == "cond" ) {
            if ( see == false ) return NULL ;
          } // if()
          else if ( name == "if" && see == false ) tree_1 = tree_1->rNode ;
        } // for()
      } // if()
      
      return save_one ;        
    } // WorkCondTwo() 
    
    void Find_Cond_Type_ERROR( Node2 tree_1 ) {
      for( tree_1 = tree_1->rNode ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
        if ( tree_1->mToken == "(" ) {
          if ( tree_1->lNode->mTokenType != LEFT_PAREN ) {
            mSaveError.mType = ERROR_COND, mSaveError.mErrorTree = tree_1->lNode ;
            throw mSaveError ;  
          } // if()
          else {
            Error_Time_Read( tree_1->lNode->lNode->mToken ) ;
            Find_Number_Error( tree_1->lNode, "cond_in", "cond_in" ) ;  
          } // else 
        } // if() 
      } // for() 
    } // Find_Cond_Type_ERROR()
    
    Node2 WorkCond( Node2 tree_1, string work_type , string name ) {
      Find_Number_Error( tree_1, tree_1->lNode->mToken, name ) ;
      Find_Cond_Type_ERROR( tree_1 ) ;
      int num = 1 ;
      Node2 out = NULL, save_one = new Node ;
      bool stop = false, see_error = false ;
      for( tree_1 = tree_1->rNode ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
        num++ ;
        if ( tree_1->mToken == "(" ) {
          if ( tree_1->lNode->mTokenType == LEFT_PAREN ) {
            if ( see_error == false ) {
              Error_Time_Read( tree_1->lNode->lNode->mToken ) ;
              Find_Number_Error( tree_1->lNode, "cond_in", "cond_in" ) ;
              if ( stop == false ) save_one = WorkCondTwo( tree_1->lNode, "cond" ) ;
              if ( save_one != NULL && stop == false ) out = save_one, stop = true ;
            } // if()
          } // if()
        } // if()
        else { 
          if ( out == NULL ) {
            mSaveError.mType = ERROR_NO_RETURN_VALUE ;
            throw mSaveError ;  
          } // if()
        } // else 
      } // for() 
      
      return out ; 
    } // WorkCond() 
    
    Node2 WorkIf( Node2 tree_1, string work_type , string name ) {
      Find_Number_Error( tree_1, tree_1->lNode->mToken, name ) ;
      tree_1 = tree_1->rNode ;
      return WorkCondTwo( tree_1, "if" ) ;  
    } // WorkIf()    
    
    Node2 WorkWatchType( Node2 tree_1, string work_type , string name ) {
      Find_Number_Error( tree_1, tree_1->lNode->mToken , name ) ;
      TokenType use_Type ;
      int num = 1 ;
      Node2 out = new Node ;
      out->lNode = NULL, out->rNode = NULL, out->mToken = "#t", out->mTokenType = T ; 
      for( tree_1 = tree_1->rNode ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
        num++ ;
        if ( tree_1->mToken == "(" ) {
          if ( tree_1->lNode->mTokenType == SYMBOL ) {
            tree_1->lNode = Watch_SYMBOL( tree_1->lNode, "" ) ;
            use_Type = tree_1->lNode->mTokenType ;
          } // if()
          else if ( tree_1->lNode->mTokenType == LEFT_PAREN )
            Error_Time_Read( tree_1->lNode->lNode->mToken ), 
            use_Type = Change_To_Project_Two( tree_1->lNode )->mTokenType ;
          else use_Type = tree_1->lNode->mTokenType ;
        } // if() 
      } // for()
    
      if ( work_type == "symbol?" && use_Type == SYMBOL ) return out ;
      else if ( work_type == "boolean?" && use_Type == T ) return out ;
      else if ( work_type == "boolean?" && use_Type == NIL ) return out ;
      else if ( work_type == "string?" && use_Type == STRING ) return out ;
      else if ( work_type == "number?" && use_Type == INT ) return out ;
      else if ( work_type == "number?" && use_Type == FLOAT ) return out ;
      else if ( work_type == "real?" && use_Type == FLOAT ) return out ;
      else if ( work_type == "integer?" && use_Type == INT ) return out ;
      else if ( work_type == "null?" && use_Type == NIL ) return out ;
      else if ( work_type == "pair?" && use_Type == LEFT_PAREN ) return out ;
      else out->mToken = "nil", out->mTokenType = NIL ;
      return out ;
    } // WorkWatchType()
    
    Node2 WorkConsEqvEqual( Node2 tree_1, string work_type, string name ) {
      Find_Number_Error( tree_1, work_type , name ) ;
      Node2 out = new Node ;
      int num = 1, e = 0 ;
      int mem_one = 0, mem_two = -1 ;
      out->mToken = "(", out->mTokenType = LEFT_PAREN ;
      for( tree_1 = tree_1->rNode ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
        num++ ;  
        if ( tree_1->mToken == "(" ) {
          if ( tree_1->lNode->mTokenType == SYMBOL ) {
            if ( work_type == "cons" ) {
              if ( num == 2 ) out->lNode = Watch_SYMBOL( tree_1->lNode, "" ) ;
              else if ( num == 3 ) out->rNode = Watch_SYMBOL( tree_1->lNode, "" ) ; 
            } // if()
            else {
              if ( num == 2 ) {  
                out->lNode = Watch_SYMBOL( tree_1->lNode, "" ), e = Find_Bound_Symbol( tree_1->lNode->mToken ) ;
                if ( e != -1 ) mem_one = Find_Bound_Memory( e ) ;
              } // if()
              else if ( num == 3 ) {  
                out->rNode = Watch_SYMBOL( tree_1->lNode, "" ), e = Find_Bound_Symbol( tree_1->lNode->mToken ) ;
                if ( e != -1 ) mem_two = Find_Bound_Memory( e ) ;
              } // if()  
            } // else
          } // if()
          else if ( tree_1->lNode->mTokenType == LEFT_PAREN ) {
            Error_Time_Read( tree_1->lNode->lNode->mToken ) ;
            if ( num == 2 ) out->lNode = Change_To_Project_Two( tree_1->lNode ) ;
            else if ( num == 3 ) out->rNode = Change_To_Project_Two( tree_1->lNode ) ;
          } // else if()
          else {  
            if ( num == 2 ) out->lNode = tree_1->lNode ;
            else if ( num == 3 ) out->rNode = tree_1->lNode ;
          } // else
        } // if()
      } // for()
      
      if ( work_type != "cons" ) {
        if ( work_type == "equal?" ) {
          try {
            WorkEqual( out->rNode, out->lNode ) ;
            out->lNode = NULL, out->rNode = NULL, out->mToken = "#t", out->mTokenType = T ;  
          } // try()            
          catch ( bool q ) {
            out->lNode = NULL, out->rNode = NULL, out->mToken = "nil", out->mTokenType = NIL ;  
          } // catch
        } // if()
        else {
          if ( mem_one == mem_two ) 
            out->lNode = NULL, out->rNode = NULL, out->mToken = "#t", out->mTokenType = T ;
          else if ( out->rNode->mTokenType == out->lNode->mTokenType ) {
            if ( out->rNode->mTokenType == STRING || out->rNode->mToken == "(" )
              out->lNode = NULL, out->rNode = NULL, out->mToken = "nil", out->mTokenType = NIL ;
            else {
              if ( out->rNode->mToken == out->lNode->mToken )
                out->lNode = NULL, out->rNode = NULL, out->mToken = "#t", out->mTokenType = T ;
              else 
                out->lNode = NULL, out->rNode = NULL, out->mToken = "nil", out->mTokenType = NIL ;    
            } // else 
          } // else if()
          else
            out->lNode = NULL, out->rNode = NULL, out->mToken = "nil", out->mTokenType = NIL ;   
        } // else
      } // if()
      
      return out ; 
    } // WorkConsEqvEqual()
    
    void WorkEqual( Node2 one, Node2 two ) {
      if ( one != NULL && two != NULL ) {
        if ( one->mToken != two->mToken ) throw false ;
        WorkEqual( one->lNode, two->lNode ), WorkEqual( one->rNode, two->rNode ) ; 
      } // if()
      else if ( one == NULL && two != NULL ) throw false ;
      else if ( one != NULL && two == NULL ) throw false ;
    } // WorkEqual()
    
    void CatchError( Node2 e, string e_2 ) {
      mSaveError.mType = ERROR_INVALID_ARG_TYPE, mSaveError.mErrorToken = e_2 , 
      mSaveError.mErrorTree = e, throw mSaveError ; 
    } // CatchError()
    
    vector< Node2 > HelpAri( string work_type, TokenType a, Node2 save, vector< Node2 > work_thing ) {
      if ( TokenAri( work_type ) == 4 || save->mToken != "(" ) {
        if ( TokenAri( work_type ) == 1 ) {
          if ( a == INT || a == FLOAT ) work_thing.push_back( save ) ;
          else CatchError( save, work_type ) ;
        } // if()
        else if ( TokenAri( work_type ) == 2 ) {
          if ( a == STRING ) work_thing.push_back( save ) ;
          else CatchError( save, work_type ) ;                      
        } // else if()
        else if ( TokenAri( work_type ) == 3 || TokenAri( work_type ) == 4 ) 
          work_thing.push_back( save ) ;
      } // if()
      else CatchError( save, work_type ) ;
      return work_thing ;        
    } // HelpAri()
    
    Node2 WorkListAri( Node2 tree_1, string work_type, string save_name ) {
      int total = Find_Number_Error( tree_1, work_type, save_name ) ;
      Node2 save = new Node, out = save, save2 = new Node ;
      save->mToken = "(", save->mTokenType = LEFT_PAREN, save->lNode = NULL, save->rNode = NULL ;
      int num = 1 ;
      vector < Node2 > work_thing ;
      for ( tree_1 = tree_1->rNode ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
        num++ ;
        if ( tree_1->mToken == "(" ) {
          if ( tree_1->lNode->mTokenType == SYMBOL ) {            
            save->lNode = Watch_SYMBOL( tree_1->lNode, "" ) ;
            if ( work_type != "list" ) {
              if ( SystemBound( save->lNode->mToken ) ) {
                if ( TokenAri( work_type ) == 1 || TokenAri( work_type ) == 2 ) 
                  CatchError( save->lNode, work_type ) ;
                else if ( TokenAri( work_type ) == 3 || TokenAri( work_type ) == 4 ) 
                  work_thing.push_back( save->lNode ) ;
              } // if()
              else {
                TokenType a = save->lNode->mTokenType ;
                work_thing = HelpAri( work_type, a, save->lNode, work_thing ) ;
              } // else 
            } // if()
          } // if()
          else if ( tree_1->lNode->mTokenType == LEFT_PAREN ) {
            Error_Time_Read( tree_1->lNode->lNode->mToken ) ; 
            save->lNode = Change_To_Project_Two( tree_1->lNode ) ;
            TokenType a = save->lNode->mTokenType ;
            if ( work_type != "list" ) work_thing = HelpAri( work_type, a, save->lNode, work_thing ) ;  
          } // else if()
          else {         
            save->lNode = tree_1->lNode ;
            TokenType a = save->lNode->mTokenType ;
            if ( work_type != "list" ) work_thing = HelpAri( work_type, a, save->lNode, work_thing ) ;
          } // else
          
          if ( work_type == "list" ) {
            if ( total != num ) {  
              save2 = new Node, save2->mToken = "(", save2->mTokenType = LEFT_PAREN, save2->lNode = NULL ;
              save2->rNode = NULL, save->rNode = save2, save = save->rNode ;
            } // if()
          } // if()
        } // if()
        else {
          if ( work_type == "list" ) {  
            save2 = new Node, save2->mToken = tree_1->mToken, save2->mTokenType = tree_1->mTokenType ;
            save2->lNode = NULL, save2->rNode = NULL, save->rNode = save2 ; 
          } // if()
        } // else         
      } // for()
      
      if ( work_type != "list" ) {
        bool float_int = false ;
        float float_num = 0 ;
        out = new Node, out->lNode = NULL, out->rNode = NULL, out->mToken = "#t", out->mTokenType = T ;  
        if ( work_type == "+" || work_type == "-" || work_type == "*" || work_type == "/" ) {
          char temp[100] ; 
          for ( int j = 0 ; j < work_thing.size() ; j++ ) {
            if ( work_thing[j]->mTokenType == FLOAT ) float_int = true ; 
          } // for()
          
          float_num = atof( work_thing[0]->mToken.c_str() ) ;
          for ( int j = 1 ; j < work_thing.size() ; j++ ) {
            if ( work_type == "+" ) float_num += atof( work_thing[j]->mToken.c_str() ) ;
            else if ( work_type == "-" ) float_num -= atof( work_thing[j]->mToken.c_str() ) ;
            else if ( work_type == "*" ) float_num *= atof( work_thing[j]->mToken.c_str() ) ;
            else if ( work_type == "/" ) float_num /= atof( work_thing[j]->mToken.c_str() ) ;
          } // for()
          
          if ( float_int == false ) {
             int i = (int) float_num ;  
            sprintf( temp, "%d", i ) ;
            string put( temp ) ;
            out->mToken = put, out->mTokenType = INT ;  
          } // if()
          else {
            sprintf( temp, "%.3f", float_num ) ;
            string put( temp ) ;
            out->mToken = put, out->mTokenType = FLOAT ;              
          } // else 
        } // if()
        else if ( work_type == "begin" )
          return work_thing[work_thing.size() - 1] ; 
        else if ( work_type == "not" ) {
          if ( work_thing[0]->mTokenType != NIL ) out->mToken = "nil", out->mTokenType = NIL ; 
        } // else if
        else if ( work_type == "and" || work_type == "or" ) {
          bool use_or = false, error_1 = false ;  
          for ( int j = 0 ; j < work_thing.size() && error_1 == false ; j++ ) {
            if ( work_thing[j]->mTokenType == NIL ) {
              if ( work_type == "and" ) out->mToken = "nil", out->mTokenType = NIL, error_1 = true ;
            } // if()
            
            if ( work_thing[j]->mTokenType != NIL ) {
              if ( work_type == "or" ) use_or = true, error_1 = true ;  
            } // if()
          } // for()
          
          if ( work_type == "or" ) {
            if ( use_or == false ) out->mToken = "nil", out->mTokenType = NIL ; 
          } // if() 
        } // else if()
        else if ( work_type == "string-append" || work_type == "string>?" ) {
          string get_string = "", use_one = "" ;
          int length_save = work_thing[0]->mToken.length(), j = 0 ;
          bool error = false ;
          if ( work_type == "string>?" ) 
            use_one = work_thing[0]->mToken.substr( 1, length_save - 2 ), j = 1 ;
          for ( ; j < work_thing.size() && error == false ; j++ ) {
            length_save = work_thing[j]->mToken.length() ; 
            if ( work_type == "string>?" ) {
              get_string = work_thing[j]->mToken.substr( 1, length_save - 2 ) ;  
              if ( use_one <= get_string ) error = true ;
              use_one = get_string ;  
            } // if()
            else
              get_string += work_thing[j]->mToken.substr( 1, length_save - 2 ) ;  
          } // for
          
          if ( work_type == "string-append" ) 
            out->mToken = "\"" + get_string + "\"", out->mTokenType = STRING ;
          else {
            if ( error == true ) out->mToken = "nil", out->mTokenType = NIL ;  
          } // else   
        } // else if()
        else if ( work_type == ">" || work_type == "<" || work_type == "<=" || 
                  work_type == ">=" || work_type == "=" ) {
          bool error_1 = false ;  
          float_num = atof( work_thing[0]->mToken.c_str() ) ;
          for ( int j = 1 ; j < work_thing.size() && error_1 == false ; j++ ) {
            if ( work_type == "<" ) {
              if ( float_num >= atof( work_thing[j]->mToken.c_str() ) ) error_1 = true ; 
            } // if()
            else if ( work_type == ">" ) {
              if ( float_num <= atof( work_thing[j]->mToken.c_str() ) ) error_1 = true ; 
            } // else if()
            else if ( work_type == "=" ) {
              if ( float_num != atof( work_thing[j]->mToken.c_str() ) ) error_1 = true ; 
            } // else if()
            else if ( work_type == "<=" ) {
              if ( float_num > atof( work_thing[j]->mToken.c_str() ) ) error_1 = true ;
            } // else if()
            else if ( work_type == ">=" ) {
              if ( float_num < atof( work_thing[j]->mToken.c_str() ) ) error_1 = true ; 
            } // else if()
            float_num = atof( work_thing[j]->mToken.c_str() ) ;
          } // for()
         
          if ( error_1 == true ) out->mToken = "nil", out->mTokenType = NIL ;            
        } // else if()
      } // if()
      
      return out ;
    } // WorkListAri()
    
    void MarkTree( Node2 use ) {
      if ( use != NULL )
        use->mark = 1, MarkTree( use->lNode ), MarkTree( use->rNode ) ; 
    } // MarkTree()
    
    Node2 WorkQuote( Node2 tree_1, string name ) {
      Find_Number_Error( tree_1, "quote" , name ), tree_1 = tree_1->rNode ;
      Node2 save = tree_1->lNode ;
      if ( save->mToken == "(" ) MarkTree( save ) ;
      else save->mark = 1 ;
      return save ;
    } // WorkQuote()
    
    Node2 WorkCarCdr( Node2 tree_1, string save_name ) {
      string Fun_name = tree_1->lNode->mToken ;
      Find_Number_Error( tree_1, tree_1->lNode->mToken, save_name ), tree_1 = tree_1->rNode ;
      Node2 save = new Node ;
      for( int num = 1 ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
        num++ ;
        if ( tree_1->mToken == "(" ) {
          if ( tree_1->lNode->mTokenType == SYMBOL ) 
            save = Watch_SYMBOL( tree_1->lNode, "" ) ;              
          else if ( tree_1->lNode->mTokenType == LEFT_PAREN ) {
            Error_Time_Read( tree_1->lNode->lNode->mToken ) ; 
            save = Change_To_Project_Two( tree_1->lNode ) ;        
          } // else if()
          else {
            mSaveError.mType = ERROR_INVALID_ARG_TYPE, mSaveError.mErrorTree = tree_1->lNode ;
            mSaveError.mErrorToken = Fun_name ;
            throw mSaveError ; 
          } // else
        } // if() 
      } // for() 
      
      if ( save->mToken == "(" ) {
        if ( Fun_name == "car" ) save = save->lNode ;
        else if ( Fun_name == "cdr" ) {
          if ( save->rNode->mToken != ")" ) save = save->rNode ;
          else {
            Node2 a = new Node ;
            a->mToken = "nil", a->mTokenType = NIL, a->lNode = NULL, a->rNode = NULL, save = a ;
          } // else
        } // else if
      } // if()
      else {
        mSaveError.mType = ERROR_INVALID_ARG_TYPE, mSaveError.mErrorTree = save ;
        mSaveError.mErrorToken = Fun_name ;
        throw mSaveError ;         
      } // else
      
      return save ;
    } // WorkCarCdr()
    
    void WorkClean( Node2 tree_1 ) {
      Find_Number_Error( tree_1, "clean-environment" , "clean-environment" ) ;
      mUserBound.clear() ;
      string out = "clean-environment" ; 
      throw out ;    
    } // WorkClean()
    
    void Clean_already_define_function( string use ) {
      int i = -1 ;
      for ( int j = 0 ; j < mUserFunction.size() ; j++ ) {
        if ( mUserFunction[j].name == use ) i = j ;  
      } // for()
      
      if ( i != -1 ) mUserFunction.erase( mUserFunction.begin() + i ) ; 
    } // Clean_already_define_function()
    
    void WorkDefine( Node2 tree_1 ) {
      Find_Number_Error( tree_1, "define" , "define" ) ;
      Node2 use = NULL ;
      Bound_Symbol add ;
      Bound_Function add_function ;
      string out = "" ;
      int num = 1, vector_num = -1, memory = 0 ;
      bool Already_bound = false, find_define_function = false ;
      tree_1 = tree_1->rNode ;
      for( num = 1 ; tree_1 != NULL ; tree_1 = tree_1->rNode ) {
        num++ ;  
        if ( tree_1->mToken == "(" ) {
          if ( tree_1->lNode->mTokenType == SYMBOL ) {
            int e = Find_Bound_Symbol( tree_1->lNode->mToken ) ;   
            if ( num == 2 ) { 
              if ( e == -1 ) {
                if ( SystemBound( tree_1->lNode->mToken ) == true ) {
                  mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = "define" ;
                  throw mSaveError ; 
                } // if()
                else  add.mBoundThing = tree_1->lNode ; 
              } // if()
              else  vector_num = e ;
            } // if()
            else if ( num == 3 ) {
              if ( find_define_function == true )  
                add_function.mBoundFunctionNode = tree_1->lNode ; 
              else if ( e != -1 ) {
                memory = mUserBound[e].mlocation, tree_1->lNode = Find_Bound_Node( e ) ; 
                if ( vector_num == -1 ) add.mBoundNode = tree_1->lNode ;
                else use = tree_1->lNode, Already_bound = true ;
              } // else if()
              else {
                if ( SystemBound( tree_1->lNode->mToken ) == false ) {
                  mSaveError.mType = ERROR_UNBOUND_SYMBOL, mSaveError.mErrorToken = tree_1->lNode->mToken ;
                  throw mSaveError ;                    
                } // if()
                else {
                  if ( vector_num == -1 ) add.mBoundNode = tree_1->lNode ;
                  else use = tree_1->lNode, Already_bound = true ;                    
                } // else
              } // else               
            } // else if()           
          } // if()
          else if ( tree_1->lNode->mTokenType == LEFT_PAREN ) {
            Error_Time_Read( tree_1->lNode->lNode->mToken ) ;  
            if ( num == 2 ) {                
              if ( SystemBound( tree_1->lNode->lNode->mToken ) == false ) {  
                int save_number = Find_Bound_Symbol( tree_1->lNode->lNode->mToken ) ;
                if ( save_number == -1 ) add.mBoundThing = tree_1->lNode->lNode ;
                else vector_num = save_number ;
                add_function.name = tree_1->lNode->lNode->mToken ;
                add_function.mBoundFunctionThing = tree_1->lNode->rNode ;
                find_define_function = true ;  
              } // if()
              else {
                mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = "define" ;
                throw mSaveError ;  
              } // else
            } // if() 
            else if ( num == 3 ) {
              if ( find_define_function == false ) {
                Node2 Work = Change_To_Project_Two( tree_1->lNode ) ;
                if ( vector_num == -1 ) {
                  if ( Work->mToken != "#<user function>" ) add.mBoundNode = Work ;
                  else {
                    add_function.name = add.mBoundThing->mToken ;
                    add_function.mBoundFunctionThing = mLambdaFunction.mBoundFunctionThing ;
                    add_function.mBoundFunctionNode = mLambdaFunction.mBoundFunctionNode ;
                    find_define_function = true ;  
                  } // else
                } // if()
                else {
                  Already_bound = true ;
                  if ( Work->mToken != "#<user function>" ) use = Work ;
                  else {
                    add_function.name = mUserBound[vector_num].mBoundThing->mToken ;
                    add_function.mBoundFunctionThing = mLambdaFunction.mBoundFunctionThing ;
                    add_function.mBoundFunctionNode = mLambdaFunction.mBoundFunctionNode ;
                    find_define_function = true ;  
                  } // else
                } // else 
              } // if()
              else add_function.mBoundFunctionNode = tree_1->lNode ;
            } // else if()       
          } // else if()
          else {
            if ( num == 2 ) {
              mSaveError.mType = ERROR_FORMAT, mSaveError.mErrorToken = "define" ;
              throw mSaveError ;                
            } // if()
            else if ( num == 3 ) {
              if ( find_define_function == false ) {  
                if ( vector_num == -1 ) add.mBoundNode = tree_1->lNode ;
                else  Already_bound = true, use = tree_1->lNode ;
              } // if()
              else add_function.mBoundFunctionNode = tree_1->lNode ;
            } // else if    
          } // else
          
          if ( num == 3 ) {
            if ( find_define_function == true ) {
              if ( vector_num == -1 ) {
                add.mBoundNode = add.mBoundThing ;
                add.mBoundNode->mark = 2 ;   
              } // if()
              else {  
                Already_bound = true ;
                use = mUserBound[vector_num].mBoundThing, use->mark = 2 ;  
              } // else
              Clean_already_define_function( add_function.name ) ;
              mUserFunction.push_back( add_function ) ;
            } // if()
             
            if ( vector_num == -1 ) out = add.mBoundThing->mToken ;
            else  out = mUserBound[vector_num].mBoundThing->mToken ;
          } // if()
        } // if()
      } // for()

      if ( Already_bound == true ) {  
        if ( memory != 0 ) mUserBound[vector_num].mlocation = memory ;
        else mUserBound[vector_num].mlocation = memory + 1 ;
        mUserBound[vector_num].mBoundNode = use ;
        throw out ; 
      } // if()
      else { 
        if ( memory == 0 ) add.mlocation = mUserBound.size() + 1 ;
        else add.mlocation = memory ;
        mUserBound.push_back( add ) ;
        throw out ;          
      } // else    
    } // WorkDefine()
};
/*
struct EvalError {
  ErrorType mType ;
  string mErrorToken ;
  Node2 mErrorTree ;
};
enum ErrorType { 
                 ERROR_NON_LIST
};
*/ 
int main() {
  MyTree hello ;    
  cout << "Welcome to OurScheme!\n\n> " ;
  while (true) {
    try {
      hello.GetMyTree() ;
    } // try 
    catch ( SyntaxError e ) {
      int save_column = 0 ;  
      if ( watch_if_add_one == false ) save_column = e.mColumn ;
      else save_column = e.mColumn + 1, watch_if_add_one = false ; 
      if ( e.mType == ERROR_EOF ) { 
        cout << "ERROR : END-OF-FILE encountered when there should be more input\n\n"
             << "Thanks for using OurScheme!" ;
        return 0;
      } // if()
      else if ( e.mType == ERROR_LINE_ENTER )
        cout << "ERROR (unexpected character) : line " << e.mLine << " column " << save_column
             << " LINE-ENTER encountered" ;
      else if ( e.mType == ERROR_UNEXPECTED )
        cout << "ERROR (unexpected character) : line " << e.mLine << " column " << save_column
             << " character '" << e.mChar << "'" ;
             
      if ( e.mType == ERROR_UNEXPECTED ) {
        try {
          hello.mtoken.ReadJunk() ; 
        } // try
        catch( SyntaxError e ) {
          cout << "\n\n> ERROR : END-OF-FILE encountered when there should be more input\n\n"
               << "Thanks for using OurScheme!" ;
          return 0;            
        } // catch()
      } // if()
    } // catch
    catch ( string p ) {
      if ( p == "clean-environment" ) cout << "environment cleaned" ;
      else cout << p << " defined" ;
    } // catch
    catch ( TreeTypeNode e ) {
      if ( e.mTree_type == ATOM_ONE )
        cout << e.mTree->mToken ; 
      else if ( e.mTree_type == DOTTED_PAIR || e.mTree_type == LIST || e.mTree_type == ATOM_TWO )
        hello.CoutTree( e.mTree, 1, true ) ;  
      else if ( e.mTree_type == EXIT ) {
        cout << "\nThanks for using OurScheme!" ;
        return 0 ;  
      } // else if()
    } // catch
    catch ( EvalError t ) {
      if ( t.mType == ERROR_FORMAT ) cout << "ERROR (" << t.mErrorToken << " format)" ;
      else if ( t.mType == ERROR_APPLY_NON_FUNCTION ) 
        cout << "ERROR (attempt to apply non-function) : ", hello.CoutTree( t.mErrorTree, 1, true ) ;
      else if ( t.mType == ERROR_UNBOUND_SYMBOL ) 
        cout << "ERROR (unbound symbol) : " << t.mErrorToken ;
      else if ( t.mType == ERROR_INVALID_ARG_NUMBER )
        cout << "ERROR (incorrect number of arguments) : " << t.mErrorToken ;
      else if ( t.mType == ERROR_INVALID_ARG_TYPE )
        cout << "ERROR (" << t.mErrorToken << " with incorrect argument type) : ", hello.CoutTree( t.mErrorTree, 1, true ) ;
      else if ( t.mType == ERROR_NO_RETURN_VALUE ) 
        cout << "ERROR (no return value) : cond" ;
      else if ( t.mType == ERROR_COND )
        cout << "ERROR (cond parameter format) : ", hello.CoutTree( t.mErrorTree, 1, true ) ;   
    } // catch
    cout << "\n\n> " ;  
  } // while()
} // main()
