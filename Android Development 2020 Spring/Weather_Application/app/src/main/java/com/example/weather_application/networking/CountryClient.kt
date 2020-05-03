package com.example.weather_application.networking

import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory

object CountryClient {

    private const val url = "https://restcountries.eu/"

    val getClient: CountryApiInterface
        get() {
            val retrofit = Retrofit.Builder()
                .baseUrl(url)
                .addConverterFactory(GsonConverterFactory.create())
                .build()

            return retrofit.create(CountryApiInterface::class.java)
        }
}