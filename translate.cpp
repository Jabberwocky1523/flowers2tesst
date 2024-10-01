#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ostream>
#include <windows.h>
#include <iterator>
#include <string>
#include <algorithm>
#include <io.h>
#include <sstream>
#include <unordered_map>
#include <map>
#include "cimage.hpp"
#include <queue>
using namespace std;
std::vector<int> stringToVecInt(const std::string &str)
{
    union
    {
        char c[2];
        int i;
    } convert;

    // 段位清零
    convert.i = 0;

    std::vector<int> vec;

    for (unsigned i = 0; i < str.length(); i++)
    {
        // GBK编码首字符大于0x80
        if ((unsigned)str[i] > 0x80)
        {
            // 利用union进行转化，注意是大端序
            convert.c[1] = str[i];
            convert.c[0] = str[i + 1];
            vec.push_back(convert.i);
            i++;
        }
        else
            // 小于0x80，为ASCII编码，一个字节
            vec.push_back(str[i]);
    }
    return vec;
}
bool include(const std::string &str, const std::string &msg)
{
    auto sour = stringToVecInt(str);
    auto find = stringToVecInt(msg);
    return std::search(sour.begin(), sour.end(), find.begin(), find.end()) != sour.end();
}
void getFiles(string path, vector<string> &files)
{
    // 文件句柄
    intptr_t hFile = 0;
    // 文件信息
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            // 如果是目录,迭代之
            // 如果不是,加入列表
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFiles(p.assign(path).append("/").append(fileinfo.name), files);
            }
            else
            {
                files.push_back(fileinfo.name);
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}
vector<string> split(const string &s, const string &seperator)
{
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size())
    {
        int flag = 0;
        while (i != s.size() && flag == 0)
        {
            flag = 1;
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[i] == seperator[x])
                {
                    ++i;
                    flag = 0;
                    break;
                }
        }

        flag = 0;
        string_size j = i;
        while (j != s.size() && flag == 0)
        {
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[j] == seperator[x])
                {
                    flag = 1;
                    break;
                }
            if (flag == 0)
                ++j;
        }
        if (i != j)
        {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}
