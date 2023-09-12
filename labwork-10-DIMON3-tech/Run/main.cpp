#include "Forecast.h"
#include "telegram-bot.h"
#include "console-forecast.h"
#include "get-info.h"

// filesystem

using namespace ftxui;
using namespace TgBot;

int main() {
  const std::string token_path = "/home/dimon_limon/lab10/token.txt";
  const std::string json_path = "//home//dimon_limon//lab10//Classes//config.json";

  StartConfig start_config = ParseStartInfo(json_path);

  std::vector<ForecastResult> cities_forecast_results = GetAndParseForecastInfo(start_config);

  std::string token = GetToken(token_path);

  RunTelegramBot(cities_forecast_results, token);

  RunConsoleForecastApplication(cities_forecast_results, start_config);
}