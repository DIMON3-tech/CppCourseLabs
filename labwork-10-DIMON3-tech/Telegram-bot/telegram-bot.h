#pragma once
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <Forecast.h>

#include <stdio.h>
#include <tgbot/tgbot.h>
#include <iomanip>
using namespace TgBot;

std::string GetToken(const std::string token_filename) {
    std::string token;

    std::ifstream fin(token_filename);
    if(!fin.is_open()) {
        std::cout << "file with token was not open!" << std::endl;
        exit(0);
    }
    std::getline(fin, token);
    return token;
}

void RunTelegramBot(std::vector<ForecastResult>& cities_forecast_results, const std::string token) {

    TgBot::Bot bot(token);

    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
    std::vector<InlineKeyboardButton::Ptr> buttons;
    InlineKeyboardButton::Ptr weather(new InlineKeyboardButton), get_custom_weather(new InlineKeyboardButton);
    weather->text = "GET WEATHER FROM JSON";
    get_custom_weather->text = "GET CUSTOM WEATHER";
    weather->callbackData = "JSON";
    get_custom_weather->callbackData = "CUSTOM";
    buttons.push_back(weather);
    buttons.push_back(get_custom_weather);
    keyboard->inlineKeyboard.push_back(buttons);

    const std::string path_to_picture = "//home//dimon_limon//lab10//welcome3.jpg";
    const std::string mime_type = "image/jpeg";

    bool test_text_state = false;

    std::vector<ForecastResult> custom_cities;
    int counter_custom_cities = 0;

    bot.getEvents().onCommand("start", [&bot, &path_to_picture, &mime_type](TgBot::Message::Ptr message) {
        TgBot::MessageEntity::Type bold;
        bot.getApi().sendMessage(message->chat->id, "Hi, " + message->chat->firstName + " ! Thank you for using this C++ application! Read instructions before using the bot");
        bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(path_to_picture, mime_type));
    });

    bot.getEvents().onCommand("weather", [&bot, &keyboard](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Choose how to get weather:", false, 0, keyboard);
    });

    bot.getEvents().onCommand("creators", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "This application was created by https://github.com/DIMON3-tech. To contact with developer write to https://t.me/itmo_basketball in Telegram");
    });

    bot.getEvents().onCallbackQuery([&bot, &cities_forecast_results, &test_text_state](TgBot::CallbackQuery::Ptr query) {
        if(query->data == "CUSTOM") {
            bot.getApi().sendMessage(query->message->chat->id, "Type city name: ");
            test_text_state = true;

        } else {
            for(int i = 0; i < cities_forecast_results.size(); ++i) {
                bot.getApi().sendMessage(query->message->chat->id, GetWeather(cities_forecast_results, i));
            }
        }
    });

    bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());

        if (test_text_state) {
            ForecastApiInformation forecast_api_information;
            forecast_api_information.city_name = message->text;
            forecast_api_information.forecast_days = 7;

            try {
                GetCitySettingsInformation(message->text, forecast_api_information);
            } catch(nlohmann::json_abi_v3_11_2::detail::out_of_range& e) {
                std::cout << e.what();
                bot.getApi().sendMessage(message->chat->id, "Sorry, it seems you typed wrong city name 😥. Try another one (just type /weather again) ");
            }

            ForecastResult forecast_result;
            try {
                forecast_result = GetForecastInformation(forecast_api_information, "Europe%2FMoscow");
            } catch(nlohmann::json_abi_v3_11_2::detail::out_of_range& e) {
                std::cout << e.what();
                bot.getApi().sendMessage(message->chat->id, "Sorry, it seems you typed wrong city name 😥. Try another one (just type /weather again) ");
            }

            custom_cities.push_back(forecast_result);

            std::string string = GetWeather(custom_cities, counter_custom_cities);
            ++counter_custom_cities;


            bot.getApi().sendMessage(message->chat->id, "weather information for " + string);
            test_text_state = false;
            return;
        }
    });


    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
}

