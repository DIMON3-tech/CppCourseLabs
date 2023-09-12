#pragma once
#include <iostream>

#include <stddef.h>  // for size_t
#include <memory>    // for shared_ptr, __shared_ptr_access, allocator
#include <vector>  // for vector
#include <string>
#include <stdio.h>
#include <chrono>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "Forecast.h"
#include <telegram-bot.h>
#include "get-info.h"

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/flexbox_config.hpp"
#include "ftxui/screen/color.hpp"  
#include "ftxui/component/component.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include <functional>
#include "ftxui/component/component_options.hpp"

std::vector<ftxui::Element> GetConsoleWeather(const std::vector<ForecastResult>& cities_forecast_results, int city_index, int day_index) {
    std::vector<ftxui::Element> elements;

    std::stringstream wind_stringstream;
    std::stringstream rain_stringstream;
    std::stringstream temperature_stringstream;

    const int precision = 1;
    
    float temperature = cities_forecast_results[city_index].temperatures_max[day_index];
    temperature_stringstream << std::fixed << std::setprecision(precision) << (temperature) << " C";
    elements.push_back(ftxui::text(temperature_stringstream.str()));
    
    float rain = cities_forecast_results[city_index].rain[day_index];
    rain_stringstream << "rain: " << std::fixed << std::setprecision(precision) << rain << " mm";
    elements.push_back(ftxui::text(rain_stringstream.str()));

    float wind = cities_forecast_results[city_index].wind[day_index];
    wind_stringstream << "wind: " << std::fixed << std::setprecision(precision) << wind << " km/h";
    elements.push_back(ftxui::text(wind_stringstream.str()));

    return elements;
}

ftxui::Element DayForecast(const int city_selected, const std::vector<ForecastResult>& cities, const int number_days) {
  std::vector<ftxui::Element> elements;
    for(int j = 0; j < cities[city_selected].days.size() && j != number_days; ++j) {
      elements.push_back(ftxui::window(ftxui::text(GetTimeFormat(cities[city_selected].days[j])), ftxui::vbox((GetConsoleWeather(cities, city_selected, j)))));
    }
  
  return ftxui::hbox(elements);
  
}

ftxui::Component CreateEventChecking(const std::vector<ForecastResult>& cities_forecast_results, ftxui::Component& renderer, ftxui::ScreenInteractive& screen, int& selected, int& number_of_days) {
  return ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape) {
        screen.ExitLoopClosure()();
        return true;
    } else if(event == ftxui::Event::Character('n')) {
        if(selected < cities_forecast_results.size() - 1) {
            ++selected;
        }
        return true;
    } else if(event == ftxui::Event::Character('p')) {
        if(selected > 0){
            --selected;
        }
        return true;
    } else if((event == ftxui::Event::Character('+')) or (event == ftxui::Event::Character('='))) {
      ++number_of_days;

      return true;
    } else if(event == ftxui::Event::Character('-') or (event == ftxui::Event::Character('_'))) {
      if(number_of_days > 0) {
        --number_of_days;
      }
      return true;
    } 
    return false;
  });
}

std::vector<std::string> GetListCityNames(const std::vector<ForecastResult>& cities) {
  std::vector<std::string> city_names;
  for(int i = 0; i < cities.size(); ++i) {
    city_names.push_back(cities[i].city_name);
  }
  return city_names;
}

void RunConsoleForecastApplication(std::vector<ForecastResult>& cities_forecast_results, const StartConfig& start_config) {
  int selected = 0;

  auto start = std::chrono::high_resolution_clock::now();


  int number_of_days = cities_forecast_results[0].days.size();
  ftxui::Element forecast_interface = DayForecast(selected, cities_forecast_results, number_of_days);

  auto component = ftxui::Renderer([&] {return forecast_interface; });

  std::vector<std::string> menu_pieces = GetListCityNames(cities_forecast_results);
  
  ftxui::ConstStringListRef c(&menu_pieces);
  
  auto menu = ftxui::Toggle(c, &selected);


  auto renderer = ftxui::Renderer(menu, [&] {
    return ftxui::vbox({ftxui::hbox(DayForecast(selected, cities_forecast_results, number_of_days)),ftxui::separator(),menu->Render()}) |
        ftxui::border;
    });

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  auto event_checking = CreateEventChecking(cities_forecast_results, renderer, screen, selected, number_of_days);

  auto finish = std::chrono::high_resolution_clock::now();
  auto elapsed_seconds = finish - start;
  if(elapsed_seconds.count() >= start_config.frequency) {
    cities_forecast_results = GetAndParseForecastInfo(start_config);
  }

  screen.Loop(event_checking);
}