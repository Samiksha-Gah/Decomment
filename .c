#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
/*constants, states in the DFA*/
enum Statetype {NORMAL, IN_STRING, IN_CHAR_LITERAL, 
CHAR_LITERAL_IGNORED, STRING_CHAR_IGNORED, IN_COMMENT, 
POSSIBLE_COMMENT_START, POSSIBLE_END_COMMENT};

/* the standard state - after taking  in the character,
it prints it and determines whether in should proceed to IN_STRING ("),
IN_CHAR_LITERAL ('), or just continue as NORMAL. If it is potentially 
the start of the comment, it does not print. The state is returned */
enum Statetype stateNormal(int x, int *line_num)
{
    enum Statetype state;
    if (x == '"') {
        putchar(x);
        state = IN_STRING;
    }
    else if (x == '\'') {
        putchar(x);
        state = IN_CHAR_LITERAL;
    } 
    else if (x == '/') {
        state = POSSIBLE_COMMENT_START;
    }
    else  {
        /* if (x == '\n') {
            (*line_num)++;
        } */
        putchar(x);
        state = NORMAL;     
}
return state;
}

/* function for when in the string - after taking in the character,
it prints it and determines whether in should be an ignored character, 
proceed back to the standard/normal state, or continue in string.
The state is returned */
enum Statetype stateInString(int x)
{
    enum Statetype state;
    if (x == '\\') {
        putchar(x);
        state = STRING_CHAR_IGNORED;
    }
    else if (x == '"') {
        putchar(x);
        state = NORMAL;
    } 
    else {
         putchar(x);
         state = IN_STRING;
}
return state;
}

/* function for when in char literal - after taking in the character,
it prints it and determines whether in should be an ignored character, 
proceed back to the standard/normal state, or continue in char literal.
The state is returned */
enum Statetype stateInCharLiteral(int x)
{
    enum Statetype state;
    if (x == '\\') { 
        putchar(x);
        state = CHAR_LITERAL_IGNORED;
    }
    else if (x == '\'') {
        putchar(x);
        state = NORMAL;
    } 
    else {
        putchar(x);
        state = IN_CHAR_LITERAL;
}
return state;
}

/* function for ignored character in string state. Prints 
the character and returns back to in string */
enum Statetype stateStringCharIgnored(int x)
{
    enum Statetype state;
    putchar(x);
    state = IN_STRING;
    return state; 
}

/* function for ignored character in char literal state. Prints 
the character and returns back to in char literal */
enum Statetype stateCharLiteralIgnored(int x)
{
    enum Statetype state;
    putchar(x);
    state = IN_CHAR_LITERAL;
    return state; 
}


/* Function for within the comment - it does not print x, 
but ends the comment, continues in the comment, and 
adds a new line if needed  */
enum Statetype stateInComment(int x)
{
    enum Statetype state;
    if (x == '*') {
        state = POSSIBLE_END_COMMENT; 
    }
    else if (x == '\n') {
        putchar(x); /*new line*/
        state = IN_COMMENT; 
    }
    else {
        state = IN_COMMENT;
    }
    return state;
}

/* Possible Comment Start Function - prints the char and the slash if
its not in the comment. It returns the new state */

 enum Statetype statePossibleCommentStart(int x, int *line_num, int *error_num)
{
    enum Statetype state;
    *error_num = *line_num;
    if (x == '/') {
        printf("/"); /*prints the second slash */
        state = POSSIBLE_COMMENT_START;
    }
    else if (x == '\'') {
        printf("/%c", x); /*print slash and proceed to in char literal*/
        state = IN_CHAR_LITERAL;
    }
    else if (x =='"') {
        printf("/%c", x); /*print slash and proceed to in string */
        state = IN_STRING;
    }
    else if (x == '*') {
        printf(" "); /*print space */
        state = IN_COMMENT;
    } 
    else {
        printf("/%c", x); /*go back to normal, since it is not 
        in a comment, printing the slash and char */
        state = NORMAL;
    }
    return state;
}


/* xyz*/
enum Statetype statePossibleEndComment(int x)
{
    enum Statetype state;
    if (x == '*') { /* stay in possible end comment */ 
        state = POSSIBLE_END_COMMENT;
    }

    else if (x == '\n') {     /* new line */ 
        putchar(x);
        state = IN_COMMENT;
    }
    else if (x == '/') { /* go back to normal, ended */ 
        state = NORMAL;
    }
    else {
        state = IN_COMMENT;
    }
return state;
}



/* Main function, printing non-comment text - 
Return EXIT_FAILURE or EXIT_SUCCESS depending
on whether in unterminated comment or not */ 
int main(void) {
    enum Statetype state = NORMAL;
    int line_num = 1;
    int error_num = line_num;
    int x;
    /* read until EOF */
    while ((x = getchar()) != EOF) { 
        if (x == '\n') {
            line_num = line_num + 1; 
        } 
        switch (state) {
            case NORMAL: 
                state = stateNormal(x, &line_num);
                break;
            case IN_STRING:
                state = stateInString(x);
                break;
            case IN_CHAR_LITERAL: 
                state = stateInCharLiteral(x);
                break;
            case CHAR_LITERAL_IGNORED: 
                state = stateCharLiteralIgnored(x);
                break;
            case STRING_CHAR_IGNORED: 
                state = stateStringCharIgnored(x);
                break;
            case IN_COMMENT: 
                state = stateInComment(x);
                break;
            case POSSIBLE_COMMENT_START: 
                /*if (state == POSSIBLE_COMMENT_START) {
                error_num = line_num;
                }*/
                state = statePossibleCommentStart(x, &line_num, &error_num); 
                break;
            case POSSIBLE_END_COMMENT: 
                state = statePossibleEndComment(x);
                break;
        }
    }
    /* If possibly at the end of a comment or in a comment, it is unterminated
    and causes an EXIT_FAILURE */
    if (state == POSSIBLE_END_COMMENT || state == IN_COMMENT) {
        fprintf(stderr, "Error: line %d: unterminated comment\n", error_num);
        return EXIT_FAILURE;
    }
     else {
        /* if ended in maybe comment, the last slash wasn't printed, so adding that */ 
        if (state == POSSIBLE_COMMENT_START) {
        printf("/");
        }
        /* No unterminated comment, so it's successful*/
        return EXIT_SUCCESS;
    }
}
