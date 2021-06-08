#include <iostream>
#include "curl/curl.h"  // is necessary
#include "string"
#include<fstream>
#include <vector>
#include<exception>
#include<chrono>
#include<thread>

using namespace std;

/* Простая программа для массовых рассылок смс на Goip32 с использованием WebApi



*/

uint64_t msgcounter;
string curlBuffer;
size_t curlWriteFunc(char *data,size_t size,size_t nmemb, string *buffer){
    size_t result = 0;

    if(buffer != NULL){
            buffer->clear();
            buffer->append(data, size * nmemb);
            result = size * nmemb;
    }
    return result;
}

template <typename T>
void LoadNumbers(const string & filename, T &V){

    ifstream file (filename);
    if(!file){
        cout<<"Something is wrong with file"<<endl;
        exit(1);

    }

    string line;
    while(getline(file,line)){
        V.push_back(line);
    }
    cout<< V.size()<<" Total numbers is laoded "<<endl;
}


bool SendRequest(const string &ip,const string &username, const string &pass,
                 const int &count, const string &number,const string &mesage){


  string line = to_string(count);
  //forming url
  string url = ip+"/default/en_US/send.html?u="+username+"&p="+pass+
           "&l="+line+"&n="+number+"&m="+mesage;
  const char * adress = url.c_str();
  //буфер для сохраненияы ошибок
  char curlErrorBuffer[CURL_ERROR_SIZE];

  //Инициализируем Курл
  CURL *curl = curl_easy_init();

  // проверяем инициализацию
  if (curl){

      // Задаем опции курлу
      // Буфер для ошибок с вишеописаной функцией
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrorBuffer );
      // задаем адресс

      curl_easy_setopt(curl, CURLOPT_URL, adress);
      //задаем юзер агент
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0");
      //задаем реферал
       curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1 );
      // переходим по локейшену задоному в заголовку
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,1);
     //куки ???
      curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
      // НЕ ПРОВЕРЯТЬ СЕРТИФИКАТ
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
      //указываем что полученые даные нужно записать  в буфер используя функцию
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunc);
     //выполянем сам запрос
      CURLcode curlResult = curl_easy_perform(curl);
     // закрываем сеанс
      curl_easy_cleanup(curl);
      //проверяем резултат
      if (curlResult == CURLE_OK){
          cout<<" OK"<<endl;
          cout<<"************************************************;"<<endl;
          cout<< curlBuffer << endl;
          cout<<"************************************************;"<<endl;

          if (curlBuffer.find("ERROR")!=curlBuffer.npos){
              cout<<"###############ATENTION#######################"<<endl;
              cout<<"            !!  RESENDING !!                  "<<endl;
              cout<<"      "<<number<<" slot: "<<count+1<<endl;
              cout<<"###############ATENTION#######################"<<endl;
              return true;
          }
      }
      else {

          cout << "FuckUp("<<curlResult<<"); LOL"<< curlErrorBuffer << endl;
          cout << "Something is wrong with GSM box"<<endl;
          exit(1);
      }
  }
    msgcounter++;
    cout<< "Total sms sent "<<msgcounter<<endl;
    return false;
}

void LoadMess(const string &filename,string &sms){
    ifstream file (filename);
    if(!file){
        cout<<"Wrong file name or file not exist"<<endl;
        exit(1);

    }
    string line;
    while(getline(file,line)){
        sms=line;
    }
}

int main()
{
    msgcounter = 0;
    cout<<endl<<endl<<

"             88                                              "<<endl
<<"             \"\"                          ,d                              "<<endl
<<"             88                                                  "<<endl
<<"       ,adPPYba,  ,adPPYb,d8 88       88 88 8b,dPPYba, MM88MMM   "<<endl
<<"       I8[    \"\" a8\"    `Y88 88       88 88 88P'   \"Y8   88      " <<endl
<<"        `\"Y8ba,  8b       88 88       88 88 88           88       " <<endl
<<"       aa    ]8I \"8a    ,d88 \"8a,   ,a88 88 88           88,     " <<endl
<<"       `\"YbbdP\"'  `\"YbbdP'88  `\"YbbdP'Y8 88 88           \"Y888   " <<endl
<<"       88                                                        "<< endl
<<"       88                                                        " <<endl<<endl<<

            endl<< "Enter file name with phone numbers"<<endl;
    string filename;
    cin>>filename;
    vector<string> numbers ;
    LoadNumbers(filename,numbers);
    cout<<"Type a quantity of sim inserted in goip32"<<endl;
    int Q;
    if(Q<1 || Q > 32){
        cout<<"Wrong quantity"<<endl;
    }
    cin>>Q;
    cout<<"Type a user name "<<endl;
    string username;
    cin>>username;
    cout<<"Type password"<<endl;
    string pass;
    cin>>pass;
    cout<<"Type an ip adress"<<endl;
    string adress;
    cin>>adress;
    int time;
    cout<<"Type pause lenght in seconds"<<endl;
    cin>>time;
    cout<<"Type file name with message to send"<<endl;
    string smsfile;
    cin>>smsfile;
    string mesage;
    LoadMess(smsfile,mesage);
    chrono::seconds pause(time);
    cout<<"Starting to send sms to  "
       <<numbers.size()<<" phone numbers"<<" Good Luck !!!"<<endl;
    int count = 1;
    for(const auto &number:numbers){

        do{
            if (count >= Q){
                count = 0;
            }
           count++;
        }while (SendRequest(adress,username,pass,count,number,mesage));
        this_thread::sleep_for(pause);
    }
    return 0;
}



