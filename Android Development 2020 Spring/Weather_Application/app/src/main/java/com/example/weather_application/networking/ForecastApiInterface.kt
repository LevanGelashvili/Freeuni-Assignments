package com.example.weather_application.networking

import com.example.weather_application.Models
import retrofit2.Call
import retrofit2.http.GET
import retrofit2.http.Query

interface ForecastApiInterface {
    @GET("data/2.5/forecast")
    fun getForecast(@Query("q") country: String?, @Query("appid") api_key: String): Call<Models.Forecast>
}