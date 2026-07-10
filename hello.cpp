#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

// simple error reporting helper
void error(const std::string &msg)
{
    throw std::runtime_error(msg);
}

class Token
{
public:
    char kind;                                     // what kind of token
    double value;                                  // for numbers: a value
    Token(char k) : kind{k}, value{0.0} {}         // construct from one value
    Token(char k, double v) : kind{k}, value{v} {} // construct from two values
};

class Token_stream
{
public:
    Token get();           // get a Token
    void putback(Token t); // put a Token back
private:
    bool full{false};  // is there a Token in the buffer?
    Token buffer{'0'}; // where putback() stores a Token
};

void Token_stream::putback(Token t)
{
    if (full)
        error("putback() into a full buffer");
    buffer = t;
    full = true;
}

Token Token_stream::get()
{
    if (full)
    {
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch; // note that >> skips whitespace (space, newline, tab, etc.)

    switch (ch)
    {
    case ';': // for "print"
    case 'q': // for "quit"
    case '(':
    case ')':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
        return Token{ch};
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        cin.putback(ch); // put digit back into the input stream
        double val;
        cin >> val; // read a floating-point number
        return Token{'8', val};
    }
    default:
        error("Bad token");
    }
}

Token_stream ts; // global Token_stream object

Token get_token()
{
    return ts.get();
}

double expression(); // forward declaration

double primary()
{
    Token t = ts.get();
    switch (t.kind)
    {
    case '(': // handle ’(’ expression ’)’
    {
        double d = expression();
        t = ts.get();
        if (t.kind != ')')
            error("')' expected");
        return d;
    }
    case '8':           // we use ’8’ to represent a number
        return t.value; // retur n the number’s value
    case '-':
        return - primary();
    case '+':
        return primary();
    default:
        error("primar y expected");
    }
}

double term()
{
    double left = primary();
    Token t = ts.get(); // get the next Token from the Token stream
    while (true)
    {
        switch (t.kind)
        {
        case '*':
            left *= primary();
            t = ts.get();
            break;
        case '/':
        {
            double d = primary();
            if (d == 0)
                error("divide by zero");
            left /= d;
            t = ts.get();
            break;
        }
        case '%':
        {
            double d = primary();
            if (d == 0)
                error("%:divide by zero");
            left = fmod(left, d);
            t = ts.get();
            break;
        }
        default:
            ts.putback(t); // put t back into the token stream
            return left;
        }
    }
}

double expression()
{
    double left = term(); // read and evaluate a Term
    Token t = ts.get();   // get the next Token from the Token stream
    while (true)
    {
        switch (t.kind)
        {
        case '+':
            left += term(); // evaluate Term and add
            t = ts.get();
            break;
        case '-':
            left -= term(); // evaluate Term and subtract
            t = ts.get();
            break;
        default:
            ts.putback(t); // put t back into the token stream
            return left;   // finally: no more + or -; return the answer
        }
    }
}

int main()
try
{
    while (cin)
    {
        cout << "> ";
        Token t = ts.get();
        while (t.kind == ';')
            t = ts.get(); // eat ’;’
        if (t.kind == 'q')
            return 0;
        ts.putback(t);
        cout << "= " << expression() << '\n';
    }
    return 0;
}
catch (exception &e)
{
    cerr << e.what() << '\n';
    return 1;
}
catch (...)
{
    cerr << "exception \n";
    return 2;
}