void erase(string &str, string ch1)
{
    string::iterator it;
    for (it = str.begin(); it < str.end(); it++)
    {
        for (char ch : ch1)
        {
            if (*it == ch)
            {
                str.erase(it);
                it--;
            }
        }
    }
}
int main()
{
    string speaker = "";
    int speakertemp = 0;
    int cleantemp = 0;
    int trantemp = 0;
    ifstream in("123/02a_01901.d");
    string screen = "l1";
    string pic[5] = {""};
    string character[5] = {""};
    string oldcharacter[5] = {""};
    int charactertemp[5] = {0};
    string background = "";
    int backgroundtemp = 0;
    string face = "";
    string oldface = "";
    int facetemp = 0;
    map<string, int> chpos0 = {{"xpos", 0}, {"ypos", 0}};
    map<string, int> chpos1 = {{"xpos", 0}, {"ypos", 0}};
    map<string, int> chpos2 = {{"xpos", 0}, {"ypos", 0}};
    map<string, int> chpos3 = {{"xpos", 0}, {"ypos", 0}};
    map<string, int> chpos4 = {{"xpos", 0}, {"ypos", 0}};
    map<int, map<string, int>> chpos = {{3, chpos3}, {4, chpos4}, {1, chpos1}, {2, chpos2}, {0, chpos0}};
    map<int, map<string, int>>::iterator chit1;
    map<string, int>::iterator chit2;
    map<string, int> tran0 = {{"startxpos", 0}, {"startypos", 0}, {"endxpos", 0}, {"endypos", 0}, {"startalign", 0}, {"endalign", 0}, {"time", 0}};
    map<string, int> tran1 = {{"startxpos", 0}, {"startypos", 0}, {"endxpos", 0}, {"endypos", 0}, {"startalign", 0}, {"endalign", 0}, {"time", 0}};
    map<string, int> tran2 = {{"startxpos", 0}, {"startypos", 0}, {"endxpos", 0}, {"endypos", 0}, {"startalign", 0}, {"endalign", 0}, {"time", 0}};
    map<string, int> tran3 = {{"startxpos", 0}, {"startypos", 0}, {"endxpos", 0}, {"endypos", 0}, {"startalign", 0}, {"endalign", 0}, {"time", 0}};
    map<string, int> tran4 = {{"startxpos", 0}, {"startypos", 0}, {"endxpos", 0}, {"endypos", 0}, {"startalign", 0}, {"endalign", 0}, {"time", 0}};
    map<int, map<string, int>> tran = {{0, tran0}, {1, tran1}, {2, tran2}, {3, tran3}, {4, tran4}};
    map<int, map<string, int>>::iterator it1;
    map<string, int>::iterator it2;
    string line;
    string a;
    queue<int> tweening_queue;
    queue<map<string, int>> tweening_queue_tran;
    if (in)
    {
        ofstream out("test.rpy");
        while (getline(in, line))
        {
            vector<string> temp = split(line, "(");
            string ans = temp.at(0);
            if (temp.size() > 1)
            {
                a = temp.at(1).substr(0, temp.at(1).length() - 1);
            }
            if (temp.at(0).compare("    screen_fade_out") == 0)
            {
                background = "black";
                out << "    scene onlayer tran" << endl;
                out << "    scene onlayer l1" << endl;
                out << "    scene onlayer l2" << endl;
                backgroundtemp = 1;
            }
            else if (ans.compare("    add_history") == 0)
            {
                out << "    voice sustain" << endl;
                out << "    " << a << endl;
            }
            else if (ans.compare("    bgm_fade_out") == 0)
            {
                float time = stof(a) / 1000;
                out << "    stop music" << endl;
            }
            else if (ans.compare("    play_se") == 0)
            {
                vector<string> con = split(a, ",");
                string con1 = con.at(0);
                con1 = con1.substr(1, con1.length() - 2);
                out << "    play sound \"se/" << con1 << ".ogg\"" << endl;
            }
            else if (ans.compare("    se_stop") == 0)
            {
                out << "    stop sound" << endl;
            }
            else if (ans.compare("    next_script") == 0)
            {
                a = a.substr(1, a.length() - 4);
                out << "    jump sc" << a << endl;
            }
            else if (ans.compare("    se_fade_out") == 0)
            {
                vector<string> con = split(a, ",");
                float con1 = stof(con.at(0)) / 1000;
                out << "    stop sound" << endl;
            }
            else if (ans.compare("    update_screen") == 0)
            {
                out << "    voice sustain" << endl;
                if (cleantemp == 1)
                {
                    cleantemp = 0;
                }
                else if (backgroundtemp == 1)
                {
                    out << "    scene onlayer tran" << endl;
                    out << "    scene " << background << " onlayer l0" << endl;
                    backgroundtemp = 0;
                }
                else if (facetemp == 1)
                {
                    if (oldface.compare("") != 0)
                    {
                        out << "    hide " << oldface << " onlayer l1" << endl;
                        out << "    voice sustain" << endl;
                        out << "    hide " << oldface << " onlayer l2" << endl;
                    }
                    out << "    show " << face << " onlayer " << screen << " at face()" << endl;
                    facetemp = 0;
                }
                for (int i = 0; i < 5; i++)
                {
                    if (charactertemp[i] == 1)
                    {
                        if (oldcharacter[i].compare("") != 0)
                        {
                            out << "    hide " << oldcharacter[i] << " onlayer l1" << endl;
                            out << "    hide " << oldcharacter[i] << " onlayer l2" << endl;
                        }
                        string imagepath = "images/" + character[i] + ".png";
                        out << "    show " << character[i] << " onlayer " << screen << ":" << endl;
                        chit1 = chpos.find(i);
                        chit2 = chit1->second.find("xpos");
                        int xpos = chit2->second;
                        chit2 = chit1->second.find("ypos");
                        int ypos = chit2->second;
                        int height = 0;
                        int width = 0;
                        int filesize = 0;
                        get_image_size_without_decode_image(imagepath.c_str(), &width, &height, &filesize);
                        out << "        " << "parallel:" << endl;
                        out << "            " << "xpos " << xpos - (width / 2) << endl;
                        out << "            " << "ypos " << ypos << endl;
                    }
                    charactertemp[i] = 0;
                    oldcharacter[i] = "";
                }
                float time = stof(a);
                out << "    with Dissolve(" << time / 1000 << ")" << endl;
            }
            else if (ans.compare("    pause") == 0)
            {
                out << "    voice sustain" << endl;
                float num = stof(a);
                out << "    pause(" << num / 1000 << ")" << endl;
            }
            else if (ans.compare("    play_voice") == 0)
            {
                a = a.substr(1, a.length() - 2);
                out << "    voice \"" << a << "\"" << endl;
            }
            else if (ans.compare("    load_pic") == 0)
            {
                vector<string> con = split(a, ",");
                int i = stoi(con.at(1));
                string con1 = con.at(0);
                con1 = con1.substr(1, con1.length() - 6);
                pic[i] = con1;
            }
            else if (ans.compare("    load_character") == 0)
            {
                vector<string> con = split(a, ",");
                int i = stoi(con.at(1));
                if (character[i].compare("") != 0)
                {
                    oldcharacter[i] = character[i];
                }
                string con1 = con.at(0);
                con1 = con1.substr(1, con1.length() - 6);
                character[i] = con1;
                charactertemp[i] = 1;
            }
            else if (ans.compare("    character_pos") == 0)
            {
                vector<string> con = split(a, ",");
                int i = stoi(con.at(3));
                int xpos = stoi(con.at(0));
                int ypos = stoi(con.at(1));
                chit1 = chpos.find(i);
                chit2 = chit1->second.find("xpos");
                chit2->second = xpos;
                chit2 = chit1->second.find("ypos");
                chit2->second = ypos;
            }
            else if (ans.compare("    load_face") == 0)
            {
                if (face.compare("") != 0)
                {
                    oldface = face;
                }
                face = a.substr(1, a.length() - 6);
                facetemp = 1;
            }
            else if (ans.compare("    load_text") == 0)
            {
                out << "    voice sustain" << endl;
                string text = a.substr(1, a.length() - 2);
                if (include(a, "12123"))
                {
                    speaker = a.substr(6, a.length() - 1);
                    speakertemp = 1;
                    continue;
                }
                if (speakertemp == 1)
                {
                    out << "    \"" << speaker << " \"" << text << "\"" << endl;
                    speakertemp = 0;
                }
                else
                {
                    out << "    \"" << text << "\"" << endl;
                }
            }
            else if (ans.compare("    play_bgm") == 0)
            {
                vector<string> con = split(a, ",");
                string bgm = con.at(0);
                bgm = bgm.substr(1, bgm.length() - 2);
                out << "    play music " << "\"bgm/" << bgm << ".ogg\"" << endl;
            }
            else if (ans.compare("    bgm_stop") == 0)
            {
                out << "    stop music" << endl;
            }
            else if (ans.compare("    clean_png") == 0)
            {
                out << "    voice sustain" << endl;
                out << "    scene onlayer l1" << endl;
                out << "    scene onlayer l2" << endl;
                out << "    scene onlayer tran" << endl;
                out << "    with Dissolve(0.1)" << endl;
                cleantemp = 1;
                for (int i = 0; i < 5; i++)
                {
                    character[i] = "";
                    charactertemp[i] = 0;
                    pic[i] = "";
                }
                face = "";
                oldface = "";
                facetemp = 0;
            }
            else if (ans.compare("    background") == 0 || ans.compare("    interlude") == 0)
            {
                background = a.substr(1, a.length() - 6);
                backgroundtemp = 1;
                out << "    scene onlayer l1" << endl;
                out << "    scene onlayer l2" << endl;
                out << "    scene onlayer tran" << endl;
            }
            else if (ans.compare("    set_window") == 0)
            {
                out << "    voice sustain" << endl;
                int window = stoi(a);
                if (window == 0)
                {
                    screen = "l1";
                }
                else
                {
                    screen = "l2";
                }
            }
            else if (ans.compare("    tweening_begin") == 0)
            {
                string line1;
                getline(in, line1);
                vector<string> con = split(a, ",");
                vector<string> temp1 = split(line1, "(");
                string a1 = temp1.at(1).substr(0, temp1.at(1).length() - 1);
                vector<string> con1 = split(a1, ",");

                int picnum = stoi(con.at(0));
                int num = stoi(con.at(0));
                it1 = tran.find(picnum);
                it2 = it1->second.find("startxpos");
                it2->second = stoi(con.at(1));
                int startxpos = stoi(con.at(1));
                it2 = it1->second.find("startypos");
                it2->second = stoi(con.at(2));
                int startypos = stoi(con.at(2));
                it2 = it1->second.find("startalign");
                it2->second = stoi(con.at(5));
                int startalign = stoi(con.at(5));

                it2 = it1->second.find("endxpos");
                it2->second = stoi(con1.at(2));
                int endxpos = stoi(con1.at(2));
                it2 = it1->second.find("endypos");
                it2->second = stoi(con1.at(3));
                int endypos = stoi(con1.at(3));
                it2 = it1->second.find("endalign");
                it2->second = stoi(con1.at(6));
                int endalign = stoi(con1.at(6));
                it2 = it1->second.find("time");
                int time = stoi(con1.at(7));
                it2->second = time;
                tweening_queue.push(picnum);
                map<string, int> tween_tran = {{"startxpos", startxpos}, {"startypos", startypos}, {"startalign", startalign}, {"endxpos", endxpos}, {"endypos", endypos}, {"time", time}, {"endalign", endalign}};
                tweening_queue_tran.push(tween_tran);
            }
            else if (ans.compare("    tweening_start") == 0)
            {
                out << "    voice sustain" << endl;
                out << "    scene onlayer l1" << endl;
                out << "    scene onlayer l2" << endl;
                out << "    scene onlayer tran" << endl;
                while (!tweening_queue.empty())
                {
                    string imagepath;
                    int num = tweening_queue.front();
                    map<string, int> tween_tran = tweening_queue_tran.front();
                    if (pic[num].compare("") != 0)
                    {
                        imagepath = "images/" + pic[num] + ".png";
                        out << "    show " + pic[num] + " onlayer tran:" << endl;
                    }
                    else if (character[num].compare("") != 0)
                    {
                        imagepath = "images/" + character[num] + ".png";
                        out << "    show " + character[num] + " onlayer tran:" << endl;
                    }
                    int width = 0;
                    int height = 0;
                    int size = 0;
                    get_image_size_without_decode_image(imagepath.c_str(), &width, &height, &size);
                    it2 = tween_tran.find("startxpos");
                    int startxpos = it2->second - (width / 2);
                    it2 = tween_tran.find("startypos");
                    int startypos = it2->second;
                    it2 = tween_tran.find("startalign");
                    float startalpha = 1.0 * it2->second / 255;
                    it2 = tween_tran.find("endxpos");
                    int endxpos = it2->second - (width / 2);
                    it2 = tween_tran.find("endypos");
                    int endypos = it2->second;
                    it2 = tween_tran.find("endalign");
                    float endalpha = 1.0 * it2->second / 255;
                    it2 = tween_tran.find("time");
                    float time = 1.0 * it2->second / 1000;
                    out << "        parallel:" << endl;
                    out << "            xpos " << startxpos << endl;
                    out << "            ypos " << startypos << endl;
                    out << "            alpha " << startalpha << endl;
                    if (startxpos != endxpos)
                    {
                        out << "            linear " << time << " xpos " << endxpos << endl;
                    }
                    else if (startypos != endypos)
                    {
                        out << "            linear " << time << " ypos " << endypos << endl;
                    }
                    else if (startalpha != endalpha)
                    {
                        out << "            linear " << time << " alpha " << endalpha << endl;
                    }
                    tweening_queue.pop();
                    tweening_queue_tran.pop();
                }
            }
            else if (ans.compare("    tweening_stop") == 0)
            {
                int picnum = stoi(a);
            }
            else if (ans.compare("    start_option") == 0)
            {
                out << "    menu:" << endl;
                while (getline(in, line))
                {
                    temp = split(line, "(");
                    ans = temp.at(0);
                    a = temp.at(1).substr(0, temp.at(1).length() - 1);
                    if (ans.compare("    stop_option") == 0)
                    {
                        break;
                    }
                    else if (ans.compare("    option") == 0)
                    {
                        vector<string> con = split(a, ",");
                        string text = con.at(0);
                        int num = stoi(con.at(1));
                        out << "        " << text << ":" << endl;
                        if (num < 100 && num > 0)
                        {
                            out << "            jump" << " label_00" << num << endl;
                        }
                        else if (num < 1000 && num >= 100)
                        {
                            out << "            jump" << " label_0" << num << endl;
                        }
                        else
                        {
                            out << "            jump" << " label_" << num << endl;
                        }
                    }
                }
            }
            else if (ans.find("::label_") != string::npos)
            {
                ans = ans.substr(2, ans.length() - 4);
                out << "label " << ans << ":" << endl;
            }
            else if (ans.compare("    jump") == 0)
            {
                int num = stoi(a);
                if (num < 100 && num > 0)
                {
                    out << "    jump label_00" << num << endl;
                }
                else if (num < 1000 && num >= 100)
                {
                    out << "    jump label_0" << num << endl;
                }
                else
                {
                    out << "    jump label_" << num << endl;
                }
            }
            else if (ans.compare("    change_var") == 0)
            {
                vector<string> con = split(a, ",");
                int num = stoi(con.at(0));
                int n = stoi(con.at(1));
                out << "    $game" << num << " +=" << n << endl;
            }
            else if (ans.compare("    wait_click") == 0)
            {
                out << "    pause" << endl;
            }
            else if (ans.compare("    special_se") == 0)
            {
                vector<string> con = split(a, ",");
                string con1 = con.at(0);
                out << "    play sound \"se/" << con1 << ".ogg\"" << endl;
            }
            else if (ans.compare("    func_83b_jumpif") == 0)
            {
                vector<string> con1 = split(a, ",");
                out << "    if persistent.loop == 2:" << endl;
                int num = stoi(con1.at(1));
                if (num < 100 && num > 0)
                {
                    out << "        jump label_00" << num << endl;
                }
                else if (num < 1000 && num >= 100)
                {
                    out << "        jump label_0" << num << endl;
                }
                else
                {
                    out << "        jump label_" << num << endl;
                }
            }
            else if (ans.compare("    jump_on_greater") == 0)
            {
                vector<string> con1 = split(a, ",");
                int num = stoi(con1.at(2));
                out << "    if game" << con1.at(0) << " > " << con1.at(1) << ":" << endl;
                if (num < 100 && num > 0)
                {
                    out << "        jump label_00" << num << endl;
                }
                else if (num < 1000 && num >= 100)
                {
                    out << "        jump label_0" << num << endl;
                }
                else
                {
                    out << "        jump label_" << num << endl;
                }
            }
            else if (ans.compare("    set_var") == 0)
            {
                vector<string> con1 = split(a, ",");
                out << "    $game" << con1.at(0) << " = " << con1.at(1) << endl;
            }
            else if (ans.compare("    jump_on_equal") == 0)
            {
                vector<string> con1 = split(a, ",");
                int num = stoi(con1.at(2));
                out << "    if game" << con1.at(0) << " == " << con1.at(1) << ":" << endl;
                if (num < 100 && num > 0)
                {
                    out << "        jump label_00" << num << endl;
                }
                else if (num < 1000 && num >= 100)
                {
                    out << "        jump label_0" << num << endl;
                }
                else
                {
                    out << "        jump label_" << num << endl;
                }
            }
            else if (ans.compare("    jump_on_less") == 0)
            {
                vector<string> con1 = split(a, ",");
                int num = stoi(con1.at(2));
                out << "    if game" << con1.at(0) << " < " << con1.at(1) << ":" << endl;
                if (num < 100 && num > 0)
                {
                    out << "        jump label_00" << num << endl;
                }
                else if (num < 1000 && num >= 100)
                {
                    out << "        jump label_0" << num << endl;
                }
                else
                {
                    out << "        jump label_" << num << endl;
                }
            }
            else if (ans.compare("    end_game") == 0)
            {
                out << "    $persistent.loop = 2" << endl;
            }
        }
    }
}
// int main()
// {
//     string path = "./ceshi/output";
//     vector<string> files;
//     getFiles(path, files);
//     for (string a : files)
//     {
//         ifstream in("./ceshi/output/" + a);
//         ofstream out(a);
//         string filename;
//         string line;
//         if (in)
//         {
//             while (getline(in, line))
//             {
//                 vector<string> temp = split(line, " ");
//                 string str = temp[temp.size() - 1];
//                 if (str.find(".png") != string::npos || str.find(".bmp") != string::npos)
//                 {
//                     erase(str, ".bmp");
//                     erase(str, ".png");
//                     out << "screen " + str + " with dissolve" << endl
//                         << endl;
//                 }
//                 else if (temp.size() == 1)
//                 {
//                     if (str.find("bgm") != string::npos)
//                     {
//                         out << "play music \"bgm/" + str + ".ogg\"" << endl
//                             << endl;
//                     }
//                     else if (str.find("se") != string ::npos || str.find("night") != string::npos || str.find("sys") != string::npos)
//                     {
//                         out << "play sound \"se/" + str + ".ogg\"" << endl
//                             << endl;
//                     }
//                     else if (str.find(".s") != string ::npos)
//                     {
//                         out << str << endl
//                             << endl;
//                     }
//                     else
//                     {
//                         if (str.find("JUMP") != string::npos)
//                         {
//                             out << str << endl
//                                 << endl;
//                             continue;
//                         }
//                         out << "voice \"voice/" + str + ".ogg\"" << endl
//                             << endl;
//                     }
//                 }
//                 else if (str.size() != 4)
//                 {
//                     out << str << endl
//                         << endl;
//                 }
//             }
//         }
//         else
//         {
//             cout << "no such file" << endl;
//         }
//     }
//     return 0;
// }
