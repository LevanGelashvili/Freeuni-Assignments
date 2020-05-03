package com.example.weather_application.networking

import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory

object CurrentWeatherClient {

    private const val url = "https://api.openweathermap.org/"

    val getClient: CurrentWeatherApiInterface
        get() {
            val retrofit = Retrofit.Builder()
                .baseUrl(url)
                .addConverterFactory(GsonConverterFactory.create())
                .build()

            return retrofit.create(CurrentWeatherApiInterface::class.java)
        }
}