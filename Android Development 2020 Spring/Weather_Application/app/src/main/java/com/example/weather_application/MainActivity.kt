package com.example.weather_application

import android.os.Bundle
import android.widget.Toast
import androidx.fragment.app.FragmentActivity
import androidx.viewpager.widget.ViewPager
import com.example.weather_application.Models.Country
import com.example.weather_application.networking.CountryClient
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response

class MainActivity : FragmentActivity() {

    private lateinit var countries: List<Country>
    private lateinit var viewPager: ViewPager

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        getCountryList()
    }

    private fun getCountryList() {
        val call: Call<List<Country>> = CountryClient.getClient.getCountryList()
        call.enqueue(object : Callback<List<Country>> {

            override fun onResponse(
                call: Call<List<Country>>?,
                response: Response<List<Country>>?
            ) {
                if (response!!.isSuccessful) {
                    initSuccessfulResponse(response)
                } else {
                    Toast.makeText(
                        this@MainActivity,
                        "Could not retrieve countries. Try again.",
                        Toast.LENGTH_SHORT
                    ).show()
                }
            }

            override fun onFailure(call: Call<List<Country>>?, t: Throwable?) {
                Toast.makeText(this@MainActivity, "${t!!.message}", Toast.LENGTH_SHORT).show()
            }
        })
    }

    private fun initSuccessfulResponse(response: Response<List<Country>>) {
        countries = response.body()!!
        viewPager = findViewById(R.id.viewPager)
        viewPager.adapter = ViewPagerAdapter(supportFragmentManager, 0, countries)
    }
}
