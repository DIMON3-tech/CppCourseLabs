#pragma once
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>

struct ForecastResult {
    std::string city_name;
    std::vector<std::string> days; // yyyy-mm-dd
    std::vector<float> temperatures_max; // C
    std::vector<float> temperatures_min; // C
    std::vector<float> rain; // mm
    std::vector<float> wind; // km/h
};

const std::vector<std::string> temperature_stickers = {" 🏖️ ", " ☀️ ", " 🌬️ ", " ❄️ "};
const std::vector<std::string> rain_stickers {" 🌞 ", " 🌦️ ", " 🌧️ ", " 💧 ", " 💦 "};

std::string CompareStickerToTemperature(float temperature) {
    const float heat = 35.0;
    const float sun = 25.0;
    const float wind = 10.0;
    const float snow = 0.0;

    if(temperature >= heat) {
        return temperature_stickers[0]; // heat sun
    }
    else if(temperature >= sun) {
        return temperature_stickers[1]; // sun
    }
    else if(temperature >= wind) {
        return temperature_stickers[2]; // wind
    }
    else {
        return temperature_stickers[3]; // snow
    }
}

std::string CompareStickersToRain(float rain) {
    const float no_rain = 0.0;
    const float weak = 1.0;
    const float medium = 4.0;
    const float strong = 16.0;
    const float rainfall = 40.0;

    if(rain >= rainfall) return rain_stickers[4]; // rainfall
    else if(rain >= strong) return rain_stickers[3]; // strong rain
    else if(rain >= medium) return rain_stickers[2]; // medium rain
    else if(rain >= weak)return rain_stickers[1]; // weak rain
    else return rain_stickers[0]; // no rain
}

std::string GetTimeFormat(const std::string old_format) {
    std::tm date = {};
    std::istringstream stream(old_format);
    stream >> std::get_time(&date, "%Y-%m-%d");

    char buffer[80];
    std::strftime(buffer, 80, "%d %B %Y", &date);
    std::string new_format = buffer;
    return new_format;
}

std::string GetWeather(std::vector<ForecastResult>& cities_forecast_results, int city_index) { // for Telegram bot
    std::stringstream answer;

    const int precision = 1;
    
    answer << '*' + cities_forecast_results[city_index].city_name + '*' + '\n' + '\n'; // name of city

    for(int j = 0; j < cities_forecast_results[city_index].days.size(); ++j) {
        answer << GetTimeFormat(cities_forecast_results[city_index].days[j]) + '\n';
        float temperature = cities_forecast_results[city_index].temperatures_max[j];
        answer << CompareStickerToTemperature(temperature) << std::fixed << std::setprecision(precision) << (temperature) << "℃" <<'\n';

        float rain = cities_forecast_results[city_index].rain[j];
        answer << "rain: " << CompareStickersToRain(rain) << std::fixed << std::setprecision(precision) << (rain) << " mm" << '\n';

        float wind = cities_forecast_results[city_index].wind[j];
        answer << "wind: " << std::fixed << std::setprecision(precision) << (wind) << " km/h" << '\n' << '\n';
    }
    return answer.str();
}

std::string ConsoleGetWeather(std::vector<ForecastResult>& cities_forecast_results, int city_index, int day_index) { // for Console application
    std::stringstream answer;

    const int precision = 1;

    answer << GetTimeFormat(cities_forecast_results[city_index].days[day_index]) + " ";
    float temperature_max = cities_forecast_results[city_index].temperatures_max[day_index];
    float temperature_min = cities_forecast_results[city_index].temperatures_min[day_index];
    answer << CompareStickerToTemperature(temperature_max) + "max: " << std::fixed << std::setprecision(precision) << (temperature_max) << "℃";
    answer << CompareStickerToTemperature(temperature_min) + "min: " << std::fixed << std::setprecision(precision) << (temperature_max) << "℃";
    return answer.str();
}

struct ForecastApiInformation {
    std::string city_name;
    float latitude;
    float longitude;
    int forecast_days; // number of days for which show weather

