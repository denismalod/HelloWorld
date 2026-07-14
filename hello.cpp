#include <iostream>
#include <vector>
#include <string_view>
#include <stdexcept>
#include <cmath>

using namespace std;

constexpr char quit = 'q';
constexpr char print = ';';
constexpr string_view prompt = "> ";
constexpr string_view result = "= ";
const char name = 'a';        // name token
const char let = 'L';         // declaration token
const string declkey = "let"; // declaration keyword

// simple error reporting helper
void error(const std::string &msg)
{
    throw std::runtime_error(msg);
}

void error(const std::string &msg1, const std::string &msg2)
{
    throw std::runtime_error(msg1 + msg2);
}

class Variable
{
public:
    string name;
    double value;
};

vector<Variable> var_table;

class Token
{
public:
    char kind;
    double value;
    string name;
    Token() : kind{0} {}                                 // default constructor
    Token(char ch) : kind{ch} {}                         // initialize kind with ch
    Token(char ch, double val) : kind{ch}, value{val} {} // initialize kind and value
    Token(char ch, string n) : kind{ch}, name{n} {}      // initialize kind and name
};

class Token_stream
{
    public:
    Token get();           // get a Token
    void putback(Token t); // put a Token back
    void ignore(char c);   // discard tokens up to and including a c
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
    case '=':
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
        if (isalpha(ch))
        {
            string s;
            s += ch;
            while (cin.get(ch) && (isalpha(ch) || isdigit(ch)))
                s += ch;
            cin.putback(ch);
            if (s == declkey)
                return Token{let}; // declaration keyword
            return Token{name, s};
        }
        error("Bad token");
    }
}

void Token_stream::ignore(char c)
// c represents the kind of Token
{
    if (full && c == buffer.kind)
    { // first look in buffer
        full = false;
        return;
    }
    full = false;
    // now search input:
    char ch = 0;
    while (cin >> ch)
    if (ch == c)
    return;
}

Token_stream ts; // global Token_stream object

void clean_up_mess() // naive
{
    ts.ignore(print);
}

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
        return -primary();
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

bool is_declared(string var)
// is var already in var_table?
{
    for (const Variable &v : var_table)
        if (v.name == var)
            return true;
    return false;
}

double define_name(string var, double val)
// add {var,val} to var_table
{
    if (is_declared(var))
        error(var, " declared twice");
    var_table.push_back(Variable{var, val});
    return val;
}

double declaration()
// assume we have seen "let’’
// handle: name = expression
// declare a var iable called "name’’ with the initial value "expression’’
{
    Token t = ts.get();
    if (t.kind != name)
        error("name expected in declaration");
    Token t2 = ts.get();
    if (t2.kind != '=')
        error("= missing in declaration of ", t.name);
    double d = expression();
    define_name(t.name, d);
    return d;
}

double statement()
{
    Token t = ts.get();
    switch (t.kind)
    {
    case let:
        return declaration();
    default:
        ts.putback(t);
        return expression();
    }
}
void calculate()
{
    while (cin)
        try
        {
            cout << prompt;
            Token t = ts.get();
            while (t.kind == print)
                t = ts.get(); // first discard all "prints"
            if (t.kind == quit)
                return;
            ts.putback(t);
            cout << result << statement() << '\n';
        }
        catch (runtime_error &e)
        {
            cerr << e.what() << '\n'; // wr ite error message
            clean_up_mess();
        }
}

int main()
try
{
    calculate();
    return 0;
}
catch (runtime_error &e)
{
    cerr << e.what() << '\n';
    return 1;
}
catch (...)
{
    cerr << "exception \n";
    return 2;
}