package com.example.weather_application.networking

import com.example.weather_application.Models
import retrofit2.Call
import retrofit2.http.GET

interface CountryApiInterface {
    @GET("rest/v2/all?fields=name")
    fun getCountryList(): Call<List<Models.Country>>
}