#include<iostream>
#include<string>
#include<vector>
#include<conio.h>
#include<cstdio>
#include<algorithm>
#include<fstream>
#include<numeric>
#include<Python.h>
using namespace std;


void clear_screen()
{
#ifdef WIN32
  system("cls");
#endif
#ifdef unix
  system("clear");
#endif
}
void clear_input()
{
    //move cursor left, change character with space and move cursor left again
    printf("\033[D");
    printf(" ");
    printf("\033[D");
}
string erase_2_new_line_ch(const string& str)
{
    int counter = 0;
    string result;
    for (auto& n : str)
    {
        if (n == '\n')counter++;

        if (counter == 2 && n == '\n')
            counter = 0;
        else
            result += n;
    }
    return result;
}
string cl(const string& str)
{
    string cleared;
    for (auto& n : str)
    {
        if (!iscntrl(n))cleared += n;
    }
    return cleared;
}
string hack(const string& str)
{
    string result;
    for (auto n : str)
    {
        auto last = *(--result.end());
        if (iscntrl(n))result += '~';
        else result += n;
    }
    return result;
}
string make_str_writable(const string& str)
{
    string result;
    for (auto n : str)
    {
        if (n == '~')result += "\n\t";
        else result += n;
    }
    return result;
}
bool process_system_command(const string& input, const string& lines)
{
    if (input[0] == '$')
    {
        if (cl(input) == "$clear")
        {
            clear_screen();
        }
        if (cl(input) == "$save")
        {
            string path;
            cout << "enter path to save session:";
            cin >> path;
            ofstream session(path);
            session << erase_2_new_line_ch(make_str_writable(lines));

            session.close();
            clear_screen();
        }
        return true;
    }
    return false;
}
string read_line(bool& clear_line, bool show = true)
{
    int counter = 0;
    bool def_started = false;
    bool check = true;

    string line;
    char ch = 0;

    if(show)cout<<">>";
    while(true)//until enter
    {
        ch = _getch();
        if (ch == ':' and check)
        {
            def_started = true;
            check = false;
            clear_line = false;
        }

        if(ch == 27)exit(0);//escape
        else if(ch == 8 and !line.empty())//backspace
	    {
	        line.pop_back();
	        clear_input();
	    }
        else if(ch != 8)
	    {
	        cout<<ch;
	        line+=ch;
	    }
        if (!def_started)
        {
            if (ch == 13)break;
        }
        else
        {
            if (ch == 13)
            {
                counter++;
                printf("\033[<1>B");
            }
            else
            {
                if (counter != 0)counter--;
            }
            if (counter == 2)break;
        }
    }

    cout<<endl;
     return line;
}

wchar_t *decode_locale(char** argv)
{
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    return program;
}
bool can_execute_str(const string& str)
{
    vector<pair<char, char>> options{
        make_pair('(',')'),
        make_pair('{','}'),
        make_pair('[',']'),
        make_pair('\'','\''),
        make_pair('"','"')
    };

    for (auto& n: options)
    {
        bool eq = count(str.begin(), str.end(), n.first) ==
                  count(str.begin(), str.end(), n.second);
        if (!eq) return false;
    }

    return true and str.find(":") == string::npos;
}
int main(int argc, char** argv)
{
    auto program = decode_locale(argv);
    Py_SetProgramName(program);
    Py_Initialize();
    clear_screen();

 
    string lines;
    string buffer;
    bool clear_line = true;

    while(true)
    {
        string line = read_line(clear_line,buffer.empty()?true:false);
        lines += line[0] != '$'?hack(line):"";
        if (process_system_command(line,lines)) continue;

        if (can_execute_str(line))
        {
            auto arg = buffer.empty() ? line : buffer;
            arg += line;
            cout<<PyRun_SimpleString(arg.c_str());
            buffer.clear();
            clear_line = true;
        }
        else
        {
            buffer += clear_line ? cl(line) : line;
            clear_line = false;
            line.clear();
        }
    }
  
    PyMem_RawFree(program);
    return 0;
}
