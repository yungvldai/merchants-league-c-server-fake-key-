#include <iostream>
#include <sstream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <ctime>

using namespace std;

string chr2str(char*);
void clean();
void answer(TCPsocket,const char*);

char floating[] = 
"MerchantsLeague-echo v.17_1/28";
string key = "a6:d3:f7:s1";

int main(int argc, char* argv[]) {
    time_t t;
    struct tm *tm;

    const long req_size = 256;
    char request[req_size];
    string to_parse, instr, tmp;

    SDL_Init (SDL_INIT_EVERYTHING);
    SDLNet_Init();

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, 49002);
    TCPsocket server = SDLNet_TCP_Open(&ip);
    TCPsocket client;

    while (1) {
        SDL_Delay(25);
        client = SDLNet_TCP_Accept(server);
        if (client) {
            SDLNet_TCP_Recv(client, request, req_size);
            to_parse = chr2str(request);
            stringstream ios(to_parse);
            ios >> instr;
            if (instr == "floating") {
                SDLNet_TCP_Send(client, floating, strlen(floating) + 1);
            }
            else if (instr == "break") {
                ios >> tmp;
                if (tmp == key) {
                    answer(client, "@ok");
                    SDLNet_TCP_Close(client);
                    SDLNet_TCP_Close(server);
                    clean();
                    exit(0);
                }
                else {
                    answer(client, "@rj-wp");
                }
            }
            else if (instr == "login") {
                string user = "", password = "";
                string check_pass = "", check_login = "";
                ios >> user >> password;
                ifstream fin(("users/" + user + "/profile").c_str());
                if (!fin.is_open()) {
                    answer(client, "@rj-nu");
                } else {
                    fin >> check_login >> check_pass;
                    if (check_login == user && check_pass == password) {
                        answer(client, "@ok");
                    } else {
                        answer(client, "@rj-wp");
                    }
                    fin.close();
                }
            }
            else if (instr == "signup") {
                string user = "", password = "";
                ios >> user >> password;
                ifstream fin(("users/" + user + "/profile").c_str());
                if (!fin.is_open()) {
                    system(("mkdir users/" + user).c_str());
                    ofstream fout(("users/" + user + "/profile").c_str());
                    fout << user << " " << password;
                    fout.close();
                    fout.open(("users/" + user + "/mail").c_str());
                    fout << "***";
                    fout.close();
                    fout.open(("users/" + user + "/money").c_str());
                    fout << "0";
                    fout.close();
                    answer(client, "@ok");
                } else {
                    answer(client, "@rj-ar");
                    fin.close();
                }
            }
            else if (instr == "pay") {
                string fromn = "",fromp = "",ton = "";
                long long howm = 0,tmp = 0,tmp2 = 0;
                ios >> fromn >> fromp >> ton >> howm;
                string chn = "",chp = "";

                ifstream fin(("users/" + fromn + "/profile").c_str());
                if (!fin.is_open()) {
                    answer(client, "@rj-nu");
                } else {
                    fin >> chn >> chp;
                    fin.close();
                    fin.open(("users/" + ton + "/money").c_str());
                    if (!fin.is_open()) {
                        answer(client, "@rj-nu");
                    } else {
                        fin >> tmp;
                        fin.close();
                        if(chn == fromn && chp == fromp){
                            fin.open(("users/" + fromn + "/money").c_str());
                            fin >> tmp2;
                            fin.close();
                            if(tmp2 >= howm){
                                ofstream fout(("users/" + fromn + "/money").c_str());
                                fout << (tmp2 - howm);
                                fout.close();
                                fout.open(("users/" + ton + "/money").c_str());
                                fout << (tmp + howm);
                                fout.close();
                                fout.open(("users/" + ton + "/mail").c_str(),ios::app);
                                fout << endl << "# " << fromn << " pay you " << howm  << " gold." << endl;
                                fout.close();
                                answer(client, "@ok");
                                fout.open("transactions",ios::app);
                                t = time(NULL);
                                tm = localtime(&t);
                                fout << " [ " << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec << " ] " << fromn << " -> " << ton << " : " << howm << endl;
                                fout.close();
                            }
                            else {
                                answer(client, "@rj-nm");
                            }
                        }
                        else {
                            answer(client, "@rj-wp");
                        }
                    }
                }
            }
            else if (instr == "bill"){
                string from = "",to = "",howm = "0",fromp = "",chu = "",chp = "";
                ios >> from >> to >> howm;

                ifstream fin(("users/" + to + "/profile").c_str());
                if (!fin.is_open()) {
                    answer(client, "@rj-nu");
                } else {
                    fin >> chu >> chp;
                    fin.close();
                    if(fromp == chp && from == chu){
                        fin.open(("users/" + to + "/mail").c_str());
                        if (!fin.is_open()) {
                            answer(client, "@rj-nu");
                        } else {
                            fin.close();
                        }
                        ofstream fout(("users/" + to + "/mail").c_str(),ios::app);
                        fout << endl << "# " << from << " bill you for payment (" << howm << " gold)." << endl;
                        fout.close();
                        answer(client, "@ok");
                    }
                    else {
                        answer(client,"@rj-wp");
                    }
                }
            }
            else if (instr == "msg"){
                string from = "",to = "",msg = "",fromp = "",chp = "",chu = "";
                ios >> from >> fromp >> to;
                getline(ios,msg);
                ifstream fin(("users/" + from + "/profile").c_str());
                if (!fin.is_open()) {
                    answer(client, "@rj-nu");
                } else {
                    fin >> chu >> chp;
                    fin.close();
                    if(fromp == chp && from == chu){
                        fin.open(("users/" + to + "/mail").c_str());
                        if (!fin.is_open()) {
                            answer(client, "@rj-nu");
                        } else {
                            fin.close();
                        }
                        ofstream fout(("users/" + to + "/mail").c_str(),ios::app);
                        fout << endl << "from " << from << ": " << msg << endl;
                        fout.close();
                        answer(client, "@ok");
                    }
                    else {
                        answer(client, "@rj-wp");
                    }
                }
            }
            else if (instr == "mail"){
                string mail = "",user = "",password = "",chp = "",chu = "";
                ios >> user >> password;
                ifstream fin(("users/" + user + "/profile").c_str());
                if (!fin.is_open()) {
                    answer(client, "@rj-nu");
                } else {
                    fin >> chu >> chp;
                    fin.close();
                    if(chu == user && chp == password){
                        fin.open(("users/" + user + "/mail").c_str());
                        while(!fin.eof()) {
                            mail += fin.get();
                        } 
                        mail[mail.length() - 1] = '\0';
                        fin.close();
                        answer(client,mail.c_str());
                        ofstream fout(("users/" + user + "/mail").c_str());
                        fout << "***";
                        fout.close();
                    }
                    else {
                        answer(client,"@rj-wp");
                    }
                }
            }
            else if (instr == "news"){
                string news = "";
                ifstream fin("news.txt");
                while(!fin.eof()) {
                    news += fin.get();
                } 
                news[news.length() - 1] = '\0';
                fin.close();
                answer(client,news.c_str());
            }
            else if (instr == "my") {
                string user;
                string tmp;
                ios >> user;
                ifstream fin(("users/" + user + "/money").c_str());
                if (!fin.is_open()) {
                    answer(client, "@rj-nu");
                } else {
                    fin >> tmp;
                    fin.close();
                    answer(client,tmp.c_str());
                }
            }
            else {
                answer(client,"@rj-er");
            }
        }
        SDLNet_TCP_Close(client);
    }
    SDLNet_TCP_Close(server);
    clean();
    
    return 0;
}

void clean() {
    SDLNet_Quit();
    SDL_Quit();
}
string chr2str(char* a){
    string returnable = "";
    for (int i = 0; i < strlen(a); i++) {
        returnable += a[i];
    }
    return returnable;
}

void answer(TCPsocket socket,const char* msg){
    SDLNet_TCP_Send(socket, msg,strlen(msg) + 1);
}
