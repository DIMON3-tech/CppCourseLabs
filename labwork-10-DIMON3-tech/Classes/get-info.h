#pragma once
#include <iostream>
#include <stdio.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "Forecast.h"
#include <telegram-bot.h>
#include <string>
#include "console-forecast.h"

#include <stddef.h>
#include <memory>
#include <vector>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/flexbox_config.hpp"
#include "ftxui/screen/color.hpp"  
#include <functional>

#include "ftxui/component/component_options.hpp"

using namespace ftxui;
using namespace TgBot;

std::vector<ForecastResult> GetAndParseForecastInfo(const StartConfig& start_config) {
    std::vector<ForecastResult> cities_forecast_results;

    for (int i = 0; i < start_config.cities.size(); ++i) {
        ForecastApiInformation forecast_api_information;
        forecast_api_information.city_name = start_config.cities[i];
        forecast_api_information.forecast_days = start_config.forecast_days;

        GetCitySettingsInformation(start_config.cities[i], forecast_api_information);

        cities_forecast_results.push_back(GetForecastInformation(forecast_api_information, "Europe%2FMoscow"));       
    }
    return cities_forecast_results;
}