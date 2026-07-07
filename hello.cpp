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
    Token t = get_token();
    switch (t.kind)
    {
    case '(': // handle '(' expression ')'
    {
        double d = expression();
        t = get_token();
        if (t.kind != ')')
            error("')' expected");
        return d;
    }
    case '8':           // we use '8' to represent a number
        return t.value; // return the number's value
    default:
        error("primary expected");
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
    double val = 0; // version 3: ’q’ and ’;’ added
    while (cin)
    {
        Token t = ts.get();
        if (t.kind == 'q') // ’q’ for ‘‘quit’’
            break;
        if (t.kind == ';') // ’;’ for ‘‘pr int now’’
            cout << "=" << val << '\n';
        else
            ts.putback(t);
        val = expression();
    }
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