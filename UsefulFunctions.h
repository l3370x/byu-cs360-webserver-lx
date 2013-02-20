#ifndef USEFULFUNCTIONS_H
#define USEFULFUNCTIONS_H

#include <string>
#include <sstream>
#include <ctime>



using namespace std;

class UsefulFunctions {
public:

    static string convertInt(int number) {
        stringstream ss;
        ss << number;
        return ss.str();
    }
    static string date ( time_t t )
    {
        struct tm * gmtime ( const time_t * timep );
        size_t strftime ( char *s , size_t max , const char * format ,
                          const struct tm * tm );

        struct tm * gmt ;
        char buf [200];
        memset ( buf ,0 ,200);
        gmt = gmtime (& t );
        if ( gmt == NULL )
            return " " ;
        if ( strftime ( buf , sizeof ( buf ) ,
                        "%a, %d %b %Y %H:%M:%S GMT" , gmt ) == 0)
            return " " ;
        return string ( buf );
    }

    static string getDate(){
        time_t currentTime;
        currentTime = time(NULL);
        return date(currentTime);
    }

};

static const string my404ERROR = "<html><head><meta h"
        "ttp-equiv=\"content-type\" conte"
        "nt=\"text/html; charset=utf-8\"/><title>So"
        "rry...</title><style> body { font-family: ver"
        "dana, arial, sans-serif; background-color: #fff; co"
        "lor: #000; }</style></head><body><div><table><tr><t"
        "d><b><font face=times color=#0039b6 size=10>C</font><f"
        "ont face=times color=#c41200 size=10>S</font><font fa"
        "ce=times color=#f3c518 size=10>3</font><font face=tim"
        "es color=#0039b6 size=10>6</font><font face=times col"
        "or=#30a72f size=10>0</font></b></td><td style=\"text-a"
        "lign: left; vertical-align: bottom; padding-bottom: 15p"
        "x; width: 50%\"><div style=\"border-bottom: 1px sol"
        "id #dfdfdf;\">Sorry...</div></td></tr></table></div><d"
        "iv style=\"margin-left: 4em;\"><h1>ERROR 404 - Oh Snap..."
        "</h1><p>The page was not found?</p></div><div style=\"mar"
        "gin-left: 4em;\">";

static const string my403ERROR = "<html><head><meta http-equiv=\"conte"
        "nt-type\" content=\"text/html; charset=utf-8\"/><t"
        "itle>Sorry...</title><style> body { font-family: v"
        "erdana, arial, sans-serif; background-color: #fff;"
        "color: #000; }</style></head><body><div><table><tr"
        "><td><b><font face=times color=#0039b6 size=10>C</f"
        "ont><font face=times color=#c41200 size=10>S</font>"
        "<font face=times color=#f3c518 size=10>3</font><fon"
        "t face=times color=#0039b6 size=10>6</font><font fa"
        "ce=times color=#30a72f size=10>0</font></b></td><td"
        "style=\"text-align: left; vertical-align: bottom; p"
        "adding-bottom: 15px; width: 50%\"><div style=\"bord"
        "er-bottom: 1px solid #dfdfdf;\">Sorry...</div></td>"
        "</tr></table></div><div style=\"margin-left: 4em;\""
        "><h1>ERROR 403 - Oh Snap...</h1><p>You are Forbidde"
        "n to access this page.</p></div><div style=\"margin"
        "-left: 4em;\">";

static const string my400ERROR = "<html><head><meta http-equiv=\"content-t"
        "ype\" content=\"text/html; charset=utf-8\"/><title>So"
        "rry...</title><style> body { font-family: verdana, ar"
        "ial, sans-serif; background-color: #fff; color: #000; }</"
        "style></head><body><div><table><tr><td><b><font face=t"
        "imes color=#0039b6 size=10>C</font><font face=times co"
        "lor=#c41200 size=10>S</font><font face=times color=#f3"
        "c518 size=10>3</font><font face=times color=#0039b6 si"
        "ze=10>6</font><font face=times color=#30a72f size=10>0"
        "</font></b></td><td style=\"text-align: left; vertical"
        "-align: bottom; padding-bottom: 15px; width: 50%\"><di"
        "v style=\"border-bottom: 1px solid #dfdfdf;\">Sorry..."
        "</div></td></tr></table></div><div style=\"margin-left"
        ": 4em;\"><h1>ERROR 400 - Oh Snap...</h1><p>You were st"
        "upid enough to make a bad request.</p></div><div style"
        "=\"margin-left: 4em;\">";

static const string my501ERROR = "<html><head><meta http-equiv=\"content-t"
        "ype\" content=\"text/html; charset=utf-8\"/><title>So"
        "rry...</title><style> body { font-family: verdana, ar"
        "ial, sans-serif; background-color: #fff; color: #000; }</"
        "style></head><body><div><table><tr><td><b><font face=t"
        "imes color=#0039b6 size=10>C</font><font face=times co"
        "lor=#c41200 size=10>S</font><font face=times color=#f3"
        "c518 size=10>3</font><font face=times color=#0039b6 si"
        "ze=10>6</font><font face=times color=#30a72f size=10>0"
        "</font></b></td><td style=\"text-align: left; vertical"
        "-align: bottom; padding-bottom: 15px; width: 50%\"><di"
        "v style=\"border-bottom: 1px solid #dfdfdf;\">Sorry..."
        "</div></td></tr></table></div><div style=\"margin-left"
        ": 4em;\"><h1>ERROR 501 - Oh Snap...</h1><p>How on Earth"
        "did you make an unimplemented request? Silly telnet use"
        "r.</p></div><div style"
        "=\"margin-left: 4em;\">";

static const string my500ERROR = "<html><head><meta http-equiv=\"content-t"
        "ype\" content=\"text/html; charset=utf-8\"/><title>So"
        "rry...</title><style> body { font-family: verdana, ar"
        "ial, sans-serif; background-color: #fff; color: #000; }</"
        "style></head><body><div><table><tr><td><b><font face=t"
        "imes color=#0039b6 size=10>C</font><font face=times co"
        "lor=#c41200 size=10>S</font><font face=times color=#f3"
        "c518 size=10>3</font><font face=times color=#0039b6 si"
        "ze=10>6</font><font face=times color=#30a72f size=10>0"
        "</font></b></td><td style=\"text-align: left; vertical"
        "-align: bottom; padding-bottom: 15px; width: 50%\"><di"
        "v style=\"border-bottom: 1px solid #dfdfdf;\">Sorry..."
        "</div></td></tr></table></div><div style=\"margin-left"
        ": 4em;\"><h1>ERROR 500 - Oh Snap...</h1><p>Not sure wh"
        "at happened here.</p></div><div style"
        "=\"margin-left: 4em;\">";


#endif // USEFULFUNCTIONS_H
