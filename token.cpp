#include "token.h"

QString Key_Word_Token::to_String()
{
    switch (value) {
    case REM: return QString("REM");break;
    case LET: return QString("LET");break;
    case PRINT: return QString("PRINT");break;
    case INPUT: return QString("INPUT");break;
    case GOTO: return QString("GOTO");break;
    case IF: return QString("IF");break;
    case THEN: return QString("THEN");break;
    case END: return QString("END");break;
    default:
        throw QString("Error when initialize key_word token.");
    }
}

QString Operator_Token::to_String()
{
    switch (op) {
    case RPA: return QString(")");break;
    case LPA: return QString("(");break;
    case ADD: return QString("+");break;
    case DEL: return QString("-");break;
    case MUL: return QString("*");break;
    case DIV: return QString("/");break;
    case MOD: return QString("MOD");break;
    case NEG: return QString("-");break;
    case POW: return QString("**");break;
    default:
        throw QString("Error when initialize operator token.");
    }
}

QString Conditional_Operator_Token::to_String()
{
    switch (op) {
    case EQ: return QString("=");break;
    case NE: return QString("<>");break;
    case LT: return QString("<");break;
    case GT: return QString(">");break;
    case LE: return QString("<=");break;
    case GE: return QString(">=");break;
    default:
        throw QString("Error when initialize conditional operator token.");
    }
}

QString Identifier_Token::to_String()
{
    return name;
}

QString Constant_Token::to_String()
{
    return QString::number(val);
}

QString String_Token::to_String()
{
    return content;
}

QString Command_Token::to_String()
{
    switch (cmd) {
    case RUN: return QString("RUN");break;
    case LOAD: return QString("LOAD");break;
    case LIST: return QString("LIST");break;
    case CLEAR: return QString("CLEAR");break;
    case HELP: return QString("HELP");break;
    case QUIT: return QString("QUIT");break;
    default:
        throw QString("Error when initialize Command token.");
    }
}
