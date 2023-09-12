target_link_libraries(Forecast PRIVATE cpr::cpr nlohmann_json::nlohmann_json /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
 target_link_libraries(Forecast
         PRIVATE ftxui::dom
         PRIVATE ftxui::component # Not needed for this example.
         PRIVATE ftxui::screen
         )

