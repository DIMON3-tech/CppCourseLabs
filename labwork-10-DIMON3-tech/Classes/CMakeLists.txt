add_executable(Forecast /home/dimon_limon/lab10/Run/main.cpp Forecast.h /home/dimon_limon/lab10/Classes/get-info.h /home/dimon_limon/lab10/Telegram-bot/telegram-bot.cpp /home/dimon_limon/lab10/Telegram-bot/telegram-bot.h /home/dimon_limon/lab10/Classes/console-forecast.h)

target_link_libraries(Forecast PRIVATE cpr::cpr nlohmann_json::nlohmann_json /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
 target_link_libraries(Forecast
         PRIVATE ftxui::dom
         PRIVATE ftxui::component
         PRIVATE ftxui::screen
         )

