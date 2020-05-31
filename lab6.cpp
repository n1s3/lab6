#include <iostream>
#include <string>
#include <fstream>

#include "json.hpp"
#include "httplib.h"
#include "include/HC.cfg.h"

using json = nlohmann::json;

void replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos) {
        return;
    }
    str.replace(start_pos, from.length(), to);
}

void gen_response(const httplib::Request& req, httplib::Response& res)
{
    std::string s;
    httplib::Client cli("api.openweathermap.org", 80);
    auto result = cli.Get("/data/2.5/forecast?id=693805&APPID=285fe41b4a6382809fe82594ad125003&units=metric");
    json j = json::parse(result->body);
    std::cout << j.at("city").at("name") << std::endl;
    int start = j.at("list")[0].at("dt");

    std::ifstream ifs("Weather.html");
    getline(ifs, s, '\0');
    ifs.close();
    replace(s, "{city.name}", j.at("city").at("name"));
    for (auto& el : j.at("list").items())
    {
        if (el.value().at("dt") >= start)
        {
            replace(s, "{list.dt}", el.value().at("dt_txt"));
            replace(s, "{list.weather.icon}", el.value().at("weather")[0].at("icon"));
            replace(s, "{list.main.temp}", to_string(el.value().at("main").at("temp")));
            start += 86400;
        }
    }
    res.set_content(s, "text/html");
}

int main()
{
    std::cout << "SERVER RUN!" << std::endl;
    httplib::Server svr; //Создаём сервер
    svr.Get("/", gen_response);
    svr.listen("localhost", 3000); //localhost 
}