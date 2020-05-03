package com.example.weather_application.networking

import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory

object ForecastClient {

    private const val url = "https://api.openweathermap.org/"

    val getClient: ForecastApiInterface
        get() {
            val retrofit = Retrofit.Builder()
                .baseUrl(url)
                .addConverterFactory(GsonConverterFactory.create())
                .build()

            return retrofit.create(ForecastApiInterface::class.java)
        }
}