    ForecastApiInformation(float latitude_from_api, float longitude_from_api) : latitude(latitude_from_api), longitude(longitude_from_api) {}

    ForecastApiInformation() = default;
};

struct StartConfig {
    float frequency;
    int forecast_days;
    std::vector<std::string> cities;

    StartConfig(float config_frequency, int config_forecast_days, std::vector<std::string>&& config_cities) : frequency(config_frequency),
    forecast_days(config_forecast_days), cities(config_cities) {}


};

StartConfig ParseStartInfo(const std::string filename) {
    std::ifstream fin(filename);
    
    nlohmann::json data = nlohmann::json::parse(fin);
    StartConfig start_config(data["frequency"], data["forecast_days"], data["cities"]);
    return start_config;
}

class AutorizationSettings {
private:
    const std::string api = "GqEFsFCgpvTUPT/UAcZ1dQ==BxoIKfFznuaVDihW";
    const std::string api_setting = "X-Api-Key";
public:
    std::string GetApi() const {
        return api;
    }

    std::string GetApiSettingInfo() const {
        return api_setting;
    }
};

void GetCitySettingsInformation(const std::string& city_name, ForecastApiInformation& forecast_api_information) {
    AutorizationSettings autorization_settings;

    const std::string main_part_http = "https://api.api-ninjas.com/v1/city?name=";

    std::string url = main_part_http + city_name;

    cpr::Response response = cpr::Get(cpr::Url{url},
                               cpr::Header{{autorization_settings.GetApiSettingInfo(), autorization_settings.GetApi()}},
                               cpr::Authentication{"dimabelunin7@gmail.com", "7657575755321170*", cpr::AuthMode::BASIC},
                               cpr::Parameters{});

    try {
        nlohmann::json data = nlohmann::json::parse(response.text);
        forecast_api_information.latitude = data.at(0)["latitude"].get<float>();
        forecast_api_information.longitude = data.at(0)["longitude"].get<float>();
    } catch(nlohmann::json_abi_v3_11_2::detail::out_of_range& e) {
        std::cout << "parsing of data from GetCitySettingsInformation is incorrect " << std::endl;
        std::cout << e.what();
    }

    
}

void StoreInformationAfterForecastApi(const nlohmann::json& data, const ForecastApiInformation& forecast_api_information, ForecastResult& forecast_result) {
    forecast_result.city_name = forecast_api_information.city_name;
    for (int i = 0; i < forecast_api_information.forecast_days; ++i) {
        forecast_result.days.push_back(data["daily"]["time"][i]);
        forecast_result.temperatures_max.push_back(data["daily"]["temperature_2m_max"][i]);
        forecast_result.temperatures_min.push_back(data["daily"]["temperature_2m_min"][i]);
        forecast_result.rain.push_back(data["daily"]["rain_sum"][i]);
        forecast_result.wind.push_back(data["daily"]["windspeed_10m_max"][i]);
    }
}

ForecastResult GetForecastInformation(const ForecastApiInformation& forecast_api_information, const std::string time_zone) {
    const std::string main_part_forecast_url = "https://api.open-meteo.com/v1/forecast?";

    std::string latitude_string = "latitude=" + std::to_string(forecast_api_information.latitude);
    std::string longitude_string = "longitude=" + std::to_string(forecast_api_information.longitude);
    std::string forecast_url = main_part_forecast_url + latitude_string + '&' + longitude_string + "&daily=temperature_2m_max,temperature_2m_min,rain_sum,windspeed_10m_max&forecast_days=16&timezone=" + time_zone;;

    cpr::Response response = cpr::Get(cpr::Url{forecast_url},
                           cpr::Parameters{});

    
    //const int http_ok = 200;
              
    if (response.status_code != 200) {
        std::cout << "Name of city is incorrect, cannot get latutide and longitude!";
        exit(0);
    }

    nlohmann::json data = nlohmann::json::parse(response.text);
    ForecastResult forecast_result;
    StoreInformationAfterForecastApi(data, forecast_api_information, forecast_result);
    return forecast_result;
    
}

