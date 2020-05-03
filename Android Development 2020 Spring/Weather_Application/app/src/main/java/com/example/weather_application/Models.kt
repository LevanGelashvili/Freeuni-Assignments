package com.example.weather_application

class Models {
    data class Country(val name: String)

    data class CurrentWeather(
    val coord: Coord,
    val weather: List<Weather>,
    val base: String,
    val main: Main,
    val visibility: Int,
    val wind: Wind,
    val clouds: Clouds,
    val dt: Int,
    val sys: CurSys,
    val timezone: Int,
    val id: Int,
    val name: String,
    val cod: Int
)
    data class CurSys(
        val type: Int,
        val id: Int,
        val country: String,
        val sunrise: Int,
        val sunset: Int
    )

    data class Forecast(
        val cod: String,
        val message: Int,
        val cnt: Int,
        val list: List<HourlyWeather>,
        val city: City
)

data class HourlyWeather(
    val dt: Int,
    val main: Main,
    val weather: List<Weather>,
    val clouds: Clouds,
    val wind: Wind,
    val rain: Rain,
    val sys: Sys,
    val dt_txt: String
)

data class City(
    val id: Int,
    val name: String,
    val coord: Coord,
    val country: String,
    val population: Int,
    val timezone: Int,
    val sunrise: Int,
    val sunset: Int
)

data class Main(
    val temp: Double,
    val feels_like: Double,
    val temp_min: Double,
    val temp_max: Double,
    val pressure: Int,
    val sea_level: Int,
    val grnd_level: Int,
    val humidity: Int,
    val temp_kf: Double
)

data class Weather(
    val id: Int,
    val main: String,
    val description: String,
    val icon: String
)

data class Clouds(
    val all: Int
)

data class Wind(
    val speed: Double,
    val deg: Int
)

data class Rain(
    val `3h`: Double
)

data class Sys(
    val pod: String
)

data class Coord(
    val lat: Double,
    val lon: Double
)
}